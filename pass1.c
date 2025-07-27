#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fin, *fsym, *flen, *finter;
    char label[20], opcode[20], operand[20], line[100];
    int addr = 0, startAddr = 0, length = 0;

    fin = fopen("input.txt", "r");
    if (fin == NULL) {
        printf("Error: Cannot open input file.\n");
        return 1;
    }

    fsym = fopen("symtab.txt", "w");
    flen = fopen("length.txt", "w");
    finter = fopen("intermediate.txt", "w");

    // Read first line to check for START directive
    fgets(line, sizeof(line), fin);
    sscanf(line, "%s%s%s", label, opcode, operand);

    // Handle START
    if (strcmp(opcode, "START") == 0) {
        sscanf(operand, "%X", &startAddr);
        addr = startAddr;
        printf("START at address %04X\n", addr);
        fprintf(finter, "%04X\t%s\t%s\t%s\n", addr, label, opcode, operand);
    } else {
        printf(" Warning: START directive missing. Assuming address 0000.\n");
        addr = 0;
    }

    // Read next line
    while (fgets(line, sizeof(line), fin)) {
        // Reset label, opcode, operand for each line
        strcpy(label, "-");
        strcpy(opcode, "-");
        strcpy(operand, "-");

        int count = sscanf(line, "%s%s%s", label, opcode, operand);
        if (count == 2) {  // No label
            strcpy(operand, opcode);
            strcpy(opcode, label);
            strcpy(label, "-");
        }

        // Stop at END directive
        if (strcmp(opcode, "END") == 0) {
            fprintf(finter, "%04X\t%s\t%s\t%s\n", addr, label, opcode, operand);
            break;
        }

        // Write to intermediate file
        fprintf(finter, "%04X\t%s\t%s\t%s\n", addr, label, opcode, operand);

        // Symbol Table: Add label if not "-"
        if (strcmp(label, "-") != 0) {
            fprintf(fsym, "%s\t%04X\n", label, addr);
        }

        // Calculate address increment based on opcode
        if (strcmp(opcode, "WORD") == 0) {
            addr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            addr += 3 * atoi(operand);
        } else if (strcmp(opcode, "RESB") == 0) {
            addr += atoi(operand);
        } else if (strcmp(opcode, "BYTE") == 0) {
            if (operand[0] == 'X')
                addr += (strlen(operand) - 3) / 2;  // X'F1'
            else if (operand[0] == 'C')
                addr += strlen(operand) - 3;       // C'EOF'
        } else {
            // Assume any valid instruction takes 3 bytes
            addr += 3;
        }
    }

    // Calculate length
    length = addr - startAddr;
    printf("Program length = %d (decimal) / %04X (hex)\n", length, length);
    fprintf(flen, "%d", length);

    // Close all files
    fclose(fin);
    fclose(fsym);
    fclose(flen);
    fclose(finter);

    printf("Pass 1 completed successfully.\n");
    return 0;
}
