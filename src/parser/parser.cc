#include "parser.h"
#include "../lexer/token.h"

using namespace grok::parser;

namespace myunique {
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<T>(params)...));
}

} // namespace myunique

namespace grok {
bool IsBinaryOperator(TokenType type) {
  return (type >= PLUS && type <= LT) ||
           ((type >= XOR) && (type <= DEC)) ||
           ((type >= GTE) && (type <= AND)) ||
           ((type == SHL) && (type == SHR));
}

} // grok


std::unique_ptr<Expression> GrokParser::ParsePrimary()
{
    TokenType tok = lex_->peek();
    std::unique_ptr<Expression> result;

    if (tok == DIGIT) {
        result = myunique::make_unique<IntegralLiteral>(lex_->GetNumber());
    } else if (tok == STRING) {
        result = myunique::make_unique<StringLiteral>(lex_->GetStringLiteral());
    } else if (tok == IDENT) {
        result = myunique::make_unique<Identifier>(lex_->GetIdentifierName());
    } else if (tok == TRUE) {
        result = myunique::make_unique<BooleanLiteral>(true);
    } else if (tok == FALSE) {
        result = myunique::make_unique<BooleanLiteral>(false);
    } else {
        throw std::runtime_error("expected a primary expression");
    }

    lex_->advance();
    return std::move(result);
}

// if you are unable to understand the function just imagine you are 
// parsing 2 + 3 * 5 - 6 / 7, (I too used that as a reference)
std::unique_ptr<Expression> GrokParser::ParseBinaryRhs(int prec,
        std::unique_ptr<Expression> lhs)
{
    while (true) {
        int tokprec = lex_->GetPrecedance();

        if (tokprec < prec) {
            return lhs;
        }

        // now we definitely have a binary operator
        auto tok = lex_->peek();
        lex_->advance();

        auto rhs = ParsePrimary();

        auto nextprec = lex_->GetPrecedance();
        if (tokprec < nextprec) {
            rhs = ParseBinaryRhs(tokprec + 1, std::move(rhs));
        }

        // merge lhs and rhs
        lhs = myunique::make_unique<BinaryExpression>(tok,
                std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<Expression> GrokParser::ParseBinary()
{
    auto lhs = ParsePrimary();

    return ParseBinaryRhs(0, std::move(lhs));
}

