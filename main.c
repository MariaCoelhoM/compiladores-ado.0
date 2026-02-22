#include <stdio.h>
#include <stdlib.h>

// Função para retornar o nome da instrução Neander
const char* get_mnemonic(unsigned char opcode) {
    switch (opcode) {
        case 0:   return "NOP";
        case 16:  return "STA";
        case 32:  return "LDA";
        case 48:  return "ADD";
        case 64:  return "OR";
        case 80:  return "AND";
        case 96:  return "NOT";
        case 128: return "JMP";
        case 144: return "JN";
        case 160: return "JZ";
        case 240: return "HLT";
        default:  return ""; // Para dados ou endereços vazios
    }
}

// Verifica se a instrução possui um operando (parâmetro de endereço)
int has_operand(unsigned char opcode) {
    if (opcode == 16 || opcode == 32 || opcode == 48 || 
        opcode == 64 || opcode == 80 || opcode == 128 || 
        opcode == 144 || opcode == 160) {
        return 1;
    }
    return 0;
}

int main() {
    // Abre o arquivo de entrada
    FILE *file_in = fopen("teste salvo.mem", "rb");
    if (!file_in) {
        printf("Erro ao abrir o arquivo .mem\n");
        return 1;
    }

    // Abre o arquivo de saída
    FILE *file_out = fopen("resultado.txt", "w");
    if (!file_out) {
        printf("Erro ao criar o arquivo resultado.txt\n");
        fclose(file_in);
        return 1;
    }

    unsigned char memory[256];
    fread(memory, 1, 256, file_in);
    fclose(file_in);

    // Cabeçalho no arquivo
    fprintf(file_out, "End.  Valor   Mnemônico\n");
    fprintf(file_out, "-----------------------\n");

    for (int i = 0; i < 256; i++) {
        unsigned char opcode = memory[i];
        const char* mnemonic = get_mnemonic(opcode);

        if (has_operand(opcode)) {
            unsigned char operand = memory[i + 1];
            fprintf(file_out, "%3d   %3d %3d   %s %d\n", i, opcode, operand, mnemonic, operand);
            i++; 
        } else {
            if (mnemonic[0] != '\0') {
                fprintf(file_out, "%3d   %3d       %s\n", i, opcode, mnemonic);
            } else {
                fprintf(file_out, "%3d   %3d\n", i, opcode);
            }
        }
    }

    fclose(file_out);
    printf("Desmontagem concluída! Verifique o arquivo 'resultado.txt'.\n");

    return 0;
}