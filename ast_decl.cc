/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symtable.h"        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

VarDecl::VarDecl(Identifier *n, Type *t, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    typeq = NULL;
}

VarDecl::VarDecl(Identifier *n, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && tq != NULL);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
    type = NULL;
}

VarDecl::VarDecl(Identifier *n, Type *t, TypeQualifier *tq, Expr *e) : Decl(n) {
    Assert(n != NULL && t != NULL && tq != NULL);
    (type=t)->SetParent(this);
    (typeq=tq)->SetParent(this);
    if (e) (assignTo=e)->SetParent(this);
}
  
void VarDecl::PrintChildren(int indentLevel) { 
   if (typeq) typeq->Print(indentLevel+1);
   if (type) type->Print(indentLevel+1);
   if (id) id->Print(indentLevel+1);
   if (assignTo) assignTo->Print(indentLevel+1, "(initializer) ");
}

void VarDecl::Emit() {
  //std::cout << "VarDecl" << std::endl;
  llvm::Type *type = llvm::Type::getVoidTy(*(irgen->GetContext()));
  
  if (this->type == Type::intType) 
  {
    type = irgen->GetIntType();
    //std::cout << "int type" << std::endl;
  }
  else if(this->type == Type::floatType)
  {
      type = irgen->GetFloatType();
      //std::cout << "float type" << std::endl;
  }
  //else
    //std::cout << "not a type yet" << std::endl;
  // float
  // bool
  // vec2
  // vec3
  // vec4

  llvm::Twine *name = new llvm::Twine(this->id->GetName());
  
  // its a global variable 
  if (symtable->GetCurrentIndex() == 0) 
  {
    /* global variable
     * Module
     * Type
     * Constant?
     * Twine(name)
     * GlobalVariable
     * ThreadLocal?
     * AddressSpace
     * Externally Initialized?
     */
    
    llvm::GlobalVariable *global = new llvm::GlobalVariable(
      *(irgen->GetOrCreateModule("")), type, false, 
      llvm::GlobalValue::ExternalLinkage, llvm::Constant::getNullValue(type),
      *name, NULL);
    //string idname = this->id->GetName();
    //llvm::Value *val = dynamic_cast<llvm::Value*>(global);
    //if (val) {
     //std::cout << "true" << std::endl;
     symtable->Insert(this->id->GetName(), global);
    //}
  }
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
    returnTypeq = NULL;
}

FnDecl::FnDecl(Identifier *n, Type *r, TypeQualifier *rq, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r != NULL && rq != NULL&& d != NULL);
    (returnType=r)->SetParent(this);
    (returnTypeq=rq)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::PrintChildren(int indentLevel) {
    if (returnType) returnType->Print(indentLevel+1, "(return type) ");
    if (id) id->Print(indentLevel+1);
    if (formals) formals->PrintAll(indentLevel+1, "(formals) ");
    if (body) body->Print(indentLevel+1, "(body) ");
}

