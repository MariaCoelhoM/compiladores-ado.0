#include <stdio.h>
#include <stdlib.h>

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
        default:  return NULL;
    }
}

int has_operand(unsigned char opcode) {
    if (opcode >= 16 && opcode <= 160 && opcode != 96) return 1;
    return 0;
}

int main() {
    FILE *fileIn = fopen("teste salvo.mem", "rb");
    if (!fileIn) {
        perror("Erro ao abrir teste salvo.mem");
        return 1;
    }

    // We laden het hele bestand in een buffer
    fseek(fileIn, 0, SEEK_END);
    long size = ftell(fileIn);
    fseek(fileIn, 0, SEEK_SET);
    unsigned char *buffer = (unsigned char *)malloc(size);
    fread(buffer, 1, size, fileIn);
    fclose(fileIn);

    FILE *fileOut = fopen("resultado.txt", "w");
    fprintf(fileOut, "End.  Cod.  Op.    Mnem.\n");
    fprintf(fileOut, "---------------------------\n");

    // We maken een schone geheugenkaart van 256 bytes
    unsigned char memory[256] = {0};

    /* LOGICA VOOR JOUW BESTAND:
       In jouw 'teste salvo.mem' staan de data bytes om de 2 bytes (bijv. op 4, 6, 8...).
       We gaan de bytes extraheren naar de juiste Neander adressen.
    */
    int memPos = 0;
    for (int i = 4; i < size && memPos < 256; i += 2) {
        memory[memPos] = buffer[i];
        memPos++;
    }

    // Nu printen we de geheugenkaart zoals in teste salvotexto.txt
    for (int i = 0; i < 256; i++) {
        unsigned char opcode = memory[i];
        const char* mnemonic = get_mnemonic(opcode);

        if (mnemonic != NULL && has_operand(opcode) && i < 255) {
            unsigned char operand = memory[i+1];
            fprintf(fileOut, "%3d   %3d  %3d   %s %d\n", i, opcode, operand, mnemonic, operand);
            i++; 
        } else if (mnemonic != NULL) {
            fprintf(fileOut, "%3d   %3d        %s\n", i, opcode, mnemonic);
        } else if (opcode != 0) {
            fprintf(fileOut, "%3d   %3d\n", i, opcode);
        } else {
            fprintf(fileOut, "%3d     0        NOP\n", i);
        }
    }

    free(buffer);
    fclose(fileOut);
    printf("Pronto! Agora o arquivo 'resultado.txt' deve estar igual ao seu texto.\n");
    return 0;
}