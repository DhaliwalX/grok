#include "astnode.h"


std::string expression_type[] = {
    "undefined", /* for undefined type */
    "_empty", "_primary", "_member", "_argument", "_array_access",
    "_constructor", "_constructorcall", "_declaration", "_declarations",
    "_unary", "_factor", "_functioncall", "_term", "_shift", "_relational",
    "_equality", "_bitand", "_bitxor", "_bitor", "_and", "_or", "_conditional",
    "_assignment", "_single", "_break", "_continue", "_compound", "if",
    "ifelse", "for", "for_in", "while", "_with", "_return", "_function",
    "_native" /* tells whether the given node contains the native call */
};

void AstNode::print(std::ostream &os, int tab)
{
    os << indent(tab, ' ') << "Expression Type: " << expression_type[(int)expression_type_]
          << std::endl;
    os << indent(tab, ' ') << "Variable: " << variable_ << std::endl;
    os << indent(tab, ' ') << "Relation1: " << token_type[relation1_] << std::endl;
    os << indent(tab, ' ') << "Relation2: " << token_type[relation2_] << std::endl;
}

void PrintAST(std::shared_ptr<AstNode> node, std::ostream &os,
                     int tab) {
  node->print(os, tab++);
  for (auto &child : node->links_) {
    PrintAST(child, os, tab);
  }
}

void PrintASTObject(std::shared_ptr<AstNode> node, int tab) {
  while (tab--)
    printf("  ");
  if (!node.get() || node->obj_.empty()) {
    printf("Undefined");
  } else
    printf("%s", node->obj_.as<JSBasicObject>()->ToString().c_str());
}

