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
   if (type == (llvm::Type*)irgen->GetIntType()) 
     ops = llvm::CmpInst::ICmp;
   else
     ops = llvm::CmpInst::FCmp;
   if ((string(op->getName())) == "<") {
     llvm::CmpInst::Predicate pred;
     if (type == (llvm::Type*)irgen->GetIntType())
       pred = llvm::ICmpInst::ICMP_SLT;
     else
       pred = llvm::ICmpInst::FCMP_OLT;
     return llvm::CmpInst::Create(ops,pred,l,r,"",irgen->GetBasicBlock()); 
   }
   return NULL;
}
llvm::Value* AssignExpr::Emit() {
   llvm::Value *l;
   llvm::Value *r = right->Emit();
   llvm::Value *o = NULL;
   if ((string(op->getName())) == "=") {
   }
   if ((string(op->getName())) == "+=") {
   }
   if ((string(op->getName())) == "-=") {
   }
   if ((string(op->getName())) == "*=") {
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
   if ((string(op->getName())) == "+") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateAdd(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFAdd(l,r,"",irgen->GetBasicBlock());
   }
   if ((string(op->getName())) == "-") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateSub(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFSub(l,r,"",irgen->GetBasicBlock());
   }
   if ((string(op->getName())) == "*") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateMul(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFMul(l,r,"",irgen->GetBasicBlock());
   }
   if ((string(op->getName())) == "/") {
     if (t == (llvm::Type*)irgen->GetIntType())
       out = llvm::BinaryOperator::CreateSDiv(l,r,"",irgen->GetBasicBlock());
     else
       out = llvm::BinaryOperator::CreateFDiv(l,r,"",irgen->GetBasicBlock());
   }
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
  //new llvm::LoadInst(NULL,"",irgen->GetBasicBlock());
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
     new llvm::StoreInst(out, NULL, irgen->GetBasicBlock());
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

