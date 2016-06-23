#ifndef PARSER_H_
#define PARSER_H_

#include "parser/common.h"
#include "parser/expression.h"
#include "lexer/lexer.h"

namespace grok { namespace parser {

class GrokParser {
public:
    GrokParser() = default;

    GrokParser(std::unique_ptr<Lexer> lex) :
        lex_(std::move(lex)) 
    { }

    std::unique_ptr<Expression> ParsePrimary();
    std::unique_ptr<Expression> ParseMember();
    std::unique_ptr<Expression> ParseBinary();
    
    std::unique_ptr<Expression>
    ParseBinaryRhs(int prec, std::unique_ptr<Expression> lhs);
private:
    std::unique_ptr<Expression> expr_ast_;
    std::unique_ptr<Lexer> lex_;
};

} // parser
} // grok

#endif
