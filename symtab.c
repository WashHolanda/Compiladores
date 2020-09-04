#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* A função hash */
static int hash ( char * key ){
  int temp = 0;
  int i = 0;
  while (key[i] != '\0'){ 
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno, int op, char* escopo, dataTypes RetType, dataTypes StmtType, IDTypes IType, int vet ){
  int h = hash(name);
  BucketList l =  hashTable[h];

  if(IType == CALL){
    // Procura a ultima declaração com o mesmo nome
    while ((l != NULL)){
      if(((strcmp(name,l->name) == 0))){
        if(l->IType == CALL){
          break;
        }
      }
      l = l->next;
    }
    if(l == NULL){
      l = (BucketList) malloc(sizeof(struct BucketListRec));
      l->name = name;
      l->lines = (LineList) malloc(sizeof(struct LineListRec));
      l->lines->lineno = lineno;
      l->vet = vet;
      l->memloc = op;
      l->IType = IType;
      l->RetType = RetType;
      l->StmtType = StmtType;
      l->escopo = escopo;
      l->lines->next = NULL;
      l->next = hashTable[h];
      hashTable[h] = l;
      return;
    }
    else{
      LineList t = l->lines;
      while (t->next != NULL) t = t->next;
      t->next = (LineList) malloc(sizeof(struct LineListRec));
      t->next->lineno = lineno;
      t->next->next = NULL;
      return ;
    }
  }

// Procura a ultima declaração com o mesmo nome
  while ((l != NULL) && ((strcmp(name,l->name) != 0))){
    l = l->next;
  }

  //Para inserir: não achou outra declaração, se achou verificar se o escopo é DIF e não é uma função
  if ( l == NULL || (op != 0 && l->escopo != escopo && l->IType != FUN && l->IType != CALL)) { /* variável não está na tabela ainda */
    if(l != NULL && strcmp(l->name,name) == 0 && strcmp(l->escopo,"global") == 0){
      fprintf(listing,"[%d] Erro semântico! Variavel '%s' já declarada no escopo global.\n",lineno,name);
      Error = TRUE;
    }
    else{
      l = (BucketList) malloc(sizeof(struct BucketListRec));
      l->name = name;
      l->lines = (LineList) malloc(sizeof(struct LineListRec));
      l->lines->lineno = lineno;
      l->vet = vet;
      if(IType != RETT) l->memloc = op;
      else l->memloc = -1;
      l->IType = IType;
      l->RetType = RetType;
      l->StmtType = StmtType;
      l->escopo = escopo;
      l->lines->next = NULL;
      l->next = hashTable[h];
      hashTable[h] = l;
    }
  }
  else if( (l->IType == FUN  && IType == VAR) || (l->IType == CALL  && IType == VAR)){
    fprintf(listing,"Erro: Nome da variavel '%s' já utilizada como nome de função.[%d]\n",name, lineno);
    Error = TRUE;
  }
  else if( l->escopo == escopo && op != 0 ){
    fprintf(listing,"Erro: Variavel '%s' já declarada neste escopo.[%d]\n",name, lineno);
    Error = TRUE;
  }
  else if(l->escopo != escopo){
    while ((l != NULL)){
      if((strcmp(l->escopo, "global")==0)&& ((strcmp( name,l->name) == 0))){
        LineList t = l->lines;
        while (t->next != NULL) t = t->next;
        t->next = (LineList) malloc(sizeof(struct LineListRec));
        t->next->lineno = lineno;
        t->next->next = NULL;
        break;
      }
      l = l->next;
    }
    if(l == NULL){
      fprintf(listing,"Erro: Variavel '%s' não declarada neste escopo.[%d]\n",name, lineno);
      Error = TRUE;
    }
  }
  else if(op == 0){
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

int st_lookup ( char * name, char * escopo){
  int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0) && (strcmp(escopo,l->escopo) != 0)){
    l = l->next;
  }
  if (l == NULL) return -1;
  else {
    return 0;
  }
}

/*Função que confere se a main() foi declarada */
void busca_main () {
  int h = hash("main");
  BucketList l =  hashTable[h];
  while ((l != NULL) && ((strcmp("main",l->name) != 0 || l->IType == VAR)))
    l = l->next;
  if (l == NULL) {
    fprintf(listing,"Erro: Função main não declarada\n");
    Error = TRUE;
  }
}

int getMemLoc(char* nome, char* escopo){
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if(strcmp(escopo,l->escopo) == 0) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->memloc;
}

dataTypes getFunStmt(char* nome){
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if (l->IType == FUN) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->StmtType;
}

int checkReturn(char* escopo){
  char nome[6] = "return";
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if (strcmp(escopo,l->escopo) == 0) return 1;
    }
    l = l->next;
  }
  return -1;
}

dataTypes getFunType(char* nome){
  int h = hash(nome);
  BucketList l =  hashTable[h];
  while ((l != NULL)){
    if (strcmp(nome,l->name) == 0){
      if (l->IType == FUN) break;
    }
    l = l->next;
  }
  if (l == NULL) return -1;
  else return l->RetType;
}

void printSymTab(FILE * listing) {
  int i;
  
  printf("\nTabela de Simbolos:\n");
  fprintf(listing,"Nome           Escopo  Tipo ID  Tipo Retorno  Tipo Param  Mem. Loc.  Num da linha\n");
  fprintf(listing,"-------------  ------  -------  ------------  ----------  ---------  ------------\n");
  for (i=0;i<SIZE;++i) {
    if (hashTable[i] != NULL) {
      BucketList l = hashTable[i];
      while (l != NULL) {
        LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
        fprintf(listing,"%-6s  ",l->escopo);
        char* id, *data;
        switch(l->IType){
          case RETT:
            id = "ret";
          break;
          case VAR:
            id = "var";
          break;
          case FUN:
             id = "fun";
          break;
          case CALL:
             id = "call";
          break;
          case VET:
            id= "vet";
          break;
          default:
          break;
        }
        switch(l->RetType){
          case INTTYPE:
            data= "INT";
          break;
          case VOIDTYPE:
            data= "VOID";
          break;
          case NULLL:
            data = "null";
          break;
          default:
          break;
        }
        fprintf(listing,"%-7s  ",id);
        fprintf(listing,"%-12s  ",data);

        switch(l->StmtType){
          case INTTYPE:
            data= "INT";
          break;
          case VOIDTYPE:
            data= "VOID";
          break;
          case NULLL:
            data = "null";
          break;
          default:
          break;
        }
        fprintf(listing,"%-10s ",data);
        if(l->memloc != -1) fprintf(listing,"    %-3d     ",l->memloc);
        else fprintf(listing,"   NULL     ");
        while (t != NULL) {
          fprintf(listing,"%3d; ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
