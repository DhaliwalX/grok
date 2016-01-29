///////////////////////////////////////////////////////////////////////////////
//
//    +-----------------------------------------------------------+
//    |          Grok : A Naive JavaScript Interpreter            |
//    +-----------------------------------------------------------+
//
// Copyright 2015 Pushpinder Singh
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
//  \file:  intepreter.cc
//  \description:  implementation of interpreter class
//  \author: Pushpinder Singh
//
///////////////////////////////////////////////////////////////////////////////

#include "interpreter.h"

#define Undefined()  { }
#define MAKE_SHARED() std::make_shared<AstNode>()


js::Stack Interpreter::stack_ = {};
js::Stack Interpreter::object_member_stack_ = {};

std::shared_ptr<AstNode> Interpreter::ExecuteHighLevelStatement(
  std::shared_ptr<AstNode> node) {
  auto expr_type = node->expression_type_;
  switch (expr_type) {

  case JS::_compound:
  {
    // In compound statements, the links_ field contains all the statements
    // included in the braces. And, also compound statements have scope
    // break and return statement are also checked while executing the body
    stack_.CreateScope();
    std::shared_ptr<AstNode> result;
    for (auto &each : node->links_) {
      result = ExecuteHighLevelStatement(each);
      if (result->expression_type_ == JS::_return
          || result->expression_type_ == JS::_break
          || result->expression_type_ == JS::_continue)
        break;
    }
    stack_.RemoveScope();
    return result;
  }
  break;

  // execute an if statement
  case JS::_if:
  {
    // a if node consists two or three children based on whether it is an if or
    // an if-else respectively. In both cases, first child is condition
    // expression whereas second child is the if body and third child is
    // optional else part
    auto cond = node->GetNthChild(0);

    // evaluate the condition and store the result
    auto result = ExecuteLowLevelStatement(cond);
    decltype(result) toret;
    auto if_st = node->GetNthChild(1);
    auto size = node->links_.size();

    if (size == 2) {
      // alone if case
      if (ConditionEvaluation(result)) {
        // execute the if body
        toret = ExecuteHighLevelStatement(if_st);
      }
      return toret.get() ? toret : MAKE_SHARED();
    }
    else if (size == 3) {
      // if-else
      auto els_branch = node->GetNthChild(2);
      if (ConditionEvaluation(result)) {
        toret = ExecuteHighLevelStatement(if_st);
        return toret;
      }
      else {
        toret = ExecuteHighLevelStatement(els_branch);
        return toret;
      }
    }
    // UNREACHABLE
    return MAKE_SHARED();
  }
  break;

  // execute a while loop
  case JS::_while:
  {
    // AstNode of while statement consists of two children. First child is
    // conditional expression and second one the body that is to be executed
    auto cond = node->GetNthChild(0);
    decltype(cond) break_res;

    // evaluate the condition initially
    auto result = ExecuteLowLevelStatement(cond);
    auto wh_st = node->GetNthChild(1);
    while (ConditionEvaluation(result)) {
      break_res = ExecuteHighLevelStatement(wh_st);
      if (break_res->expression_type_ == JS::_break)
        break;
      else if (break_res->expression_type_ == JS::_return) {
        return break_res;
      }
      result = ExecuteLowLevelStatement(cond);
    }
    return break_res;
  }
  break;

  case JS::_for:
  {
    // not for in loop
    // for statement always contains 4 children. Due to some parsing error,
    // for loops with empty initialization or empty condition or empty update
    // part will not work.
    auto init = node->GetNthChild(0);
    auto cond = node->GetNthChild(1);
    auto update = node->GetNthChild(2);
    auto for_st = node->GetNthChild(3);

    // initialize the loop and condition
    auto rn_init = ExecuteLowLevelStatement(init),
      rn_cond = ExecuteLowLevelStatement(cond);
    decltype(rn_init) rn_update;
    decltype(init) result;
    for (; ConditionEvaluation(rn_cond = ExecuteLowLevelStatement(cond));
    rn_update = ExecuteLowLevelStatement(update)) {
      result = ExecuteHighLevelStatement(for_st);
      if (result.get() 
          && result->expression_type_ == JS::_break)
        break;
      else if (result.get() 
               && result->expression_type_ == JS::_return)
        return result;
    }
    return result.get() ? result : MAKE_SHARED();
  }
  break;

  case JS::_break:
  {
    return node;
  }

  default:
  {
    // any other statement or expression is executed by this call
    return ExecuteLowLevelStatement(node);
  }
  }
  return std::shared_ptr<AstNode>(nullptr);
}

