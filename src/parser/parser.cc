#include "parser/parser.h"
#include "parser/ifstatement.h"
#include "parser/forstatement.h"
#include "parser/blockstatement.h"
#include "parser/functionstatement.h"
#include "lexer/token.h"

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

bool IsAssign(TokenType type_) {
    return (type_ == ASSIGN) || (type_ >= PLUSEQ && type_ <= MULEQ) ||
           (type_ >= BOREQ && type_ <= XOREQ) ||
           (type_ >= NOTEQ && type_ <= SHREQ);
}

} // grok


std::unique_ptr<Expression> GrokParser::ParsePrimary()
{
    TokenType tok = lex_->peek();
    std::unique_ptr<Expression> result;

    if (tok == DIGIT) {
        result = std::make_unique<IntegralLiteral>(lex_->GetNumber());
    } else if (tok == STRING) {
        result = std::make_unique<StringLiteral>(lex_->GetStringLiteral());
    } else if (tok == IDENT) {
        result = std::make_unique<Identifier>(lex_->GetIdentifierName());
    } else if (tok == TRUE) {
        result = std::make_unique<BooleanLiteral>(true);
    } else if (tok == FALSE) {
        result = std::make_unique<BooleanLiteral>(false);
    } else {
        throw std::runtime_error("expected a primary expression");
    }

    lex_->advance();
    return std::move(result);
}

