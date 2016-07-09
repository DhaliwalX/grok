#include "parser/parser.h"
#include "parser/ifstatement.h"
#include "parser/forstatement.h"
#include "parser/blockstatement.h"
#include "parser/functionstatement.h"
#include "parser/returnstatement.h"
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
           (type_ > NOTEQ && type_ <= SHREQ);
}

} // grok

std::unique_ptr<Expression> GrokParser::ParseArrayLiteral()
{
    std::vector<std::unique_ptr<Expression>> exprs;
    // eat '['
    lex_->advance();
    auto tok = lex_->peek();

    if (tok == RSQB) {
        // done
        return std::make_unique<ArrayLiteral>(std::move(exprs));
    }
    while (true) {
        auto one = ParseAssignExpression();
        exprs.push_back(std::move(one));

        tok = lex_->peek();
        
        if (tok == RSQB)
            break;
        if (tok != COMMA)
            throw std::runtime_error("expected a ',' or ']'");
        lex_->advance();   
    }

    return std::make_unique<ArrayLiteral>(std::move(exprs));
}

std::unique_ptr<Expression> GrokParser::ParseObjectLiteral()
{
    std::map<std::string, std::unique_ptr<Expression>> Props;

    // eat the left brace '{'
    lex_->advance();
    auto tok = lex_->peek();

    // if next tok is '}' then nothing to be done
    if (tok == RBRACE) {
        return std::make_unique<ObjectLiteral>(std::move(Props));
    }

    std::string name;
    std::unique_ptr<Expression> prop;
    while (true) {
        tok = lex_->peek();
        if (tok == STRING) {
            name = lex_->GetStringLiteral();
        } else if (tok == IDENT) {
            name = lex_->GetIdentifierName();
        } else {
            throw std::runtime_error("expected an Identifier or a string");
        }

        lex_->advance();
        tok = lex_->peek();

        if (tok != COLON) {
            throw std::runtime_error("expected a ':'");
        }
        lex_->advance();

        prop = ParseAssignExpression();
        Props[name] = std::move(prop);

        // next token should be a ',' or '}'
        tok = lex_->peek();
        if (tok == RBRACE)
            break;
        if (tok != COMMA)
            throw std::runtime_error("expected a ',' or '}'");
        lex_->advance();
    }

    return std::make_unique<ObjectLiteral>(std::move(Props));
}

std::unique_ptr<Expression> GrokParser::ParsePrimary()
{
    TokenType tok = lex_->peek();
    std::unique_ptr<Expression> result;

    if (tok == JSNULL) {
        result = std::make_unique<NullLiteral>();
    } else if (tok == DIGIT) {
        result = std::make_unique<IntegralLiteral>(lex_->GetNumber());
    } else if (tok == STRING) {
        result = std::make_unique<StringLiteral>(lex_->GetStringLiteral());
    } else if (tok == IDENT) {
        result = std::make_unique<Identifier>(lex_->GetIdentifierName());
    } else if (tok == TRUE) {
        result = std::make_unique<BooleanLiteral>(true);
    } else if (tok == FALSE) {
        result = std::make_unique<BooleanLiteral>(false);
    } else if (tok == THIS) {
        result = std::make_unique<ThisHolder>();
    } else if (tok == LPAR) {
        lex_->advance();    // eat '('
        result = ParseCommaExpression();
        tok = lex_->peek();

        if (tok != RPAR)
            throw std::runtime_error("expected a ')'");
    } else if (tok == LSQB) {
        result = ParseArrayLiteral();
    } else if (tok == LBRACE) {
        result = ParseObjectLiteral();
    } else if (tok == FUNC) {
        result = ParseFunction();
        return result;
    } else {
        throw std::runtime_error("expected a primary expression");
    }

    lex_->advance();
    return result;
}

std::unique_ptr<Expression> GrokParser::ParseDotExpression()
{
    // eat the '.'
    lex_->advance();

    auto tok = lex_->peek();

    // this token should be a valid identifier
    if (tok != IDENT)
        throw std::runtime_error("expected a valid identifier");
    auto name = lex_->GetIdentifierName();

    auto ident = std::make_unique<Identifier>(name);
    lex_->advance();
    return std::make_unique<DotMemberExpression>(std::move(ident));
}

std::unique_ptr<Expression> GrokParser::ParseIndexExpression()
{
    // eat the '['
    lex_->advance();
    auto expr = ParseAssignExpression();
    if (lex_->peek() != RSQB)
        throw std::runtime_error("expected a ']'");

    lex_->advance(); // consumex ']'
    return std::make_unique<IndexMemberExpression>(std::move(expr));
}

