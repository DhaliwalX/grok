#ifndef PARSER_H_
#define PARSER_H_

#include "parser/common.h"
#include "parser/expression.h"
#include "lexer/lexer.h"

namespace grok { namespace parser {

// forward declaration needed by GrokParser::ParsePrototype
class FunctionPrototype;

/// GrokParser ::= main parser class
class GrokParser {
    friend std::ostream &operator<<(std::ostream &os, GrokParser &parser);
public:
    GrokParser() = default;

    GrokParser(std::unique_ptr<Lexer> lex) :
        lex_(std::move(lex)) 
    {
        lex_->advance();
    }

    std::unique_ptr<Expression> ParsePrimary();
    // std::unique_ptr<Expression> ParseMember();
    std::unique_ptr<Expression> ParseBinary();

    std::unique_ptr<Expression>
    ParseBinaryRhs(int prec, std::unique_ptr<Expression> lhs);

    std::unique_ptr<Expression> ParseTernary();
    std::unique_ptr<Expression> ParseAssignExpression();
    std::unique_ptr<Expression> ParseCommaExpression();
    std::unique_ptr<Expression> ParseIfStatement();
    std::unique_ptr<Expression> ParseElseBranch();
    std::unique_ptr<Expression> ParseStatement();
    std::unique_ptr<Expression> ParseForStatement();
    std::vector<std::string> ParseArgumentList();
    std::unique_ptr<FunctionPrototype> ParsePrototype();
    std::unique_ptr<Expression> ParseFunction();
    std::unique_ptr<Expression> ParseBlockStatement();

    bool ParseExpression();

    std::shared_ptr<Expression> ParsedAST()
    {
        return expr_ast_;
    }
private:
    std::shared_ptr<Expression> expr_ast_;
    std::unique_ptr<Lexer> lex_;
};


} // parser
} // grok

#endif
