/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */

#include <string.h>
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "symtable.h"

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::PrintChildren(int indentLevel) { 
    printf("%d", value);
}
llvm::Value* IntConstant::Emit() {
    return llvm::ConstantInt::get(irgen->GetIntType(), value);
}
FloatConstant::FloatConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
void FloatConstant::PrintChildren(int indentLevel) { 
    printf("%g", value);
}
llvm::Value* FloatConstant::Emit() {
    return llvm::ConstantFP::get(irgen->GetFloatType(), value);
}
BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::PrintChildren(int indentLevel) { 
    printf("%s", value ? "true" : "false");
}
llvm::Value* BoolConstant::Emit() {
    return llvm::ConstantInt::get(irgen->GetBoolType(), value);
}
VarExpr::VarExpr(yyltype loc, Identifier *ident) : Expr(loc) {
    Assert(ident != NULL);
    this->id = ident;
}

void VarExpr::PrintChildren(int indentLevel) {
    id->Print(indentLevel+1);
}

llvm::Value* VarExpr::Emit() {
  llvm::Value *var = NULL;
  for (int i = symtable->GetCurrentIndex(); i >= 0; i--) {
    var = symtable->Lookup(i, id->GetName());
    if (var != NULL) {
      llvm::Twine* name = new llvm::Twine(id->GetName());
      llvm::Value* out = new llvm::LoadInst(var,*name,irgen->GetBasicBlock());
      return out;
    }
  }
  return NULL;
}
llvm::Value* VarExpr::Store() {
  llvm::Value *var = NULL;
  for (int i = symtable->GetCurrentIndex(); i >= 0; i--) {
    var = symtable->Lookup(i, id->GetName());
    if (var != NULL) return var;
  }
  return var;
}
Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

void Operator::PrintChildren(int indentLevel) {
    printf("%s",tokenString);
}

bool Operator::IsOp(const char *op) const {
    return strcmp(tokenString, op) == 0;
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o) 
  : Expr(Join(l->GetLocation(), o->GetLocation())) {
    Assert(l != NULL && o != NULL);
    (left=l)->SetParent(this);
    (op=o)->SetParent(this);
}

