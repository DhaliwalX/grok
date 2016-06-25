#include "parser/parser.h"
#include "vm/context.h"
#include "vm/printer.h"
#include "vm/codegen.h"
#include "vm/instruction-list.h"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char *argv[]) {
    std::string in;
    bool result = false;
    using namespace grok::vm;
    using namespace grok::parser;

    InitializeVMContext();
    std::cout << ">> ";
    std::getline(std::cin, in);
    auto lex = std::make_unique<Lexer>(in);
    
    grok::parser::GrokParser parser{ std::move(lex) };
    result = parser.ParseExpression();

    std::cout << std::string(75, '=') << std::endl;
    if (!result) {
        std::cout << "Errors occurred" << std::endl;
        return -1;
    }

    auto AST = parser.ParsedAST();
    CodeGenerator CG;
    CG.Generate(AST.get());

    auto IR = CG.GetIR();
    Printer P{ std::cout };
    P.Print(IR);
    return 0;
}