std::unique_ptr<Expression> GrokParser::ParseMemberExpression()
{
    auto primary = ParsePrimary();
    auto tok = lex_->peek();

    // if next token is neither '[' or '.'
    if (tok != LSQB && tok != DOT)
        return primary;

    std::vector<std::unique_ptr<Expression>> Members;
    std::unique_ptr<Expression> Member;

    Members.push_back(std::move(primary));
    while (true) {
        tok = lex_->peek();
        if (tok == DOT) {
            Member = ParseDotExpression();
        } else if (tok == LSQB) {
            Member = ParseIndexExpression();
        } else if (tok == LPAR) {
            auto args = ParseArgumentList();
            Member = std::make_unique<ArgumentList>(std::move(args));
        } else {
            break;
        }
        Members.push_back(std::move(Member));
    }

    return std::make_unique<MemberExpression>(std::move(Members));
}

std::vector<std::unique_ptr<Expression>> GrokParser::ParseArgumentList()
{
    std::vector<std::unique_ptr<Expression>> exprs;

    auto tok = lex_->peek();
    if (tok != LPAR)
        throw std::runtime_error("expected a '('");
    lex_->advance();

    tok = lex_->peek();
    if (tok == RPAR) {
        lex_->advance();
        return { };
    }

    while (true) {
        auto one = ParseAssignExpression();
        exprs.push_back(std::move(one));

        tok = lex_->peek();
        if (tok == RPAR)
            break;
        if (tok != COMMA)
            throw std::runtime_error("expected a ',' or ')'");
        lex_->advance();
    }

    lex_->advance(); // eat the last ')'
    return exprs;    
}

/// ParseFunctionCall ::= Parses a Function call expression like
///                 call(a, b);
std::unique_ptr<Expression> GrokParser::ParseFunctionCall()
{
    auto maybemember = ParseMemberExpression();
    auto tok = lex_->peek();

    if (tok != LPAR)
        return maybemember;
    auto args = ParseArgumentList();
    return std::make_unique<FunctionCallExpression>(std::move(maybemember),
        std::move(args));
}

std::unique_ptr<Expression> GrokParser::ParseCallExpression()
{
    auto func = ParseFunctionCall();
    auto tok = lex_->peek();

    if (tok != DOT && tok != LSQB && tok != LPAR)
        return func;
    std::vector<std::unique_ptr<Expression>> Members;
    std::unique_ptr<Expression> Member;

    while (true) {
        tok = lex_->peek();
        if (tok == DOT) {
            Member = ParseDotExpression();
        } else if (tok == LSQB) {
            Member = ParseIndexExpression();
        } else if (tok == LPAR) {
            auto args = ParseArgumentList();
            Member = std::make_unique<ArgumentList>(std::move(args));
        } else {
            break;
        }
        Members.push_back(std::move(Member));
    }

    return std::make_unique<CallExpression>(std::move(func),
        std::move(Members));
}

std::unique_ptr<Expression> GrokParser::ParseNewExpression()
{
    auto tok = lex_->peek();

    if (tok != NEW) {
        return ParseCallExpression();
    }
    lex_->advance(); // eat new
    auto member = ParseNewExpression();
    return std::make_unique<NewExpression>(std::move(member));
}

std::unique_ptr<Expression> GrokParser::CreateBinaryExpression(TokenType op,
        std::unique_ptr<Expression> LHS, std::unique_ptr<Expression> RHS)
{
    return std::make_unique<BinaryExpression>(op, std::move(LHS),
        std::move(RHS));
}

std::unique_ptr<Expression> GrokParser::ParseUnaryExpression()
{
    auto tok = lex_->peek();

    if (tok == PLUS) {
        lex_->advance();
        // convert + (Expr) to Expr * 1
        return CreateBinaryExpression(MUL, 
            ParseUnaryExpression(), std::make_unique<IntegralLiteral>(1));
    } else if (tok == MINUS) {
        lex_->advance();

        // similarly for `-Expr` to `Expr * -1` 
        return CreateBinaryExpression(MUL, 
            ParseUnaryExpression(), std::make_unique<IntegralLiteral>(-1));
    } else if (tok == INC || tok == DEC || tok == NOT || tok == BNOT) {
        lex_->advance();

        return std::make_unique<PrefixExpression>(tok, ParseNewExpression());
    }
    auto L = ParseNewExpression();

    tok = lex_->peek();
    if (tok == INC) {
        lex_->advance();
        return std::make_unique<PostfixExpression>(tok, std::move(L));
    } else if (tok == DEC) {
        lex_->advance();
        return std::make_unique<PostfixExpression>(tok, std::move(L));
    } else {
        return L;
    }
}

/// reference for this function ::= llvm/examples/Kaleidoscope/chapter3/toy.cpp
/// if you are unable to understand the function just imagine you are 
/// parsing 2 + 3 * 5 - 6 / 7, (I too used that as a reference)
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

        auto rhs = ParseUnaryExpression();

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
    auto lhs = ParseUnaryExpression();

    // parse the rhs, if any
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
        return one;

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

    return result;
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

