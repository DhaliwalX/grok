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
//  \file:  excpetions.h
//  \description:  definition of some classes useful for throwing exceptions
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include "token.h"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

class Token;
class Lexer;

// main exception class, all other exception classes
// are derived from this class
class Exception : std::exception {
public:
  Exception(const std::string &msg) :
    std::exception(), msg_(msg) {
  }

  Exception(const char *msg) :
    std::exception(), msg_(msg) {
  }

  Exception() :
    std::exception(), msg_() {
  }

  const char * what() const noexcept override {
    return msg_.c_str();
  }
private:
  std::string msg_;
};


class LexerException : public Exception {
public:
  LexerException(std::string msg, Token *&tok) :
    Exception(msg), tok_(tok) {
  }

  LexerException(std::string msg) :
    Exception(msg), tok_(nullptr) {
  }

  LexerException() :
    Exception(), tok_(nullptr) {
  }

  const char *what() const noexcept override { // returns the msg
    std::ostringstream os;
    if (tok_ != nullptr)
      os << Exception::what() << " at Line no.: "
      << tok_->position_.row_ << " and Column no.: "
      << tok_->position_.col_ << "\n";
    else
      os << Exception::what();
    return ("SyntaxError: " + std::string(Exception::what())).c_str();
  }
private:
  Token *tok_;
};

class JsonException : public Exception {
public:
  JsonException(std::string &msg) :
    Exception(msg) {
  }

  JsonException() :
    Exception(std::string("Unrecognized Error")) {
  }

  const char * what() const noexcept override {
    return ("JsonError: " + std::string(Exception::what())).c_str();
  }
};

class SyntaxException : public Exception {
public:
  SyntaxException(const std::string &str, Token *tok)
    : Exception(str), tok_(tok) {
  }

  SyntaxException()
    : Exception(std::string()), tok_(nullptr) {
  }

  const char *what() const noexcept override {
    return ("SyntaxError:\n" + std::string(Exception::what()) +
            " at Line: " + std::to_string(tok_->position_.row_)).c_str();
  }

  ~SyntaxException() {
    delete tok_;
  }

private:
  Token *tok_;
};


class NanException : public Exception {
public:
  NanException(const std::string &msg) :
    Exception(msg) {
  }

  NanException() :
    Exception() {
  }

  const char *what() const noexcept override { // message function
    return ("NaNError: " + std::string(Exception::what())).c_str();
  }
};

#endif
