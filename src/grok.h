///////////////////////////////////////////////////////////////////////////////
//
//    +-----------------------------------------------------------+
//    |          Grok : A Naive JavaScript Interpreter            |
//    +-----------------------------------------------------------+
//
// Copyright 2015 Pushpinder Singh
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
//  \file:  grok.h
//  \description:  definition of Grok class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GROK_H_
#define GROK_H_

#include "interpreter.h"
#include "jsobject.h"
#include "symtable.h"
#include "lexer.h"
#include "parser.h"
#include "bytecode.h"
#include "vm.h"
#include "codegen.h"
#include "timer.h"

#include <chrono>
#include <cstdio>
#include <string>

#define DEBUG_OPT(opt)  options.push_back(opt)

class Grok {
public:
  Grok() :
    interpreter_(), expression_(), is_debug_(false),
    lexer_(nullptr) {
  }

  void InteractiveShell() {
    char buffer[1024];
    std::printf(">> ");
    std::string bufs;
    std::fgets(buffer, 1024, stdin);
    bufs = buffer;
    bufs.push_back(';');
    expression_ = nullptr;
    Lexer *lexer = new Lexer(bufs);
    bool result = Expression::Parse(lexer, expression_);
    if (result) {
      for (auto link : expression_->links_)
        interpreter_.ExecuteHighLevelStatement(link);
    }
    else {
      std::printf("%s\n", Expression::err_msg_.c_str());
      Expression::err_msg_.clear();
    }
    delete lexer;
    std::printf("\n");
  }


  void WorkAndDebug() {
    Timer t;
    char buffer[1024];
    std::printf(">> ");
    std::string bufs;
    std::fgets(buffer, 1024, stdin);
    bufs = buffer;
    bufs.push_back(';');
    expression_ = nullptr;
    Lexer *lexer = new Lexer(bufs);
    std::cout << "Parsing...   ";
    t.Start();
    bool result = Expression::Parse(lexer, expression_);
    t.Stop();
    std::cout << "[time: " << t.ElapsedTimeInMicroseconds() / 1000.0
      << "ms]\n";
    if (result) {
      std::cout << "Executing...     ";
      t.Start();
      for (auto link : expression_->links_)
        interpreter_.ExecuteHighLevelStatement(link);
      t.Stop();
      std::cout << "\n[Execution time: "
        << t.ElapsedTimeInMicroseconds() / 1000.0
        << "ms]\n";
    }
    else {
      std::printf("%s\n", Expression::err_msg_.c_str());
      Expression::err_msg_.clear();
    }
    delete lexer;
    std::printf("\n");
  }


  void Loadfile(const char *file) {
    bool f = true;
    Lexer *lexer = new Lexer(file, f);
    if (f) {
      bool result = Expression::Parse(lexer, expression_);
      if (result) {
        for (auto link : expression_->links_)
          interpreter_.ExecuteHighLevelStatement(link);
      }
      else {
        std::printf("%s\n", Expression::err_msg_.c_str());
        Expression::err_msg_.clear();
      }
    }
    else {
      printf("%s", lexer->GetErrorReport().c_str());
    }
    delete lexer;
    std::printf("\n");
  }

  std::string GetGrokVersion() const {
    return std::string("Grok (version 1.0): A simple JavaScript Interpreter");
  }

  void PrintIntroduction() {
    std::printf("%s\nType print(...) to print something... ",
                GetGrokVersion().c_str());
    is_debug_ ? std::printf("[debug mode]\n") : std::printf("\n");
  }

  void ShowHelp() {
    std::printf("%s\n\n"
                "     Usage:   grok [options] [file-name]\n"
                "\n"
                "     options:\n"
                "             -d    : print debug information.\n"
                "             -h    : print this information and exit.\n"
                "             -v    : print the version of the Grok.\n"
                "             -n    : run with virtual machine - unstable\n"
                "                     many features will not work",
                GetGrokVersion().c_str());
  }

  bool SetCommandOptions(std::vector<std::string>& options) {
    for (auto &option : options) {
      if (option == "-d") {
        is_debug_ = true;
        return true;
      }
      else if (option == "-h") {
        ShowHelp();
        return false;
      }
      else if (option == "-t") {
        RunTest();
        return false;
      }
      else if (option == "-v") {
        std::printf("%s\n", GetGrokVersion().c_str());
        return false;
      }
      else if (option == "-n") {
        std::printf("%s\n", GetGrokVersion().c_str());
        new_ = 1;
        return true;
      }
      else if (option[0] == '-') {
        std::printf("Bad arguments\n"
                    " Use -h to print help\n");
      }
      else {
        // it should be a file-name
        Loadfile(option.c_str());
        return false;
      }
    }
  }

  void RunTest() {
  }

  void PromptInput() {
    std::fprintf(stdout, ">> ");
    std::getline(std::cin, program_);
  }

  void RunWithNewInterpreter() {
    PromptInput();
    MakeLexer(&lexer_, program_);
    std::shared_ptr<AstNode> parsed;
    if (!Expression::Parse(lexer_, parsed)) {
      if (!Expression::err_msg_.empty()) 
        printf("%s\n", Expression::err_msg_.c_str());
      Expression::err_msg_.clear();
      return;
    }
    BytecodeProgram<Register, Bytecode> bytecodes;
    Code::CodeGen codegen;
    if (!Code::GenerateCode(&codegen, &bytecodes, parsed, &machine_)) {
      printf("Code Generation failed\n");
      return;
    }
    bytecodes.print(std::cout);
    bytecodes.text_.push_back(B::hlt());
    machine_.prepare_machine(&bytecodes);
    machine_.execute();
    try {
      Register result = machine_.StackTop();
      result.print(std::cout);
      printf("\n");
    }
    catch (...) {
      printf("undefined\n");
      return;
    }
  }

  void Run(int argc, char *argv[]) {

    std::vector<std::string> options;
#ifdef DEBUG_OPT
    DEBUG_OPT(std::string("-n"));
#endif
    if (argc > 1) {
      for (int i = 1; i < argc; i++) {
        options.push_back(std::string(argv[i]));
      }
      bool result = SetCommandOptions(options);
      if (!result) return;
    }
    PrintIntroduction();
    Heap::heap.CreateScope();
    while (true) {
      if (new_) {
        RunWithNewInterpreter();
      }
      else {
        is_debug_ ? InteractiveShell() : WorkAndDebug();
      }
    }
  }

private:
  Interpreter interpreter_;
  Machine machine_;
  Lexer *lexer_;
  std::string program_;
  std::shared_ptr<AstNode> expression_;
  bool is_debug_;
  bool new_;
};

#endif // RUNNER_H_
