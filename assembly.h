#define nregisters 32
#define nregtemp 10
#define nregparam 5

#define process0 0 // Instruções Sistema Operacional
#define GPprocess0 42 // $gp do Sistema Operacional
#define SPprocess0 74 // $sp do Sistema Operacional

#define process1 600 // Instruções Fatorial Recursivo
#define GPprocess1 642 // $gp do Fatorial Recursivo
#define SPprocess1 674 // $sp do Fatorial Recursivo

#define process2 900 // Instruções Fibonacci
#define GPprocess2 942 // $gp do Fibonacci
#define SPprocess2 974 // $sp do Fibonacci

typedef enum {  add, sub, mult, divi, and, or, xor, nor, sll, srl, slt, lw, sw, in, out, addi, subi, multi, divim, slti, andi, ori, beq, bne, blt, bgt, bleq, bgeq, j, jal, jst, spc, lst, sst, hlt } InstrKind;

typedef enum {  formatR, formatIorD, formatO, formatJ } InstrFormat;
typedef enum {  instr, lbl } LineKind;

typedef enum {  simple, vector, address } VarKind;

/* enum { $zero, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11, $t12, $t13, $t14, $t15, $t16, $t17, $t18, $p1, $p2, $p3, $p4, $p5, $p6, $p7, $p8, $p9, $p10, $sp, $gp, $ret } Reg;*/

typedef enum { $zero, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $p1, $p2, $p3, $p4, $p5, $so1, $so2, $so3, $so4, $so5, $so6, $so7, $so8, $so9, $so10, $so11, $so12, $so13, $sp, $gp, $ret} Reg;

typedef struct {
    InstrFormat format;
    InstrKind opcode;
    Reg reg1;
    Reg reg2;
    Reg reg3;
    int im;
    char *imlbl;
} Instruction;

typedef struct AssemblyCodeRec {
    int lineno;
    LineKind kind;
    union {
        Instruction instruction;
        char *label;
    } line;
    struct AssemblyCodeRec *next;
} *AssemblyCode;

typedef struct VarListRec {
    char * id;
    int size;
    int memloc;
    VarKind kind;
    struct VarListRec * next;
} *VarList;

typedef struct FunListRec {
    char * id;
    int size;
    int memloc;
    VarList vars;
    struct FunListRec * next;
} *FunList;

void generateAssembly (QuadList head);

AssemblyCode getAssembly ();