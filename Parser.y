%{
//GLC para gerar parser para calculadora simples
#include <stdio.h>
#define YYDEBUG 1    //Para exibir na tela os passos da análise sintática quando o parser é executado
void yyerror(char *);
extern "C"
{
  int yylex(void);
  void abrirArq();
}
%}

%start init
%token IF ELSE WHI RET
%token ID NUM 
%left SOM SUB
%left MUL DIV
%token ATR PEV INT VOID ACH FCH ACO FCO MAIO MENO MAIG MEIG DIF IGL VIRG TB LINE SPACE
%token APR FPR
%token COM  FIMCOM FIM ERR

//Para mostrar o valor semântico to token quando for debugar o parser
%printer { fprintf (yyoutput, "’%d’", $$); } NUM

%%
init:  lista-dec { savedTree = $1; }
;

lista-dec:  lista-dec declaracao | declaracao
;

declaracao:  var-dec| fun-dec
;

var-dec:  tipo ID PEV | tipo ID ACO NUM FCO PEV
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

escopo: ACH dec-locais lista-dec-locais FCH
;

dec-locais: dec-locais var-dec | /*vazio*/ 
;

lista-dec-locais: lista-dec-locais dec-interna | /*vazio*/ 
dec-interna: exp-dec | escopo | sel-dec | iteracao-dec | retorno-dec
;

exp-dec: exp PEV | PEV
;

sel-dec: IF APR exp FPR dec-interna | IF APR exp FPR dec-interna ELSE dec-interna
;

interacao-dec: WHI APR exp FPR dec-interna
;

retorno-dec: RET PEV | RET exp PEV
;

exp: var IGL exp | exp-simples
;

var: ID | ID ACO exp FCO
;

exp-simples: exp-soma relacional exp-soma | exp-soma
;

relacional: MEIG | MENO | MAIO | MAIG | ATR | DIF
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

ativacao: ID APR args FPR
;

args: arg-lista |  /*vazio*/
;

arg-lista: arg-lista VIRG exp | exp
;

%%

int main(){
  extern int yydebug;
  yydebug = 1;

  printf("\nParser em execução...\n");
  abrirArq();
  return yyparse();
}

void yyerror(char * msg){
  extern char* yytext;
  printf("\n%s : %s %d\n", msg, yytext, yylval);
}