void Interpreter::PrintResult(std::ostream & os) {// UNIMPLEMENTED
}

void Interpreter::PrintAst(std::ostream & os) {// UNIMPLEMENTED
}

std::shared_ptr<AstNode>
Interpreter::ExecuteLowLevelStatement(std::shared_ptr<AstNode>& node) {
  auto crawler = node;
  if (node.get() != nullptr) {
    switch (crawler->expression_type_) {
    case AstNode::ExpressionType::_undefined:
      // if the expression is undefined then pop from the stack
      break;

    case AstNode::ExpressionType::_assignment:
      // If the expression is that of assignment, then get the lhs and rhs.
      // In this case, the lhs should be an l-value, otherwise it is an error.
      // L-values are the nodes with there variable_ part having some data.
      // AstNode of this type will have its first child containig lhs value
      // and second child will contain the rhs of the assignment.
    {
      auto lhs = ExecuteLowLevelStatement(node->GetNthChild(0));
      auto &rhs = node->GetNthChild(1);
      auto op = node->GetRelation();
      switch (op) {
      case TokenType::ASSIGN:
      {
        auto result = ExecuteLowLevelStatement(rhs);
        auto var = lhs;
        (var->obj_) = (result->obj_);
        return var;
      }
      break;

      // these operation are still unimplemented
      case PLUSEQ:
      case MINUSEQ:
      case MULEQ:
      case DIVEQ:
      case MODEQ:
      case BANDEQ:
      case BOREQ:
      case XOREQ:
      case SHREQ:
      case SHLEQ:
      {

      }

      }
    }

    case AstNode::ExpressionType::_conditional:
      // AstNode of _conditional type consists of three children.
      // First child contains the condition part and second child contains
      // 1st expression and third child contains the second expression.
      // first we will evaluate the condition then in the second part
      // of this function, after analysing the condition then, we will interprete
      // the either of the two expression recursively
    {
      auto cond = node->GetNthChild(0);
      auto result = ExecuteLowLevelStatement(cond);
      decltype(result) toret;
      auto first_st = node->GetNthChild(1);
      auto sec_st = node->GetNthChild(2);
      if (ConditionEvaluation(result)) {
        toret = ExecuteHighLevelStatement(first_st);
        return toret;
      }
      else {
        toret = ExecuteHighLevelStatement(sec_st);
        return toret;
      }
      return MAKE_SHARED();
    }
    break;

    case AstNode::ExpressionType::_or:
    case AstNode::ExpressionType::_and:
    case AstNode::ExpressionType::_bitor:
    case AstNode::ExpressionType::_bitxor:
    case AstNode::ExpressionType::_bitand:
    case AstNode::ExpressionType::_equality:
    case AstNode::ExpressionType::_relational:
    case AstNode::ExpressionType::_shift:
    case AstNode::ExpressionType::_term:
    case AstNode::ExpressionType::_factor:
      // AstNode of each of the above types consists of two children.
      // First child contains the lhs value,
      // And second child contains the rhs value
      return BinaryOperator(crawler);

    case AstNode::ExpressionType::_unary:
      return UnaryOperator(crawler);

    case AstNode::ExpressionType::_member:
      return MemberAccess(crawler);

    case AstNode::ExpressionType::_primary:
      if (node->GetRelation() == IDENT) {
        // this code will be changed in future with better version
        auto exists = stack_.Exists(crawler->variable_.GetName());
        if (!exists)
          stack_.PushVariable(std::pair<std::string, std::shared_ptr<AstNode>>
                              (crawler->variable_.GetName(), MAKE_SHARED()));
        return stack_.FindVariable(node->variable_.GetName());
      }
      else if (node->GetRelation() == LSQB) {
        // parse and store the array
        auto arr = std::make_shared<JSArray>();
        for (auto &each : node->links_) {
          arr->Push(ExecuteLowLevelStatement(each));
        }
        auto result = MAKE_SHARED();
        result->SetRelation(LSQB);
        result->obj_ = arr;
        result->expression_type_ = AstNode::ExpressionType::_primary;
        return result;
      }
      else if (node->GetRelation() == LBRACE) {
        // parse and store the object
        auto object = std::make_shared<JSObject>();
        for (auto &each : node->links_) {
          object->AddProperty(each->variable_.GetName(),
                              ExecuteLowLevelStatement(each->GetNthChild(0)));
        }
        auto result = MAKE_SHARED();
        result->SetRelation(LBRACE);
        result->obj_ = object;
        result->expression_type_ = AstNode::ExpressionType::_primary;
        return result;
      }
      return crawler;

    case AstNode::ExpressionType::_constructor:
    {
      // UNIMPLEMENTED
    }

    case JS::_return:
    {
      // lots and lots of bugs here. NEVER use return statement.
      // You may see unexpected results.
      if (node->GetChildrenNumber()) {
        auto ret = node->GetLastChild();
        auto result = ExecuteLowLevelStatement(ret);
        result->expression_type_ = JS::_return;
        return result;
      }
      return std::make_shared<AstNode>
        (AstNode(AstNode::ExpressionType::_return));
    }

    case AstNode::ExpressionType::_declaration:
    {
      auto rel = node->GetSecRelation();
      if (!stack_.Exists(node->variable_.GetName())) {
        stack_.PushVariable(std::pair<std::string, std::shared_ptr<AstNode>>
                            (node->variable_.GetName(), MAKE_SHARED()));
      }

      if (node->links_.size() == 1) {
        auto crawl = stack_.FindVariable(node->variable_.GetName());
        *crawl = *ExecuteLowLevelStatement(node->GetNthChild(0));
        return crawl;
      }
    }

    break;

    case AstNode::ExpressionType::_declarations:
    {
      // handle declarations like ```var x = 10, foo = "Dhaliwal";```
      std::shared_ptr<AstNode> result;
      for (auto &each : node->links_) {
        result = ExecuteLowLevelStatement(each);
      }
      return result;
    }
    break;

    case AstNode::ExpressionType::_function:
    {
      // function definition
      auto name = node->variable_.GetName();
      auto func = node;
      if (name.size())
        stack_.PushVariable(std::pair<std::string,
                            std::shared_ptr<AstNode >> (name, func));
      return node;
    }
    break;

    }
  }
  return MAKE_SHARED();
}

