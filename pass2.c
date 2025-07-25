#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fin, *flen, *fsym, *foptab;
    char label[20], opcode[20], operand[20];
    char symLabel[20], symAddr[10], optabOp[20], optabCode[10];
    int len, found;

    fin = fopen("input.txt", "r");
    flen = fopen("length.txt", "r");
    fsym = fopen("symtab.txt", "r");

    fscanf(fin, "%s%s%s", label, opcode, operand);
    char start[10];
    strcpy(start, operand);

    fscanf(flen, "%d", &len);
    printf("H^%s^%06s^%06d\n", label, start, len);

    fscanf(fin, "%s%s%s", label, opcode, operand);
    printf("T^%06s^", start);

    while (strcmp(opcode, "END") != 0) {
        foptab = fopen("optab.txt", "r");
        found = 0;

        while (fscanf(foptab, "%s%s", optabOp, optabCode) != EOF) {
            if (strcmp(opcode, optabOp) == 0) {
                // Found in OPTAB
                rewind(fsym);
                while (fscanf(fsym, "%s%s", symAddr, symLabel) != EOF) {
                    if (strcmp(operand, symLabel) == 0) {
                        printf("%s%s^", optabCode, symAddr);
                        found = 1;
                        break;
                    }
                }
                break;
            }
        }
        fclose(foptab);

        if (!found) {
            if (strcmp(opcode, "WORD") == 0) {
                printf("%06d^", atoi(operand));
            } else if (strcmp(opcode, "BYTE") == 0) {
                int i;
                for (i = 2; i < strlen(operand) - 1; i++) {
                    printf("%X", operand[i]);
                }
                printf("^");
            }
        }

        fscanf(fin, "%s%s%s", label, opcode, operand);
    }

    printf("\nE^%06s\n", start);

    fclose(fin);
    fclose(flen);
    fclose(fsym);
    return 0;
}