// reference for this function ::= llvm/examples/Kaleidoscope/chapter3/toy.cpp
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
        lhs = std::make_unique<BinaryExpression>(tok,
                std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<Expression> GrokParser::ParseBinary()
{
    auto lhs = ParsePrimary();

    // parse the rhs
    return ParseBinaryRhs(3, std::move(lhs));
}

std::unique_ptr<Expression> GrokParser::ParseAssignExpression()
{
    auto lhs = ParseTernary();
    auto tok = lex_->peek();

    if (!grok::IsAssign(tok))
        return lhs;

    lex_->advance();
    auto rhs = ParseAssignExpression();
    return std::make_unique<AssignExpression>(std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expression> GrokParser::ParseTernary()
{
    auto first = ParseBinary();

    auto tok = lex_->peek();
    if (tok != CONDITION) {
        return first;
    }

    // now we're parsing conditional expression
    // eat '?'
    lex_->advance();
    auto second = ParseAssignExpression();
    
    tok = lex_->peek();
    if (tok != COLON) {
        throw std::runtime_error("expected a ':'");
    }

    // eat ':'
    lex_->advance();
    auto third = ParseAssignExpression();

    return std::make_unique<TernaryExpression>(std::move(first),
                    std::move(second), std::move(third));
}

std::unique_ptr<Expression> GrokParser::ParseCommaExpression()
{
    auto one = ParseAssignExpression();
    auto tok = lex_->peek();

    // if we have a comma ',', then we definitely have to parse
    // expr of type (expr, expr*)
    if (tok != COMMA)
        return std::move(one);

    std::vector<std::unique_ptr<Expression>> exprs;

    // loop until we don't find a ','
    while (true) {
        exprs.push_back(std::move(one));

        lex_->advance();
        one = ParseAssignExpression();
        
        tok = lex_->peek();
        if (tok != COMMA)
            break;   
    }

    return std::make_unique<CommaExpression>(std::move(exprs));
}

std::unique_ptr<Expression> GrokParser::ParseElseBranch()
{
    // eat 'else'
    lex_->advance();

    return ParseStatement();
}

std::unique_ptr<Expression> GrokParser::ParseIfStatement()
{
    std::unique_ptr<Expression> result;

    // eat 'if'
    lex_->advance();

    auto tok = lex_->peek();
    if (tok != LPAR) 
        throw std::runtime_error("expected a '('");
    lex_->advance();

    // parse the condition of if statement
    auto condition = ParseCommaExpression();

    tok = lex_->peek();
    if (tok != RPAR)
        throw std::runtime_error("expected a ')'");
    lex_->advance();

    // parse the body of 'if'
    auto body = ParseStatement();

    tok = lex_->peek();
    if (tok == ELSE) {
        result = std::make_unique<IfElseStatement>(std::move(condition),
            std::move(body), ParseElseBranch());
    } else {
        result = std::make_unique<IfStatement>(std::move(condition),
            std::move(body));
    }

    return std::move(result);
}

std::unique_ptr<Expression> GrokParser::ParseForStatement()
{
    // eat 'for'
    lex_->advance();
    auto tok = lex_->peek();

    if (tok != LPAR)
        throw std::runtime_error("expected a '('");
    lex_->advance();

    // parse 'for ( >>this<< ;...' part
    auto init = ParseCommaExpression();

    tok = lex_->peek();
    if (tok != SCOLON)
        throw std::runtime_error("expected a ';'");
    lex_->advance();

    // parse 'for (x = 10; >>this<< ...' part
    auto condition = ParseCommaExpression();

    tok = lex_->peek();
    if (tok != SCOLON)
        throw std::runtime_error("expected a ';'");
    lex_->advance();

    // parse 'for (x = 10; x < 100; >>this<<...' part
    auto update = ParseCommaExpression();

    tok = lex_->peek();
    if (tok != RPAR)
        throw std::runtime_error("expected a ')'");
    lex_->advance();

    // parse 'for (x = 10; x < 100; x = x + 1) >>rest<<...' part
    auto body = ParseStatement();
    return std::make_unique<ForStatement>(std::move(init),
        std::move(condition), std::move(update), std::move(body));
}

std::vector<std::string> GrokParser::ParseArgumentList()
{
    auto tok = lex_->peek();
    auto result = std::vector<std::string>();

    if (tok != LPAR)
        throw std::runtime_error("expected a '('");
    lex_->advance();

    // check for ')'
    tok = lex_->peek();
    if (tok == RPAR)
        return { }; // return an empty vector

    while (true) {
        tok = lex_->peek();

        if (tok != IDENT) 
            throw std::runtime_error("expected an identifier");

        result.push_back(lex_->GetIdentifierName());
        lex_->advance();

        tok = lex_->peek();
        if (tok == RPAR)
            break;

        if (tok != COMMA)
            throw std::runtime_error("expected a ',' or ')'");
        lex_->advance();
    }

    // eat the ')'
    lex_->advance();
    return std::move(result);
}

std::unique_ptr<FunctionPrototype> GrokParser::ParsePrototype()
{
    // eat 'function'   
    lex_->advance();
    auto tok = lex_->peek();

    if (tok != IDENT)
        throw std::runtime_error("expected an identifier");
    auto name = lex_->GetIdentifierName();
    // eat the IDENT
    lex_->advance();

    // parse the argument list
    auto args = ParseArgumentList();
    return std::make_unique<FunctionPrototype>(name, std::move(args));
}

std::unique_ptr<Expression> GrokParser::ParseFunction()
{
    auto proto = ParsePrototype();
    auto body = ParseStatement();

    return std::make_unique<FunctionStatement>(std::move(proto),
        std::move(body));
}

std::unique_ptr<Expression> GrokParser::ParseBlockStatement()
{
    std::vector<std::unique_ptr<Expression>> stmts;
    lex_->advance(); // eat '{'

    while (true) {
        auto stmt = ParseStatement();
        stmts.push_back(std::move(stmt));

        auto tok = lex_->peek();
        if (tok == RBRACE)
            break;
    }

    return std::make_unique<BlockStatement>(std::move(stmts));
}

std::unique_ptr<Expression> GrokParser::ParseStatement()
{
    auto tok = lex_->peek();

    switch (tok) {
    default:
    {
        auto result = ParseCommaExpression();
        tok = lex_->peek();

        if (tok != SCOLON)
            throw std::runtime_error("expected a ';'");
        lex_->advance();
        return result;
    }

    case IF:
        return ParseIfStatement();  
    case FOR:
        return ParseForStatement();
    case FUNC:
        return ParseFunction();
    case LBRACE:
        return ParseBlockStatement();
    }
}

bool GrokParser::ParseExpression()
{
    try {
        expr_ast_ = ParseStatement();
    } catch (std::exception &e) {
        std::cerr << "<" << lex_->GetCurrentPosition().row_
            << ":" << lex_->GetCurrentPosition().col_ << "> ";
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}

namespace grok { namespace parser {
std::ostream &operator<<(std::ostream &os, GrokParser &parser)
{
    *parser.ParsedAST() << os;
    return os;
}

}} // end namespaces