std::shared_ptr<AstNode>
Interpreter::BinaryOperator(std::shared_ptr<AstNode>& binary) {
  switch (binary->expression_type_) {
  case AstNode::ExpressionType::_or:
  case AstNode::ExpressionType::_and:
  case AstNode::ExpressionType::_bitor:
  case AstNode::ExpressionType::_bitxor:
  case AstNode::ExpressionType::_bitand:
  case AstNode::ExpressionType::_equality:
  case AstNode::ExpressionType::_relational:
  case AstNode::ExpressionType::_shift:
  case AstNode::ExpressionType::_term:
  case AstNode::ExpressionType::_factor:
  {
    // binary operation

    // get left term
    std::shared_ptr<AstNode> left =
      ExecuteLowLevelStatement(binary->GetNthChild(0));

    // get right term
    std::shared_ptr<AstNode> right =
      ExecuteLowLevelStatement(binary->GetNthChild(1));

    // apply the operation
    return Operate(left, right, binary->GetRelation());
  }

  case AstNode::ExpressionType::_unary:
  {
    // unary operation like i++, --a
    return UnaryOperator(binary);
  }
  } // switch

  return std::shared_ptr<AstNode>();
}

inline long long BinaryOperation(long long lh, long long rh,
                                 TokenType operation) {
  long long res;
  switch (operation) {
  case PLUS:
    res = lh + rh;
    break;
  case MINUS:
    res = lh - rh;
    break;
  case MUL:
    res = lh * rh;
    break;
  case DIV:
    res = lh / rh;
    break;
  case MOD:
    res = lh % rh;
    break;
  case SHR:
    res = lh >> rh;
    break;
  case SHL:
    res = lh << rh;
    break;
  case EQUAL:
    res = lh == rh;
    break;
  case NOTEQ:
    res = lh != rh;
    break;
  case LT:
    res = lh < rh;
    break;
  case GT:
    res = lh > rh;
    break;
  case XOR:
    res = lh ^ rh;
    break;
  case OR:
    res = lh || rh;
    break;
  case BOR:
    res = lh | rh;
    break;
  case AND:
    res = lh && rh;
    break;
  case BAND:
    res = lh & rh;
    break;
  case GTE:
    res = lh <= rh;
    break;
  case LTE:
    res = lh >= rh;
    break;
  }
  return res;
}