void CompoundExpr::PrintChildren(int indentLevel) {
   if (left) left->Print(indentLevel+1);
   op->Print(indentLevel+1);
   if (right) right->Print(indentLevel+1);
}
llvm::Value *RelationalExpr::Emit() {
   llvm::Value* l = left->Emit();
   llvm::Value* r = right->Emit();
   llvm::Type* type = l->getType();
   llvm::CmpInst::OtherOps ops;
   llvm::Type* comp = irgen->GetIntType();
   llvm::CmpInst::Predicate pred;

   if (type == comp) ops = llvm::CmpInst::ICmp;
   else ops = llvm::CmpInst::FCmp;

   // LESS THAN
   if ((string(op->getName())) == "<") {
     if (type == comp) pred = llvm::ICmpInst::ICMP_SLT;
     else pred = llvm::CmpInst::FCMP_OLT;
     return llvm::CmpInst::Create(ops,pred,l,r,"",irgen->GetBasicBlock()); 
   }
   // LESS THAN OR EQUAL TO
   else if ((string(op->getName())) == "<=") {
     if (type == comp) pred = llvm::ICmpInst::ICMP_SLE;
     else pred = llvm::CmpInst::FCMP_OLE;
     return llvm::CmpInst::Create(ops,pred,l,r,"",irgen->GetBasicBlock());
   }
   // GREATER THAN
   else if ((string(op->getName())) == ">") {
     if (type == comp) pred = llvm::ICmpInst::ICMP_SGT;
     else pred = llvm::CmpInst::FCMP_OGT;
     return llvm::CmpInst::Create(ops,pred,l,r,"",irgen->GetBasicBlock());
   }
   // GREATER THAN OR EQUAL TO
   else if ((string(op->getName())) == ">=") {
     if (type == comp) pred = llvm::ICmpInst::ICMP_SGE;
     else pred = llvm::CmpInst::FCMP_OGE;
     return llvm::CmpInst::Create(ops,pred,l,r,"",irgen->GetBasicBlock());
   }
   return NULL;
}
llvm::Value* AssignExpr::Emit() {
   llvm::Value *l;
   llvm::Value *r = right->Emit();
   // EQUAL ASSIGNMENT
   if ((string(op->getName())) == "=") {
     VarExpr* leftV = dynamic_cast<VarExpr*>(left);
     llvm::Value* temp = leftV->Store();
     return new llvm::StoreInst(r, temp, irgen->GetBasicBlock());
   }
   // MULTIPLICITIVE ASSIGNMENT
   if ((string(op->getName())) == "*=") {
     VarExpr* leftV = dynamic_cast<VarExpr*>(left);
     llvm::Value* temp = leftV->Store();
     l = left->Emit();
     llvm::Value *res = llvm::BinaryOperator::CreateMul(l,r,"",irgen->GetBasicBlock());
     return new llvm::StoreInst(res, temp, irgen->GetBasicBlock());
   }
   // DIVISION ASSIGNMENT
   if ((string(op->getName())) == "/=") {
     VarExpr* leftV = dynamic_cast<VarExpr*>(left);
     llvm::Value* temp = leftV->Store();
     l = left->Emit();
     llvm::Value *res = llvm::BinaryOperator::CreateUDiv(l,r,"",irgen->GetBasicBlock());
     return new llvm::StoreInst(res, temp, irgen->GetBasicBlock());
   }
   // ADDITION ASSIGNMENT
   if ((string(op->getName())) == "+=") {
     VarExpr* leftV = dynamic_cast<VarExpr*>(left);
     llvm::Value* temp = leftV->Store();
     l = left->Emit();
     llvm::Value *res = llvm::BinaryOperator::CreateAdd(l,r,"",irgen->GetBasicBlock());
     return new llvm::StoreInst(res, temp, irgen->GetBasicBlock());
   }
   // SUBTRACTION ASSIGNMENT
   if ((string(op->getName())) == "-=") {
     VarExpr* leftV = dynamic_cast<VarExpr*>(left);
     llvm::Value* temp = leftV->Store();
     l = left->Emit();
     llvm::Value *res = llvm::BinaryOperator::CreateSub(l,r,"",irgen->GetBasicBlock());
     return new llvm::StoreInst(res, temp, irgen->GetBasicBlock());
   }
   return NULL; 
}
llvm::Value* ArithmeticExpr::Emit() {
   llvm::Value *r = right->Emit();
   llvm::Type *t = r->getType();
   llvm::Value *l = NULL;
   if (left != NULL) {
     l = left->Emit();
   }
   llvm::Value *out = NULL;
   // ADDITION
   if ((string(op->getName())) == "+") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateAdd(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFAdd(l,r,"",irgen->GetBasicBlock());
   }
   // SUBTRACTION
   if ((string(op->getName())) == "-") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateSub(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFSub(l,r,"",irgen->GetBasicBlock());
   }
   // MULTIPLICATION
   if ((string(op->getName())) == "*") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateMul(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFMul(l,r,"",irgen->GetBasicBlock());
   }
   // DIVISION
   if ((string(op->getName())) == "/") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateSDiv(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFDiv(l,r,"",irgen->GetBasicBlock());
   }
   // PRE INCREMENT
   if ((string(op->getName())) == "++") {
     if (t == (llvm::Type*)irgen->GetIntType()) {
       llvm::Value *add = llvm::ConstantInt::get(irgen->GetIntType(),1);
       out = llvm::BinaryOperator::CreateAdd(r,add,"",irgen->GetBasicBlock());
     }
     else { 
       llvm::Value *add = llvm::ConstantFP::get(irgen->GetFloatType(),1.0);
       out = llvm::BinaryOperator::CreateFAdd(r,add,"",irgen->GetBasicBlock());
     }
     VarExpr* expr = dynamic_cast<VarExpr*>(right);
     new llvm::StoreInst(out, expr->Store(), irgen->GetBasicBlock());
     return out;
   }
   // POST INCREMENT
   if ((string(op->getName())) == "--") {
     if (t == (llvm::Type*)irgen->GetIntType()) {
       llvm::Value *sub = llvm::ConstantInt::get(irgen->GetIntType(),1);
       out = llvm::BinaryOperator::CreateSub(r,sub,"",irgen->GetBasicBlock());
     }
     else {
       llvm::Value *sub = llvm::ConstantFP::get(irgen->GetFloatType(),1.0);
       out = llvm::BinaryOperator::CreateSub(r,sub,"",irgen->GetBasicBlock());
     }
     VarExpr* expr = dynamic_cast<VarExpr*>(right);
     new llvm::StoreInst(out, expr->Store(), irgen->GetBasicBlock());
     return out;
   }
   return out;
}
llvm::Value* PostfixExpr::Emit() {
   llvm::Value *l = left->Emit();
   llvm::Type *type = l->getType();
   llvm::Value *out = NULL;
   // POST INCREMENT
   if ((string(op->getName())) == "++") { 
     if (type == (llvm::Type*)irgen->GetIntType()) {
       llvm::Value *add = llvm::ConstantInt::get(irgen->GetIntType(),1);
       out = llvm::BinaryOperator::CreateAdd(l,add,"",irgen->GetBasicBlock());
     }
     else {
       llvm::Value *add = llvm::ConstantFP::get(irgen->GetFloatType(),1.0);
       out = llvm::BinaryOperator::CreateFAdd(l,add,"",irgen->GetBasicBlock());
     }
     VarExpr* expr = dynamic_cast<VarExpr*>(left);
     new llvm::StoreInst(out, expr->Store(), irgen->GetBasicBlock());
     return out;
   }
   // PRE DECREMENT
   if ((string(op->getName())) == "--") {
     if (type == (llvm::Type*)irgen->GetIntType()) {
       llvm::Value *sub = llvm::ConstantInt::get(irgen->GetIntType(),1);
       out = llvm::BinaryOperator::CreateSub(l,sub,"",irgen->GetBasicBlock());
     }
     else {
       llvm::Value *sub = llvm::ConstantFP::get(irgen->GetFloatType(),1.0);
       out = llvm::BinaryOperator::CreateSub(l,sub,"",irgen->GetBasicBlock());
     }
     VarExpr* expr = dynamic_cast<VarExpr*>(left);
     new llvm::StoreInst(out, expr->Store(), irgen->GetBasicBlock());
     return out;
   }
   return out;
}   
ConditionalExpr::ConditionalExpr(Expr *c, Expr *t, Expr *f)
  : Expr(Join(c->GetLocation(), f->GetLocation())) {
    Assert(c != NULL && t != NULL && f != NULL);
    (cond=c)->SetParent(this);
    (trueExpr=t)->SetParent(this);
    (falseExpr=f)->SetParent(this);
}

void ConditionalExpr::PrintChildren(int indentLevel) {
    cond->Print(indentLevel+1, "(cond) ");
    trueExpr->Print(indentLevel+1, "(true) ");
    falseExpr->Print(indentLevel+1, "(false) ");
}
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}

void ArrayAccess::PrintChildren(int indentLevel) {
    base->Print(indentLevel+1);
    subscript->Print(indentLevel+1, "(subscript) ");
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}


void FieldAccess::PrintChildren(int indentLevel) {
    if (base) base->Print(indentLevel+1);
    field->Print(indentLevel+1);
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

void Call::PrintChildren(int indentLevel) {
   if (base) base->Print(indentLevel+1);
   if (field) field->Print(indentLevel+1);
   if (actuals) actuals->PrintAll(indentLevel+1, "(actuals) ");
}

