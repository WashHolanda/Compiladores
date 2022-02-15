#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <stdio.h>

/* counter for variable memory locations */
static int location = 0;
char* escopo = "global";
int check_return = FALSE;

void UpdateScope(TreeNode * t){
  if (t != NULL && t->kind.exp == FunDeclK){
    escopo = t->attr.name;
    if(getFunType(escopo) == INTTYPE && check_return == TRUE){
      if(checkReturn(escopo) == -1){
        printf("Erro Semantico: Retorno da funcao '%s' inexistente. [%d]\n",escopo,t->lineno);
        Error = TRUE;
      }
    }
  }
}

/* Procedure traverse is a generic recursive syntax tree traversal routine: it applies preProc in
preorder and postProc in postorder to tree pointed to by t */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{
  if (t != NULL){ 
    UpdateScope(t);
    preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    if(t->child[0]!= NULL && t->kind.exp == FunDeclK) escopo = "global";
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}
/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t){ 
  if (t==NULL) return;
  else return;
}

/* O procedimento insertNode insere identificadores armazenados em t na tabela de símbolos */
static void insertNode( TreeNode * t) {
  dataTypes TIPO = NULLL;
  switch (t->nodekind){
    case StmtK:
      switch (t->kind.stmt){
      case ReturnVOID:
        if(getFunType(escopo) == INTTYPE){
          printf("Erro Semantico: Retorno da função '%s' incompatível. [%d]\n",escopo,t->lineno);
          Error = TRUE;
        }
        st_insert("return",t->lineno,0,escopo,INTTYPE, NULLL, RETT, t->vet); 
        break;
      case ReturnINT:
        if(getFunType(escopo) == VOIDTYPE){
          printf("Erro Semantico: Retorno da função '%s' incompatível. [%d]",escopo,t->lineno);
          Error = TRUE;
        }
        st_insert("return",t->lineno,0,escopo,INTTYPE, NULLL, RETT, t->vet); 
        break;
      default:
        break;
      }
      break; 
    case ExpK:
      switch(t->kind.exp){
        case VarDeclK:
          st_insert(t->attr.name,t->lineno,location++,escopo,INTTYPE, TIPO, VAR, t->vet); 
          if (st_lookup(t->attr.name, escopo) == -1)
          /* não encontrado na tabela, inserir*/
            st_insert(t->attr.name,t->lineno,location++, escopo,INTTYPE, TIPO, VAR , t->vet);
          break;
        case FunDeclK:
          location = 0;
          if(strcmp(t->child[1]->attr.name,"VOID") == 0) TIPO = VOIDTYPE;
          else TIPO = INTTYPE;
          if (st_lookup(t->attr.name,escopo) == -1){
            /* não encontrado na tabela, inserir*/
            st_insert(t->attr.name,t->lineno,location++, "global",t->type, TIPO,FUN, t->vet);}
          else{
          /* encontrado na tabela, erro semântico */
            fprintf(listing,"Erro Semantico: Multiplas declarações da função '%s'. [%d]\n", t->attr.name, t->lineno);
            Error = TRUE;
            }
          break;
        case VarParamK:
              st_insert(t->attr.name,t->lineno,location++,escopo,INTTYPE, TIPO, VAR, t->vet);
          break;
        case VetParamK:
              st_insert(t->attr.name,t->lineno,location++,escopo,INTTYPE, TIPO, VET, t->vet);
          break;
        case VetorK:
          st_insert(t->attr.name,t->lineno,location++, escopo,INTTYPE, TIPO, VET, t->vet);
          location += t->child[1]->attr.val - 1;
          break;
        case IdK:
          if(t->add != 1){
            if (st_lookup(t->attr.name, escopo) == -1){
              fprintf(listing,"Erro Semântico: A variavel '%s' não foi declarada. [%d]\n", t->attr.name, t->lineno);
              Error = TRUE;
            }
            else {
              st_insert(t->attr.name,t->lineno,0,escopo,INTTYPE, TIPO,FUN, t->vet);
            }
          }
          break;
        case AtivK:
          if (st_lookup(t->attr.name, escopo) == -1 && strcmp(t->attr.name, "input")!=0 && strcmp(t->attr.name, "output")!=0 && strcmp(t->attr.name, "storePC")!=0 && strcmp(t->attr.name, "storeStack")!=0 && strcmp(t->attr.name, "loadStack")!=0 && strcmp(t->attr.name, "storeRegs")!=0 && strcmp(t->attr.name, "loadRegs")!=0){
            fprintf(listing,"Erro Semântico: A função '%s' não foi declarada. [%d]\n", t->attr.name, t->lineno);
            Error = TRUE;
          }
          else {
            /*if(st_lookup(t->attr.name, escopo) == 0 && aqui ver se o número de param é igual)*/
            st_insert(t->attr.name,t->lineno,location++,escopo,getFunType(t->attr.name), TIPO,CALL, t->vet);
            /*else{
              fprintf(listing,"Erro Semântico: A chamada '%s' tem parâmetros imcompatíveis. [%d]\n", t->attr.name, t->lineno);
            }*/
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree){
  traverse(syntaxTree,insertNode,nullProc);
  busca_main();
  check_return = TRUE;
  typeCheck(syntaxTree);
  if(TraceAnalyze) printSymTab(listing);
}

static void typeError(TreeNode * t, char * message){
  fprintf(listing,"Erro Semantico: %s [%d]\n",message,t->lineno);
  Error = TRUE;
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
void checkNode(TreeNode * t){
  switch (t->nodekind){
  case ExpK:
      switch (t->kind.exp){
      case OpK:
        if((t->child[0] == NULL) || (t->child[1] == NULL)) break;
        if (((t->child[0]->kind.exp == AtivK) &&( getFunType(t->child[0]->attr.name)) == VOIDTYPE) ||
            ((t->child[1]->kind.exp == AtivK) && (getFunType(t->child[1]->attr.name) == VOIDTYPE)))
              typeError(t,"Uma funcao com retorno VOID não pode ser um operando.");
        break;
      case AtivK:
        if (((t->params > 0) && (getFunStmt(t->attr.name)) == VOIDTYPE))
              typeError(t,"Insercao de parametros a uma função do tipo VOID.");
        break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt){
        case AssignK:
          if((t->child[1] == NULL)) break;
          if (t->child[1]->kind.exp == AtivK && getFunType(t->child[1]->attr.name) == VOIDTYPE)
            typeError(t,"Uma funcao com retorno VOID não pode ser atribuida a uma variavel.");
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree){
  traverse(syntaxTree,checkNode, nullProc);
}