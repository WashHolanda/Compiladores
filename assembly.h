#define nregisters 32
#define nregtemp 12
#define nregparam 6
#define nregso 6

#define process0 0 // Instruções Sistema Operacional
#define GPprocess0 42 // $gp do Sistema Operacional
#define SPprocess0 74 // $sp do Sistema Operacional

#define process1 600 // Instruções Fatorial Recursivo
#define GPprocess1 642 // $gp do Fatorial Recursivo
#define SPprocess1 674 // $sp do Fatorial Recursivo

#define process2 900 // Instruções Fibonacci
#define GPprocess2 942 // $gp do Fibonacci
#define SPprocess2 974 // $sp do Fibonacci

extern int condGP;

typedef enum {  add, sub, mult, divi, and, or, xor, nor, sll, srl, slt, lw, sw, in, out, addi, subi, multi, divim, slti, andi, ori, beq, bne, blt, bgt, bleq, bgeq, j, jal, jst, spc, lst, sst, hlt } InstrKind;

typedef enum {  formatR, formatIorD, formatO, formatJ } InstrFormat;
typedef enum {  instr, lbl } LineKind;

typedef enum {  simple, vector, address } VarKind;

typedef enum { $zero, $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11, $t12, $p1, $p2, $p3, $p4, $p5, $p6, $s1, $s2, $s3, $s4, $s5, $s6, $ps1, $ssp, $sgp, $ctrl, $sp, $gp, $ret} Reg;

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