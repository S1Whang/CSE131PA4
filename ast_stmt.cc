/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "symtable.h"

#include "irgen.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"                                                   


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

llvm::Value* Program::Emit() {
    // TODO:
    // This is just a reference for you to get started
    //
    // You can use this as a template and create Emit() function
    // for individual node to fill in the module structure and instructions.
    //
    /* 
    IRGenerator irgen;
    llvm::Module *mod = irgen.GetOrCreateModule("Name_the_Module.bc");

    // create a function signature
    std::vector<llvm::Type *> argTypes;
    llvm::Type *intTy = irgen.GetIntType();
    argTypes.push_back(intTy);
    llvm::ArrayRef<llvm::Type *> argArray(argTypes);
    llvm::FunctionType *funcTy = llvm::FunctionType::get(intTy, argArray, false);

    //llvm::Function *f = llvm::cast<llvm::Function>(mod->getOrInsertFunction("foo", intTy, intTy, (Type *)0));
    llvm::Function *f = llvm::cast<llvm::Function>(mod->getOrInsertFunction("Name_the_function", funcTy));
    llvm::Argument *arg = f->arg_begin();
    arg->setName("x");

    // insert a block into the runction
    llvm::LLVMContext *context = irgen.GetContext();
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context, "entry", f);

    // create a return instruction
    llvm::Value *val = llvm::ConstantInt::get(intTy, 1);
    llvm::Value *sum = llvm::BinaryOperator::CreateAdd(arg, val, "", bb);
    llvm::ReturnInst::Create(*context, sum, bb);

    // write the BC into standard output
    llvm::WriteBitcodeToFile(mod, llvm::outs());
    */
    llvm::Module *mod = irgen->GetOrCreateModule("Program.bc");
    for (int i =0; i < decls->NumElements(); i++) {
      decls->Nth(i)->Emit();
    }
    llvm::WriteBitcodeToFile(mod, llvm::outs());
    return NULL;
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
}

llvm::Value *StmtBlock::Emit() {
    for (int i = 0; i < stmts->NumElements(); i++) {
      stmts->Nth(i)->Emit();
    }
    return NULL;
} 

DeclStmt::DeclStmt(Decl *d) {
    Assert(d != NULL);
    (decl=d)->SetParent(this);
}

void DeclStmt::PrintChildren(int indentLevel) {
    decl->Print(indentLevel+1);
}

llvm::Value* DeclStmt::Emit() {
    decl->Emit();
    return NULL;
} 

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && b != NULL);
    (init=i)->SetParent(this);
    step = s;
    if ( s )
      (step=s)->SetParent(this);
}

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    if ( step )
      step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

llvm::Value* ForStmt::Emit(){
    /*
    llvm::LLVMContext *c = irgen->GetContext();
    llvm::Function *function = irgen->GetFunction();
    llvm::BasicBlock *header = llvm::BasicBlock::Create(*c,"header",function);
    llvm::BasicBlock *stepBlock = llvm::BasicBlock::Create(*c,"stepBlock",function);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(*c, "body", function);
    llvm::BasicBlock *footer = llvm::BasicBlock::Create(*c,"footer", function );

    init->Emit();
    llvm::BasicBlock *currentBlock = irgen->GetBasicBlock();
    llvm::BranchInst::Create(header, currentBlock);
    irgen->SetBasicBlock(header);
    llvm::Value* value = test->Emit();
    llvm::BranchInst::Create(bodyBlock, footer, value , header);
    irgen->SetBasicBlock(bodyBlock);
    body->Emit();
    llvm::BranchInst::Create(stepBlock,bodyBlock);
    irgen->SetBasicBlock(stepBlock);
    step->Emit();
    llvm::BranchInst::Create(header, stepBlock);
    return NULL;
    */
    /*
    std::cout << "ForLoop\n" << std::endl;
    llvm::LLVMContext *c = irgen->GetContext();
    llvm::Function *f = irgen->GetFunction();
    llvm::BasicBlock *head = llvm::BasicBlock::Create(*c, "head", f);
    llvm::BasicBlock *step = llvm::BasicBlock::Create(*c, "step", f);
    llvm::BasicBlock *body = llvm::BasicBlock::Create(*c, "body", f);
    llvm::BasicBlock *foot = llvm::BasicBlock::Create(*c, "foot", f);

    init->Emit();
    llvm::BranchInst::Create(head, irgen->GetBasicBlock());
    irgen->SetBasicBlock(head);
    llvm::Value* value = test->Emit();
    llvm::BranchInst::Create(body, foot, value, head);
    irgen->SetBasicBlock(body);
    body->Emit();
    llvm::BranchInst::Create(step,body);
    irgen->SetBasicBlock(step);
    step->Emit();
    llvm::BranchInst::Create(head, step);
i*/    return NULL;
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}



