#define nregisters 32
#define nregtemp 18
#define nregparam 10

#define gploc 0 // tam 32
#define sploc 32  // tam 32 
//#define raloc 47 // tam 16

typedef enum {  add, sub, mult, divi, and, or, xor, nor, sll, srl, slt, lw, sw, in, out, addi, subi, multi, divim, slti, andi, ori, beq, bne, blt, bgt, bgeq, bleq, j, jal, jst, hlt } InstrKind;

typedef enum {  formatR, formatIorD, formatO, formatJ } InstrFormat;
typedef enum {  instr, lbl } LineKind;

typedef enum {  simple, vector, address } VarKind;

typedef enum { $t1, $t2, $t3, $t4, $t5, $t6, $t7, $t8, $t9, $t10, $t11, $t12, $t13, $t14, $t15, $t16, $t17, $t18, $p1, $p2, $p3, $p4, $p5, $p6, $p7, $p8, $p9, $p10, $sp, $gp, $ret, $zero } Reg;

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

int getSize();