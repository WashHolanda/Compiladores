%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static TreeNode * savedTree; /* armazena a arvore sintática para retorno posterior*/
static int yylex(void);
int yyerror(char *message);
char *scope = "Global";
char * idtype = "";
char * datatype = "";
static char *savedname;
int flag = 0;
int params = 0;
%}

//Alfabeto de Tokens recebidos pelo Parser
%start init
%token IF ELSE WHI RET VOID
%right INT
%token ATR PEV ACH FCH ACO FCO MAIO MENO MAIG MEIG DIF IGL VIRG TB LINE SPACE
%token FIM ERR
%token ID NUM 
%left SOM SUB
%left MUL DIV
%token APR FPR
%nonassoc FPR
%nonassoc ELSE

//Para mostrar o valor semântico do token quando for debugar o parser
%printer { fprintf (yyoutput, "’%d’", $$); } NUM

//Expressões Regulares
%%

init:  lista-dec { savedTree = $1; }
;

lista-dec:  lista-dec declaracao | declaracao
;

declaracao:  var-dec| fun-dec
;

var-dec:  tipo ID PEV | tipo ID ACO NUM FCO PEV | error {yyerrok;}
;

tipo: INT | VOID
;

fun-dec: tipo ID APR parametros FPR escopo
;

parametros: VOID | lista-parametros
;

lista-parametros: lista-parametros VIRG tipo-parametro | tipo-parametro
;

tipo-parametro: tipo ID | tipo ID ACO FCO
;

escopo: ACH dec-locais lista-dec-locais FCH | ACH FCH | ACH dec-locais FCH |ACH lista-dec-locais FCH |
;

dec-locais: dec-locais var-dec | var-dec
;

lista-dec-locais: lista-dec-locais dec-interna | error {yyerrok;}
;
 
dec-interna: exp-dec | escopo | sel-dec | iteracao-dec | retorno-dec
;

exp-dec: exp PEV | PEV
;

sel-dec: IF APR exp FPR dec-interna | IF APR exp FPR dec-interna ELSE dec-interna
;

iteracao-dec: WHI APR exp FPR dec-interna
;

retorno-dec: RET PEV | RET exp PEV
;

exp: var ATR exp | exp-simples
;

var: ID | ID ACO exp FCO
;

exp-simples: exp-soma relacional exp-soma | exp-soma
;

relacional: MEIG | MENO | MAIO | MAIG | IGL | DIF
;

exp-soma: exp-soma soma termo | termo
;

soma: SOM | SUB
;

termo: termo mult fator | fator
;

mult: MUL | DIV 
;

fator: APR exp FPR | var | ativacao | NUM
;

ativacao: ID APR args FPR | ID APR FPR
;

args: args VIRG exp | exp
;

%%

int yyerror(char *message){
    fprintf(listing,"Erro sintático na linha %d\n",lineno);
    fprintf(listing,"Token: ");
    printToken(yychar,tokenString);
    Error = TRUE;
    return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}