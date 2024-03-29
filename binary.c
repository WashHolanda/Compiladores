#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "assembly.h"

const char * opcodes[] =  {  "add", "sub", "mult", "divi", "and", "or", "xor", "nor", "sll", "srl", "slt", "lw", "sw", "in", "out", "addi", "subi", "multi", "divim", "slti", "andi", "ori", "beq", "bne", "blt", "bgt", "bleq", "bgeq", "j", "jal", "jst", "spc", "lst", "sst", "hlt" };

const char * opcodeBins[] =   { "0000000", "0000000", "0000000", "0000000", "0000000", "0000000", "0000000", "0000000", "0000000", "0000000", "0000000", "0000001", "0000010", "0000011", "0000100", "0000101", "0000101", "0000101", "0000101", "0000110", "0000110", "0000110", "0000111", "0000111", "0000111", "0000111", "0001000", "0001000", "0001001",
"0001010", "0001011", "0001100", "0001101", "0001110", "1111111" };

const char * regBins[] = {  "00000", "00001", "00010", "00011", "00100", "00101", "00110", "00111", "01000", "01001", "01010", "01011", "01100", "01101", "01110",
                            "01111", "10000", "10001", "10010", "10011", "10100", "10101", "10110", "10111", "11000", "11001", "11010", "11011", "11100", "11101",
                            "11110", "11111" };

const char * functBins[] = { "00000000", "00000001", "00000010", "00000011", "00000100", "00000101", "00000110", "00000111", "00001000", "00001001", "00001010", "", "", "", "",
                            "00", "01", "10", "11", "00", "01", "10", "00", "01", "10", "11", "00", "01", "", "",
                            "", "", "", "","00000000" };

char * getImediate (int im, int size) {
    int i = 0;
    char * bin = (char *) malloc(size + 2);
    size --;
    for (unsigned bit = 1u << size; bit != 0; bit >>= 1) {
        bin[i++] = (im & bit) ? '1' : '0';
    }
    bin[i] = '\0';
    return bin;
}

char * assembly2binary (Instruction i) {
    char * bin = (char *) malloc((35 + 5) * sizeof(char));
      if (i.format == formatR) {
        if(i.opcode == sll || i.opcode == srl)
            //sprintf(bin, "{7'b%s,5'd%d,5'd%d,5'd%d,5'b%s,8'b%s}", opcodeBins[i.opcode], i.reg1, i.reg2, i.reg3, "00000", functBins[i.opcode]); //deslocamento deve ser adicionado no lugar do "00000"
            sprintf(bin, "%s_%s_%s_%s_%s_%s", opcodeBins[i.opcode], regBins[i.reg1], regBins[i.reg2], regBins[i.reg3], "00000", functBins[i.opcode]); //deslocamento deve ser adicionado no lugar do "00000"
        else
            //sprintf(bin, "{7'b%s,5'd%d,5'd%d,5'd%d,5'b%s,8'b%s}", opcodeBins[i.opcode], i.reg1, i.reg2, i.reg3, "00000", functBins[i.opcode]);
            sprintf(bin, "%s_%s_%s_%s_%s_%s", opcodeBins[i.opcode], regBins[i.reg1], regBins[i.reg2], regBins[i.reg3], "00000", functBins[i.opcode]);
    }
    else if (i.format == formatIorD) {
        if(i.opcode == lw || i.opcode == sw || i.opcode == lst ||i.opcode == sst)
            sprintf(bin, "%s_%s_%s_%s", opcodeBins[i.opcode], regBins[i.reg1], regBins[i.reg2], getImediate(i.im, 18));
            //sprintf(bin, "{7'b%s,5'd%d,5'd%d,18'd%d}", opcodeBins[i.opcode], i.reg1, i.reg2, i.im);
        else
            //sprintf(bin, "{7'b%s,5'd%d,5'd%d,16'd%d,2'b%s}", opcodeBins[i.opcode], i.reg1, i.reg2, i.im, functBins[i.opcode]);
            sprintf(bin, "%s_%s_%s_%s_%s", opcodeBins[i.opcode], regBins[i.reg1], regBins[i.reg2], getImediate(i.im, 16), functBins[i.opcode]);
    }
    else if (i.format == formatO) {
        if (i.opcode == in)
            //sprintf(bin, "{7'b%s,5'd%d,5'd%d,%s}", opcodeBins[i.opcode], 0, i.reg1, "000000000000000000");
            sprintf(bin, "%s_%s_%s_%s", opcodeBins[i.opcode], regBins[$zero], regBins[i.reg1], "000000000000000000");
        if (i.opcode == out)
            //sprintf(bin, "{7'b%s,5'd%d,%s}", opcodeBins[i.opcode], i.reg1, "00000000000000000000000");
            sprintf(bin, "%s_%s_%s", opcodeBins[i.opcode], regBins[i.reg1], "00000000000000000000000");
    }
    else {
        if (i.opcode == jst || i.opcode == spc)
            //sprintf(bin, "{7'b%s,%s}", opcodeBins[i.opcode], "0000000000000000000000000000");
            sprintf(bin, "%s_%s", opcodeBins[i.opcode], "0000000000000000000000000000");
        else
            //sprintf(bin, "{7'b%s,28'd%d}", opcodeBins[i.opcode], i.im);
            sprintf(bin, "%s_%s", opcodeBins[i.opcode], getImediate(i.im, 28));
    }
    return bin;
}

void generateBinary (AssemblyCode head) {
    AssemblyCode a = head;
    FILE * c = code;
    char * bin;

    printf("\nC- Binary Code\n");

    while (a != NULL) {
        if (a->kind == instr) {
            /*fprintf(c, "ram[%d] = ", a->lineno);
            printf("ram[%d] = ", a->lineno);*/
            fprintf(c, "ram[%d] = 35'b", a->lineno);
            printf("ram[%d] = 35'b", a->lineno);
            bin = assembly2binary(a->line.instruction);
            fprintf(c, "%s;// %s\n", bin,opcodes[a->line.instruction.opcode]);
            printf("%s;// %s\n", bin, opcodes[a->line.instruction.opcode]);
        }
        else {
            fprintf(c, "//%s\n", a->line.label);
            printf("//.%s\n", a->line.label);
        }
        a = a->next;
    }
}
