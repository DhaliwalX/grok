#include "parser/parser.h"
#include "vm/context.h"
#include "vm/printer.h"
#include "vm/codegen.h"
#include "vm/instruction-list.h"
#include "object/jsbasicobject.h"
#include "vm/vm.h"
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

    std::cout << parser << std::endl;
    auto AST = parser.ParsedAST();
    CodeGenerator CG;
    CG.Generate(AST.get());

    auto IR = CG.GetIR();
    Printer P{ std::cout };
    P.Print(IR);
    std::cout << std::string(75, '=') << std::endl;
    
    // execute the code
    auto TheVM = CreateVM(GetGlobalVMContext());
    TheVM->SetCounters(IR->begin(), IR->end());
    TheVM->Run();

    Value Result = TheVM->GetResult();
    auto O = GetObjectPointer<grok::obj::JSObject>(Result);
    std::cout << O->ToString() << std::endl;
    return 0;
}