std::shared_ptr<AstNode>
Interpreter::UnaryOperator(std::shared_ptr<AstNode>& unary) {
  switch (unary->GetRelation()) {
  case TokenType::MINUS:
  {
    // unary minus e.g. -3432
    std::shared_ptr<AstNode> number =
      ExecuteLowLevelStatement(unary->GetNthChild(0));
    auto type = CheckType(number);
    if (type == DIGIT) {
      std::shared_ptr<JSNumber> num = std::dynamic_pointer_cast
        <JSNumber, JSBasicObject>(number->obj_);
      num->GetValue() = -(num->GetValue());
      number->obj_ = num;
      return number;
    }
  }

  case TokenType::INC:
  {
    // to differentiate between pre and post operation the parser places the
    // operand as a child in pre while it places the operand in the node itself
    // in case of post operation
    // pre-increment operator
    if (unary->GetChildrenNumber()) {
      std::shared_ptr<AstNode> number =
        ExecuteLowLevelStatement(unary->GetNthChild(0));
      auto type = CheckType(number);
      if (type == DIGIT) {
        std::shared_ptr<JSNumber> num = std::dynamic_pointer_cast
          <JSNumber, JSBasicObject>(number->obj_);
        num->GetValue() = ++(num->GetValue());
        number->obj_ = num;
        return number;
      }
      else if (type == IDENT) {
        auto actual = stack_.FindVariable(number->variable_.GetName());
        if (CheckType(number) == DIGIT) {
          std::shared_ptr<JSNumber> num = std::dynamic_pointer_cast
            <JSNumber, JSBasicObject>(number->obj_);
          num->GetValue() = ++(num->GetValue());
          number->obj_ = num;
          return number;
        }
      }
    }
    // post-increment
    else {
      auto type = CheckType(unary);
      if (type == IDENT) {
        auto num = std::dynamic_pointer_cast<JSNumber, JSBasicObject>
          (stack_.FindVariable(unary->variable_.GetName())->obj_);
        auto result = MAKE_SHARED();
        result->SetRelation(TokenType::DIGIT);
        result->obj_ = std::make_shared<JSNumber>(*num);
        auto &inc = num->GetValue();
        inc++;
        return result;
      }
    }
  }

  case TokenType::DEC:
  {
    // decrement operation is same as increment operation except the
    // operator and the kind of operation
    if (unary->GetChildrenNumber()) {
      auto number = ExecuteLowLevelStatement(unary->GetNthChild(0));
      auto type = CheckType(number);
      if (type == DIGIT) {
        std::shared_ptr<JSNumber> num = std::dynamic_pointer_cast
          <JSNumber, JSBasicObject>(number->obj_);
        num->GetValue() = --(num->GetValue());
        number->obj_ = num;
        return number;
      }
      else if (type == IDENT) {
        auto actual = stack_.FindVariable(number->variable_.GetName());
        if (CheckType(number) == DIGIT) {
          std::shared_ptr<JSNumber> num = std::dynamic_pointer_cast
            <JSNumber, JSBasicObject>(number->obj_);
          num->GetValue() = --(num->GetValue());
          number->obj_ = num;
          return number;
        }
      }
    }
    else {
      auto type = CheckType(unary);
      if (type == IDENT) {
        auto num = std::dynamic_pointer_cast<JSNumber, JSBasicObject>
          (stack_.FindVariable(unary->variable_.GetName())->obj_);
        auto result = MAKE_SHARED();
        result->SetRelation(TokenType::DIGIT);
        result->obj_ = std::make_shared<JSNumber>();
        *(result->obj_) = *num;
        auto &dec = num->GetValue();
        dec--;
        return result;
      }
    }
  }

  case TokenType::DELETE:
  case TokenType::NEW:
    Undefined();
    break;


  }
  return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode>
Interpreter::ArrayAccess(std::shared_ptr<AstNode>& node) {
  // accessing an array or an object member using array notation i.e. arr[3]
  // in case of array or obj['property'] in case of object

  // find the object on the stack
  auto arr_node = stack_.FindVariable(node->GetNthChild(0)->variable_.GetName());
  if (arr_node->obj_.get()
      && arr_node->obj_->is<JSBasicObject::ObjectType::_array>()) {

    // convert the object so found into JSArray
    auto arr =
      std::dynamic_pointer_cast<JSArray, JSBasicObject>(arr_node->obj_);

    // get the index of the element
    auto result = ExecuteLowLevelStatement(node->GetNthChild(1));
    auto size = arr->Size();
    if (CheckType(result) == DIGIT) {
      auto idx = std::dynamic_pointer_cast<JSNumber, JSBasicObject>
        (result->obj_)->GetValue();
      if (idx > size - 1 || idx < 0)
        return MAKE_SHARED();
      auto element = arr->At(idx);
      return element;
    }
  }
  else if (arr_node->obj_.get()
           && arr_node->obj_->is<JSBasicObject::ObjectType::_object>()) {
    // if it is an object
    // as of parser limitations, access like arr[3].prop is not allowed
    // or obj['prop'][3] or obj['prop'].some is not allowed

    // push the properties of the object onto object stack
    object_member_stack_.CreateScope();
    PushObjectProperties(arr_node->obj_);

    // get the property
    auto result = object_member_stack_.FindVariable(
      std::dynamic_pointer_cast<JSString, JSBasicObject>
      (node->GetNthChild(1)->obj_)->GetString());

    // remove the properties from the stack
    object_member_stack_.RemoveScope();
    if (result.get() && result->expression_type_ != JS::_undefined) {
      return result;
    }
    else {
      auto o = std::dynamic_pointer_cast<JSObject, JSBasicObject>
        (arr_node->obj_);
      o->AddProperty(node->GetNthChild(1)->variable_.GetName(),
                     MAKE_SHARED());
      result = o->GetProperty(node->GetNthChild(1)->variable_.GetName());
    }
    return result;
  }
  return std::shared_ptr<AstNode>();
}

std::shared_ptr<AstNode>
Interpreter::MemberAccess(std::shared_ptr<AstNode>& node) {
  // accessing a member or calling function
  std::shared_ptr<AstNode> mem;
  TokenType type;
  switch (node->GetSecRelation()) {
  case TokenType::DOT:
  {
    // accessing a member of an object using dot notation
    std::shared_ptr<AstNode> result, member, tmp = node;
    auto prim = node->GetNthChild(0);

    // get the object
    auto object_node = stack_.FindVariable(prim->variable_.GetName());

    std::shared_ptr<JSBasicObject> object = object_node->obj_;
    member = node->GetNthChild(1);
    while (true) {
      object_member_stack_.CreateScope();

      // load the properties onto stack
      PushObjectProperties(object);

      if (member->GetRelation() == TokenType::IDENT) {
        result = object_member_stack_.FindVariable
          (member->variable_.GetName());
        if (result.get() && result->expression_type_ != JS::_undefined) {
          break;
        }
        else {
          auto o = std::dynamic_pointer_cast<JSObject, JSBasicObject>(object);
          o->AddProperty(member->variable_.GetName(),
                         MAKE_SHARED());
          result = o->GetProperty(member->variable_.GetName());
          break;
        }
      }
      else if (member->GetRelation() == TokenType::DOT) {
        object = object_member_stack_.FindVariable
          (member->GetNthChild(0)->variable_.GetName())->obj_;
        member = member->GetNthChild(1);
        continue;
      }
      else if (member->GetRelation() == TokenType::LSQB) {
        tmp = ArrayAccess(tmp);
      }
    }
    object_member_stack_.RemoveAllScopes();
    return result;
    break;
  }

  case TokenType::LSQB:
  {
    return ArrayAccess(node);
  }

  case TokenType::LPAR:
  {
    // this is a function call
    return FunctionCall(node);
  }

  }
  return std::shared_ptr<AstNode>();
}

void Interpreter::PushObjectProperties(std::shared_ptr<JSBasicObject> obj) {
  if (obj->is<JSBasicObject::ObjectType::_object>()) {
    auto object = std::dynamic_pointer_cast<JSObject, JSBasicObject>(obj);
    for (auto prop : *object)
      object_member_stack_.PushVariable(prop);
  }
  else if (obj->is<JSBasicObject::ObjectType::_array>()) {
    
  }
}

std::shared_ptr<AstNode>
Interpreter::FunctionCall(std::shared_ptr<AstNode>& node) {
  std::shared_ptr<AstNode> parameters, result;

  // create a function scope
  stack_.CreateScope();

  auto func_id = node->GetNthChild(0);
  auto arguments = node->GetNthChild(1);

  auto func_ptr = stack_.FindVariable(func_id->variable_.GetName());
  if (func_ptr.get()
      && func_ptr->obj_->is<JSBasicObject::ObjectType::_function>()) {
    auto function = std::dynamic_pointer_cast<JSFunction, JSBasicObject>
      (func_ptr->obj_);
    auto func_parameters = function->GetParameters();

    auto no_of_arguments = arguments->links_.size();
    auto parameters_no = func_parameters.size();
    decltype(parameters_no) loop;

    // now the assignment of function parameters will done on the following
    // basis:
    //  1) If both number of arguments and number of parameters are same
    //     then all the parameters will assigned there correct values
    //  2) If number of arguments passed is more than number of parameters
    //     then all the parameters will be assigned there correct values, and
    //     rest of the arguments will be discarded
    //  3) If number of arguments is less than number of parameters then only
    //     leftmost parameters will be assigned with arguments rest will be
    //     be initialized with default (undefined) value
    loop = (parameters_no < no_of_arguments) ? parameters_no : no_of_arguments;

    for (decltype(loop) i = 0; i < loop; i++) {
      auto temp = ExecuteLowLevelStatement(arguments->links_[i]);
      // declare the parameters
      DeclareVar(func_parameters[i], temp);
    }

    auto func_handle = std::dynamic_pointer_cast<JSFunction, JSBasicObject>
      (func_ptr->obj_);

    // Javascript function have two flavors : Native and Script
    if (func_ptr->expression_type_ == JS::_native
        || func_handle->IsNative()) {
      auto handle = func_handle->GetNativeFunction();
      // execute the native function
      result = handle(MAKE_SHARED());
    }
    else {
      result = ExecuteHighLevelStatement(function->GetJSHandle());
    }
    stack_.RemoveScope();
    return result;
  }
}


std::shared_ptr<AstNode> Interpreter::Operate(std::shared_ptr<AstNode>& l,
                                              std::shared_ptr<AstNode>& r,
                                              TokenType operation) {
  // execute the binary operation
  auto type1 = CheckType(l);
  auto type2 = CheckType(r);
  std::shared_ptr<JSNumber> rhs, lhs, result = std::make_shared<JSNumber>(0);
  std::shared_ptr<AstNode> r0 = MAKE_SHARED(), rh0, lh0;
  std::shared_ptr<JSString> rhss, lhss,
    sresult = std::make_shared<JSString>("");
  long long res0;

  if (type1 == DIGIT && type2 == DIGIT) {
    lhs = std::dynamic_pointer_cast<JSNumber, JSBasicObject>((l->obj_));
    rhs = std::dynamic_pointer_cast<JSNumber, JSBasicObject>((r->obj_));
    r0 = MAKE_SHARED();
    long res = 0, lh = lhs->GetValue(), rh = rhs->GetValue();
    res = BinaryOperation(lh, rh, operation);
    *result = JSNumber(res);
    r0->obj_ = result;
    r0->SetRelation(DIGIT);
    return r0;
  }
  else if (type1 == STRING && type2 == STRING) {
    if (operation == PLUS) {
      *sresult = JSString(l->obj_->ToString() + r->obj_->ToString());
      r0->obj_ = sresult;
      r0->SetRelation(STRING);
      return r0;
    }
  }
  else if (type1 == STRING && type2 == DIGIT) {
    if (operation == PLUS) {
      *sresult = JSString(l->obj_->ToString() + r->obj_->ToString());
      r0->obj_ = sresult;
      r0->SetRelation(STRING);
      return r0;
    }
  }
  else if (type1 == DIGIT && type2 == STRING) {
    if (operation == PLUS) {
      *sresult = JSString(l->obj_->ToString() + r->obj_->ToString());
      r0->obj_ = sresult;
      r0->SetRelation(STRING);
      return r0;
    }
  }
  return std::shared_ptr<AstNode>();
}

void Interpreter::DeclareVar(std::string& var, std::shared_ptr<AstNode> &data) {
  std::pair<std::string, std::shared_ptr<AstNode>> p;
  p.first = var;
  p.second = data;
  stack_.PushVariable(p);
}

std::shared_ptr<AstNode> Interpreter::AssignVar(std::string& var,
                                                std::shared_ptr<AstNode>& data) {
  auto actual_var = stack_.FindVariable(var);
  if (actual_var.get() != nullptr) {
    *actual_var = *(data);
  }
  return actual_var;
}

TokenType Interpreter::CheckType(std::shared_ptr<AstNode>& node) {
  if (node->obj_.get()) {
    if (node->GetRelation() == DIGIT || node->GetRelation() == STRING
        || node->GetRelation() == IDENT) {
      return node->relation1_;
    }
    else if (node->obj_->GetType() == JSBasicObject::ObjectType::_number)
      return DIGIT;
    else if (node->obj_->GetType() == JSBasicObject::ObjectType::_string)
      return STRING;
    else if (node->variable_.GetName().size()) {
      return IDENT;
    }
  }
}

bool Interpreter::ConditionEvaluation(std::shared_ptr<AstNode> &node) {
  if (node->obj_.get() != nullptr
      && (node->obj_->GetType() == JSBasicObject::ObjectType::_number
          || node->obj_->GetType() == JSBasicObject::ObjectType::_string)
      && node->obj_->ToString() != "0") {
    return true;
  }
  return false;
}

