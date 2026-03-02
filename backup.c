#include <stdio.h>
#include <stdlib.h>

// Tabela padrão do Neander
const char* get_mnemonic(unsigned char opcode) {
    switch (opcode) {
        case 0:   return "NOP";
        case 16:  return "STA";
        case 32:  return "LDA";
        case 48:  return "ADD"; // Apenas o 48 é ADD agora
        case 64:  return "OR";
        case 80:  return "AND";
        case 96:  return "NOT";
        case 128: return "JMP";
        case 144: return "JN";
        case 160: return "JZ";
        case 240: return "HLT";
        default:  return NULL;
    }
}

int has_operand(unsigned char opcode) {
    // Apenas estas instruções puxam o próximo byte como endereço
    if (opcode == 16 || opcode == 32 || opcode == 48 || 
        opcode == 64 || opcode == 80 || (opcode >= 128 && opcode <= 160)) {
        return 1;
    }
    return 0;
}

int main() {
    FILE *fileIn = fopen("teste salvo.mem", "rb");
    if (!fileIn) {
        perror("Erro ao abrir teste salvo.mem");
        return 1;
    }

    // Carregar o ficheiro binário
    fseek(fileIn, 0, SEEK_END);
    long size = ftell(fileIn);
    fseek(fileIn, 0, SEEK_SET);
    unsigned char *buffer = (unsigned char *)malloc(size);
    fread(buffer, 1, size, fileIn);
    fclose(fileIn);

    unsigned char memory[256] = {0};
    int memPos = 0;

    // Lógica de Extração: 
    // O seu ficheiro parece ter 2 bytes para cada posição de memória.
    // O byte de dados real está na posição par, começando após o cabeçalho "NDR2"
    for (int i = 4; i < size && memPos < 256; i += 2) {
        memory[memPos++] = buffer[i];
    }

    FILE *fileOut = fopen("resultado.txt", "w");
    fprintf(fileOut, "End.  Cod.  Op.    Mnem.\n");
    fprintf(fileOut, "---------------------------\n");

    for (int i = 0; i < 256; i++) {
        unsigned char opcode = memory[i];
        const char* mnemonic = get_mnemonic(opcode);

        if (mnemonic != NULL && has_operand(opcode) && i < 255) {
            unsigned char operand = memory[i + 1];
            fprintf(fileOut, "%3d   %3d  %3d   %s %d\n", i, opcode, operand, mnemonic, operand);
            i++; 
        } else if (mnemonic != NULL) {
            fprintf(fileOut, "%3d   %3d        %s\n", i, opcode, mnemonic);
        } else {
            // Se não for uma instrução conhecida, imprime apenas o valor (como o 50 ou 20)
            if (opcode == 0)
                fprintf(fileOut, "%3d     0        NOP\n", i);
            else
                fprintf(fileOut, "%3d   %3d\n", i, opcode);
        }
    }

    free(buffer);
    fclose(fileOut);
    printf("Conversão terminada! Verifique o resultado.txt\n");
    return 0;
}