#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* getSymbolAddress(char *symbol) {
    static char address[10];
    FILE *fsym = fopen("symtab.txt", "r");
    char sym[20], addr[10];

    while (fscanf(fsym, "%s%s", sym, addr) != EOF) {
        if (strcmp(sym, symbol) == 0) {
            fclose(fsym);
            strcpy(address, addr);
            return address;
        }
    }

    fclose(fsym);
    return NULL;
}

char* getOpcodeCode(char *opcode) {
    static char code[10];
    FILE *fopt = fopen("optab.txt", "r");
    char op[20], val[10];

    while (fscanf(fopt, "%s%s", op, val) != EOF) {
        if (strcmp(op, opcode) == 0) {
            fclose(fopt);
            strcpy(code, val);
            return code;
        }
    }

    fclose(fopt);
    return NULL;
}

int main() {
    FILE *fin, *fout, *flen;
    char label[20], opcode[20], operand[20];
    char objectCode[30];
    char *symAddr, *optCode;
    int startAddr, currAddr, length;
    char textRecord[1000] = "";
    int textLen = 0;
    char programName[20];

    fin = fopen("intermediate.txt", "r");
    fout = fopen("output.txt", "w");
    flen = fopen("length.txt", "r");

    if (!fin || !fout || !flen) {
        printf("Error: missing input files.\n");
        return 1;
    }

    // Read the first line (START)
    fscanf(fin, "%X%s%s%s", &startAddr, programName, opcode, operand);
    fscanf(flen, "%d", &length);

    // Write header record
    fprintf(fout, "H^%s^%06X^%06X\n", programName, startAddr, length);

    // Begin Text record
    int textStartAddr = startAddr;
    textRecord[0] = '\0';
    textLen = 0;

    while (fscanf(fin, "%X%s%s%s", &currAddr, label, opcode, operand) != EOF) {
        if (strcmp(opcode, "END") == 0) break;

        objectCode[0] = '\0';

        optCode = getOpcodeCode(opcode);
        if (optCode != NULL) {
            symAddr = getSymbolAddress(operand);
            if (symAddr != NULL)
                sprintf(objectCode, "%s%s", optCode, symAddr);
            else
                sprintf(objectCode, "%s0000", optCode);  // symbol not found
        } else if (strcmp(opcode, "BYTE") == 0) {
            if (operand[0] == 'C') {
                int i;
                for (i = 2; operand[i] != '\''; i++) {
                    sprintf(objectCode + strlen(objectCode), "%X", operand[i]);
                }
            } else if (operand[0] == 'X') {
                strncpy(objectCode, operand + 2, strlen(operand) - 3);
                objectCode[strlen(operand) - 3] = '\0';
            }
        } else if (strcmp(opcode, "WORD") == 0) {
            int val = atoi(operand);
            sprintf(objectCode, "%06X", val);
        } else {
            // RESW / RESB — skip, no object code
            continue;
        }

        // Append object code to T record
        if (strlen(objectCode) > 0) {
            strcat(textRecord, "^");
            strcat(textRecord, objectCode);
            textLen += strlen(objectCode) / 2;
        }
    }

    // Write Text Record
    fprintf(fout, "T^%06X^%02X%s\n", textStartAddr, textLen, textRecord);

    // End record
    fprintf(fout, "E^%06X\n", textStartAddr);

    fclose(fin);
    fclose(fout);
    fclose(flen);

    printf("✅ Pass 2 completed. Check output.txt for object code.\n");
    return 0;
}