std::unique_ptr<Expression> GrokParser::ParseWhileStatement()
{
    lex_->advance(); // eat 'while'

    auto tok = lex_->peek();

    if (tok != LPAR) {
        throw std::runtime_error("expected a '('");
    }
    lex_->advance();
    
    auto condition = ParseCommaExpression();
    tok = lex_->peek();
    if (tok != RPAR)
        throw std::runtime_error("expected a ')'");

    lex_->advance();
    auto body = ParseStatement();

    return std::make_unique<WhileStatement>(std::move(condition),
            std::move(body));
}

std::unique_ptr<Expression> GrokParser::ParseDoWhileStatement()
{
    lex_->advance(); // eat 'do'
    auto body = ParseStatement();

    auto tok = lex_->peek();
    if (tok != WHILE)
        throw std::runtime_error("expected 'while'");
    lex_->advance();

    tok = lex_->peek();
    if (tok != LPAR) {
        throw std::runtime_error("expected a '('");
    }
    lex_->advance();

    auto condition = ParseCommaExpression();
    tok = lex_->peek();
    if (tok != RPAR) {
        throw std::runtime_error("expected a ')'");
    }
    lex_->advance();

    tok = lex_->peek();
    if (tok != SCOLON)
        throw std::runtime_error("expected a ';'");
    lex_->advance();

    return std::make_unique<DoWhileStatement>(std::move(condition),
        std::move(body));
}

std::vector<std::string> GrokParser::ParseParameterList()
{
    auto tok = lex_->peek();
    auto result = std::vector<std::string>();

    if (tok != LPAR)
        throw std::runtime_error("expected a '('");
    lex_->advance();

    // check for ')'
    tok = lex_->peek();
    if (tok == RPAR) {
        lex_->advance();
        return { }; // return an empty vector
    }

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
    return result;
}

std::unique_ptr<FunctionPrototype> GrokParser::ParsePrototype()
{
    // eat 'function'   
    lex_->advance();
    auto tok = lex_->peek();

    std::string name;
    if (tok == IDENT) {
        name = lex_->GetIdentifierName();
        // eat the IDENT
        lex_->advance();
    }

    // parse the argument list
    auto args = ParseParameterList();
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

    lex_->advance(); // eat '}'
    return std::make_unique<BlockStatement>(std::move(stmts));
}

std::unique_ptr<Expression> GrokParser::ParseReturnStatement()
{
    lex_->advance(); // eat 'return'
    auto expr = ParseAssignExpression();
    auto tok = lex_->peek();

    if (tok != SCOLON)
        throw std::runtime_error("expected a ';'");
    lex_->advance();
    return std::make_unique<ReturnStatement>(std::move(expr));
}

std::unique_ptr<Declaration> GrokParser::ParseDeclaration()
{
    auto tok = lex_->peek();
    if (tok != IDENT) {
        throw std::runtime_error("expected an identifier");
    }
    std::string name = lex_->GetIdentifierName();
    lex_->advance();

    tok = lex_->peek();
    if (tok == SCOLON || tok == COMMA) {
        return std::make_unique<Declaration>(name);
    } else if (tok != ASSIGN) {
        throw std::runtime_error("expected a '='");
    }
    lex_->advance();
    return std::make_unique<Declaration>(name, ParseAssignExpression());
}

std::unique_ptr<Expression> GrokParser::ParseVariableStatement()
{
    lex_->advance();    // eat 'var'

    std::vector<std::unique_ptr<Declaration>> decl_list;
    while (true) {
        decl_list.push_back(ParseDeclaration());

        auto tok = lex_->peek();
        if (tok == SCOLON)
            break;
        else if (tok != COMMA)
            throw std::runtime_error("expected a ',' or ';'");
        lex_->advance(); // eat ','
    }

    lex_->advance(); // eat ';'
    return std::make_unique<DeclarationList>(std::move(decl_list));
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
    case RET:
        return ParseReturnStatement();
    case WHILE:
        return ParseWhileStatement();
    case DO:
        return ParseDoWhileStatement();
    case VAR:
        return ParseVariableStatement();
    }
}

bool GrokParser::ParseExpression()
{
    std::vector<std::unique_ptr<Expression>> exprs;
    expr_ast_ = std::make_shared<BlockStatement>(std::move(exprs));
    try {
        while (!(lex_->peek() == EOS)) {
            expr_ast_->PushExpression(ParseStatement());
        }
    } catch (std::exception &e) {
        std::cerr << "<" << lex_->GetCurrentPosition().row_
            << ":" << lex_->GetCurrentPosition().col_ << "> ";
        std::cerr << e.what() << ", got '"
            << TOKENS[lex_->peek()].value_ << "'." << std::endl;
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
