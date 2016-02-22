#ifndef NATIVE_H_
#define NATIVE_H_

#include "jsfunction.h"
#include "vm.h"
#include "jsobject.h"
#include "jsarray.h"
#include "parser.h"
#include "lexer.h"

#include <cstdio>
#include <string>
#include <vector>

class NativeInstaller {
public:
  static void InstallFunction(std::string name, std::string proto,
                              NATIVE_HANDLE_TYPE handle) {
    Lexer *lexer = new Lexer(proto);
    std::shared_ptr<AstNode> proto_node;
    if (!Expression::FunctionStatement(lexer, proto_node, true, handle)) {
      printf("%s\n", Expression::err_msg_.c_str());
      Expression::err_msg_.clear();
      return;
    }
    Heap::heap.PushVariable({name, proto_node});
    delete lexer;
  }

  static void InstallNativeObject() {}
};

#endif // NATIVE_H_