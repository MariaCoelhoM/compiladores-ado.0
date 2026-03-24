#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEM_SIZE 256

typedef struct {
    char name[5];
    uint8_t opcode;
    int has_operand;
} Instruction;

Instruction table[] = {
    {"NOP", 0x00, 0}, {"STA", 0x10, 1}, {"LDA", 0x20, 1},
    {"ADD", 0x30, 1}, {"OR",  0x40, 1}, {"AND", 0x50, 1},
    {"NOT", 0x60, 0}, {"JMP", 0x80, 1}, {"JN",  0x90, 1},
    {"JZ",  0xA0, 1}, {"HLT", 0xF0, 0}
};

int main(int argc, char *argv[]) {
    // Agora o programa avisa que o padrão é binario.mem
    if (argc < 2) {
        printf("Uso: %s <entrada.txt> [saida.mem]\n", argv[0]);
        return 1;
    }

    char *arquivo_saida = (argc > 2) ? argv[2] : "binario.mem";

    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(arquivo_saida, "wb");
    
    if (!in || !out) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    uint8_t memory[MEM_SIZE] = {0};
    char line[100], mnemonic[10];
    int operand, pc = 0;

    // Cabeçalho Neander: 03 4E 00 00
    uint8_t header[] = {0x03, 0x4E, 0x00, 0x00};
    fwrite(header, 1, 4, out);

    while (fgets(line, sizeof(line), in)) {
        if (line[0] == ';' || line[0] == '\n' || line[0] == '\r') continue;

        int num_items = sscanf(line, "%s %d", mnemonic, &operand);
        if (num_items <= 0) continue;

        int found = 0;
        for (int i = 0; i < 11; i++) {
            if (strcasecmp(mnemonic, table[i].name) == 0) {
                memory[pc++] = table[i].opcode;
                if (table[i].has_operand) {
                    memory[pc++] = (uint8_t)operand;
                }
                found = 1;
                break;
            }
        }
        
        if (!found && strcasecmp(mnemonic, "DATA") == 0) {
            memory[pc++] = (uint8_t)operand;
            found = 1;
        }
    }

    fwrite(memory, 1, MEM_SIZE, out);
    
    fclose(in);
    fclose(out);
    printf("Sucesso! Arquivo binario '%s' gerado.\n", arquivo_saida);
    return 0;
}