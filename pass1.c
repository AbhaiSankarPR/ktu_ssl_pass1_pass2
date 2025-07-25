#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fin, *fsym, *flen, *foptab;
    char label[20], opcode[20], operand[20], optabOp[20];
    int addr = 0, startAddr = 0, length = 0, opval;

    fin = fopen("input.txt", "r");
    fsym = fopen("symtab.txt", "w");
    flen = fopen("length.txt", "w");

    fscanf(fin, "%s%s%s", label, opcode, operand);

    if (strcmp(opcode, "START") == 0) {
        startAddr = atoi(operand);
        addr = startAddr;
        printf("START at address %d\n", addr);
        fscanf(fin, "%s%s%s", label, opcode, operand);
    }

    while (strcmp(opcode, "END") != 0) {
        if (strcmp(label, "-") != 0) {
            fprintf(fsym, "%s\t%d\n", label, addr);
        }

        if (strcmp(opcode, "WORD") == 0) {
            addr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            addr += 3 * atoi(operand);
        } else if (strcmp(opcode, "RESB") == 0) {
            addr += atoi(operand);
        } else if (strcmp(opcode, "BYTE") == 0) {
            if (operand[0] == 'X')
                addr += 1;
            else
                addr += strlen(operand) - 3; // e.g., C'EOF' = 3
        } else {
            // Assume it's in OPTAB
            addr += 3;
        }

        fscanf(fin, "%s%s%s", label, opcode, operand);
    }

    length = addr - startAddr;
    printf("Program length = %d\n", length);
    fprintf(flen, "%d", length);

    fclose(fin);
    fclose(fsym);
    fclose(flen);
    return 0;
}