llvm::Value* WhileStmt::Emit(){
/*
    llvm::LLVMContext *c = irgen->GetContext();
    llvm::BasicBlock *header = llvm::BasicBlock::Create(*c);
    llvm::BasicBlock *bodyBlock = llvm::BasicBlock::Create(*c);
    llvm::BasicBlock *footer = llvm::BasicBlock::Create(*c);
    llvm::BasicBlock *currentBlock = irgen->GetBasicBlock();

    llvm::BranchInst::Create(header, currentBlock);
    irgen->SetBasicBlock(header);
    llvm::Value* value = test->Emit();
    llvm::BranchInst::Create(bodyBlock, footer, value, header);
    irgen->SetBasicBlock(bodyBlock);
    body->Emit();
    llvm::BranchInst::Create(header, bodyBlock);
*/
    llvm::LLVMContext *ctx = irgen->GetContext();
    llvm::BasicBlock *headB = llvm::BasicBlock::Create(*ctx, "Head", irgen->GetFunction());
    llvm::BasicBlock *bodyB = llvm::BasicBlock::Create(*ctx, "Body", irgen->GetFunction());
    llvm::BasicBlock *footB = llvm::BasicBlock::Create(*ctx, "Foot", irgen->GetFunction());
    
    llvm::BranchInst::Create(headB, irgen->GetBasicBlock());
    irgen->SetBasicBlock(headB);
    llvm::Value *testB = test->Emit();
    llvm::BranchInst::Create(bodyB, footB, testB, headB);
    irgen->SetBasicBlock(footB);
    body->Emit();
    llvm::BranchInst::Create(headB,footB);
    irgen->SetBasicBlock(footB);

    return NULL;
}


IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    if (test) test->Print(indentLevel+1, "(test) ");
    if (body) body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}

llvm::Value* IfStmt::Emit(){
  llvm::Function *function = irgen->GetFunction();
  llvm::LLVMContext *c = irgen->GetContext();
  llvm::Value* valueB = test->Emit();
  llvm::BasicBlock* footB = llvm::BasicBlock::Create(*c, "Foot", function);
  llvm::BasicBlock* elseB = NULL;
  if(elseBody != NULL)
    elseB = llvm::BasicBlock::Create(*c, "else", function);
  llvm::BasicBlock* thenB = llvm::BasicBlock::Create(*c, "then", function);
  llvm::BranchInst::Create(thenB,elseBody?elseB:footB,valueB, irgen->GetBasicBlock());
  symtable->Push();
  //thenB->moveAfter(irgen->GetBasicBlock());
  irgen->SetBasicBlock(thenB);
  body->Emit();
  symtable->Pop();
  //elseB->moveAfter(thenB);
  llvm::BranchInst::Create(footB, thenB);
  if (elseBody != NULL) {
    irgen->SetBasicBlock(elseB);
    elseBody->Emit();
    llvm::BranchInst::Create(footB,elseB);
  }
  //foot->moveAfter(elseB);
  //llvm::BranchInst::Create(foot, elseB);
  irgen->SetBasicBlock(footB);
  return NULL;
}




ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    expr = e;
    if (e != NULL) expr->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    if ( expr ) 
      expr->Print(indentLevel+1);
}

llvm::Value *ReturnStmt::Emit() {
  llvm::LLVMContext *c = irgen->GetContext();
  llvm::BasicBlock *bb = irgen->GetBasicBlock();
  if (expr != NULL) {
    llvm::Value *retExpr = expr->Emit();  
    llvm::ReturnInst::Create(*c, retExpr, bb);
  }
  else
    llvm::ReturnInst::Create(*c, bb); 
  return NULL;
}
SwitchLabel::SwitchLabel(Expr *l, Stmt *s) {
    Assert(l != NULL && s != NULL);
    (label=l)->SetParent(this);
    (stmt=s)->SetParent(this);
}

SwitchLabel::SwitchLabel(Stmt *s) {
    Assert(s != NULL);
    label = NULL;
    (stmt=s)->SetParent(this);
}

void SwitchLabel::PrintChildren(int indentLevel) {
    if (label) label->Print(indentLevel+1);
    if (stmt)  stmt->Print(indentLevel+1);
}

SwitchStmt::SwitchStmt(Expr *e, List<Stmt *> *c, Default *d) {
    Assert(e != NULL && c != NULL && c->NumElements() != 0 );
    (expr=e)->SetParent(this);
    (cases=c)->SetParentAll(this);
    def = d;
    if (def) def->SetParent(this);
}

void SwitchStmt::PrintChildren(int indentLevel) {
    if (expr) expr->Print(indentLevel+1);
    if (cases) cases->PrintAll(indentLevel+1);
    if (def) def->Print(indentLevel+1);
}

