#include "parser/expression.h"
#include "parser/ifstatement.h"
#include "parser/forstatement.h"
#include "parser/blockstatement.h"
#include "vm/instruction.h"
#include "vm/instruction-builder.h"
#include "lexer/token.h"

namespace grok {
namespace parser {

using namespace grok::vm;

void IntegralLiteral::emit(std::shared_ptr<InstructionBuilder> builder)
{
    auto instr = InstructionBuilder::Create<Instructions::push>();
    instr->data_type_ = d_num;
    instr->number_ = value_;

    builder->AddInstruction(std::move(instr));
}

void StringLiteral::emit(std::shared_ptr<InstructionBuilder> builder)
{
    auto instr = InstructionBuilder::Create<Instructions::push>();
    instr->data_type_ = d_str;
    instr->str_ = str_;

    builder->AddInstruction(std::move(instr));
}

void BooleanLiteral::emit(std::shared_ptr<InstructionBuilder> builder)
{
    auto instr = InstructionBuilder::Create<Instructions::push>();
    instr->data_type_ = d_bool;
    instr->boolean_ = pred_;

    builder->AddInstruction(std::move(instr));
}

void Identifier::emit(std::shared_ptr<InstructionBuilder> builder)
{
    auto instr = InstructionBuilder::Create<Instructions::fetch>();
    instr->data_type_ = d_name;
    instr->str_ = name_;

    builder->AddInstruction(std::move(instr));
    
    instr = InstructionBuilder::Create<Instructions::pushim>();
    instr->data_type_ = d_null;
    builder->AddInstruction(std::move(instr));
}

void EmitBinaryOperator(BinaryExpression::Operator op,
        std::shared_ptr<InstructionBuilder> builder)
{
    auto instr = InstructionBuilder::Create<Instructions::noop>();
    switch (op) {
    default:
        throw std::runtime_error("unknown operator");
    case PLUS:
        instr->kind_ = Instructions::adds;
        break;
    case MINUS:
        instr->kind_ = Instructions::subs;
        break;
    case MUL:
        instr->kind_ = Instructions::muls;
        break;
    case DIV:
        instr->kind_ = Instructions::divs;
        break;
    case SHR:
        instr->kind_ = Instructions::shrs;
        break;
    case SHL:
        instr->kind_ = Instructions::shls;
        break;
    case MOD:
        instr->kind_ = Instructions::rems;
        break;
    case LT:
        instr->kind_ = Instructions::lts;
        break;
    case GT:
        instr->kind_ = Instructions::gts;
        break;
    case EQUAL:
        instr->kind_ = Instructions::eqs;
        break;
    case NOTEQ:
        instr->kind_ = Instructions::neqs;
        break;
    }
    instr->data_type_ = d_null;
    builder->AddInstruction(std::move(instr));
}

void BinaryExpression::emit(std::shared_ptr<InstructionBuilder> builder)
{
    lhs_->emit(builder);
    rhs_->emit(builder);

    EmitBinaryOperator(op_, builder);
}

void AssignExpression::emit(std::shared_ptr<InstructionBuilder> builder)
{
    // generate code for rhs
    rhs_->emit(builder);

    // I don't know why but I don't like the code below
    auto lhs = lhs_.get();
    auto ident = dynamic_cast<Identifier*>(lhs);
    if (!ident)
        throw std::runtime_error("fatal: can't assign to an rvalue");

    auto instr = InstructionBuilder::Create<Instructions::store>();
    instr->data_type_ = d_name;
    instr->str_ = ident->GetName();
    builder->AddInstruction(std::move(instr));
}

void TernaryExpression::emit(std::shared_ptr<InstructionBuilder> builder)
{
    // now build the code for conditional expression
    // result will be stored in the flags
    first_->emit(builder);
    
    // add a jmpz instruction at the end of this block
    auto instr = InstructionBuilder::Create<Instructions::jmpz>();
    instr->data_type_ = d_null;
    instr->number_ = 0.0;
    instr->jmp_addr_ = 0;
    builder->AddInstruction(std::move(instr));

    // now create a block that will handle the instructions for second_
    builder->CreateBlock();
    second_->emit(builder);

    // add jmp instruction at the end of current block
    instr = InstructionBuilder::Create<Instructions::jmp>();
    
    // update the jmpz instruction which is currently in stack
    builder->UpdateStackedJump();

    // create another block for third_
    builder->CreateBlock();
    third_->emit(builder);
    builder->EndBlockForJump();

    // end the block
    builder->EndBlock();
}

void CommaExpression::emit(std::shared_ptr<InstructionBuilder> builder)
{
    for (auto &expr : exprs_) {
        expr->emit(builder);
    }
}

void IfStatement::emit(std::shared_ptr<InstructionBuilder> builder)
{
    // code generation for if statement is almost same as that of ternary
    condition_->emit(builder);

    auto instr = InstructionBuilder::Create<Instructions::jmpz>();
    instr->data_type_ = d_null;
    instr->jmp_addr_ = 0;
    builder->AddInstruction(std::move(instr));

    // create a block that will hold if body
    builder->CreateBlock();
    body_->emit(builder);
    builder->EndBlockForJump();
}

void IfElseStatement::emit(std::shared_ptr<InstructionBuilder> builder)
{
    // same as that of ternary expression
    // result of if condition will be stored in the flags
    condition_->emit(builder);
    
    // add a jmpz instruction at the end of current block
    auto instr = InstructionBuilder::Create<Instructions::jmpz>();
    instr->data_type_ = d_null;
    instr->number_ = 0.0;
    instr->jmp_addr_ = 0;
    builder->AddInstruction(std::move(instr));

    // now create a block that will hold instruction for `if` body
    builder->CreateBlock();
    body_->emit(builder);

    // add jmp instruction at the end of current block used for skipping `else`
    instr = InstructionBuilder::Create<Instructions::jmp>();
    
    // update the jmpz instruction which is currently in stack
    builder->UpdateStackedJump();

    // create another block for `else` body
    builder->CreateBlock();
    else_->emit(builder);
    builder->EndBlockForJump();

    // end the block
    builder->EndBlock();
}

void ForStatement::emit(std::shared_ptr<InstructionBuilder> builder)
{
    // init code for `for`
    init_->emit(builder);

    // start of the condition_ instructions
    auto cmp_blk_start = builder->CurrentLength();
    condition_->emit(builder);

    // insert a jmpz instruction at the end of the condition block
    auto instr = InstructionBuilder::Create<Instructions::jmpz>();
    instr->data_type_ = d_null;
    instr->jmp_addr_ = 0; // to be calculate
    auto instr_ptr = instr.get();

    builder->AddInstruction(std::move(instr));

    // end of the condition instructions
    auto cmp_blk_end = builder->CurrentLength();

    body_->emit(builder);
    update_->emit(builder);
    
    // insert a jmp back instruction
    instr = InstructionBuilder::Create<Instructions::jmp>();
    instr->data_type_ = d_null;
    instr->jmp_addr_ = 0;   // to be calculated later
    auto jmp_back_ptr = instr.get();
    builder->AddInstruction(std::move(instr));

    // mark the end of the for loop
    auto for_loop_end = builder->CurrentLength();

    // update the jump instructions
    instr_ptr->jmp_addr_ = for_loop_end - cmp_blk_end;
    jmp_back_ptr->jmp_addr_ = -(for_loop_end - cmp_blk_start);
}

void BlockStatement::emit(std::shared_ptr<InstructionBuilder> builder)
{
    for (auto &stmt : stmts_) {
        stmt->emit(builder);
    }
}

void FunctionCallExpression::emit(std::shared_ptr<InstructionBuilder> builder)
{
    // TODO
}

}
}
