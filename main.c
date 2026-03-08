#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEM_SIZE 256

// Opcodes Neander
typedef enum {
    NOP = 0x00, STA = 0x10, LDA = 0x20, ADD = 0x30, OR  = 0x40,
    AND = 0x50, NOT = 0x60, JMP = 0x80, JN  = 0x90, JZ  = 0xA0, HLT = 0xF0
} Opcode;

typedef struct {
    uint8_t mem[MEM_SIZE];
    uint8_t ac;
    uint8_t pc;
    bool n, z;
    int mem_access;
    int inst_count;
} Neander;

void update_flags(Neander *n) {
    n->z = (n->ac == 0);
    n->n = (n->ac & 0x80) != 0; 
}

void print_output(FILE *dest, Neander *cpu, uint8_t *original_mem, bool hex) {
    fprintf(dest, "\n--- MAPA DE MEMORIA (ANTES) ---\n");
    for (int i = 0; i < MEM_SIZE; i++) {
        fprintf(dest, hex ? "%02X " : "%3d ", original_mem[i]);
        if ((i + 1) % 16 == 0) fprintf(dest, "\n");
    }

    fprintf(dest, "\n--- STATUS FINAL ---\n");
    if (hex) {
        fprintf(dest, "AC: %02X\nPC: %02X\n", cpu->ac, cpu->pc);
    } else {
        fprintf(dest, "AC: %d\nPC: %d\n", cpu->ac, cpu->pc);
    }
    fprintf(dest, "Flag N: %d\nFlag Z: %d\n", cpu->n, cpu->z);
    fprintf(dest, "Acessos a memoria: %d\n", cpu->mem_access);
    fprintf(dest, "Instrucoes executadas: %d\n", cpu->inst_count);

    fprintf(dest, "\n--- MAPA DE MEMORIA (DEPOIS) ---\n");
    for (int i = 0; i < MEM_SIZE; i++) {
        fprintf(dest, hex ? "%02X " : "%3d ", cpu->mem[i]);
        if ((i + 1) % 16 == 0) fprintf(dest, "\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo.mem> [-h para hexadecimal]\n", argv[0]);
        return 1;
    }

    bool use_hex = (argc > 2 && strcmp(argv[2], "-h") == 0);
    Neander cpu = {0};
    uint8_t original_mem[MEM_SIZE];

    FILE *f_in = fopen(argv[1], "rb");
    if (!f_in) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    // Pula o cabeçalho de 4 bytes do Neander (03 4E 00 00)
    fseek(f_in, 4, SEEK_SET);
    fread(cpu.mem, 1, MEM_SIZE, f_in);
    memcpy(original_mem, cpu.mem, MEM_SIZE); // Backup para o "Antes"
    fclose(f_in);

    bool running = true;
    while (running && cpu.pc < MEM_SIZE) {
        uint8_t opcode = cpu.mem[cpu.pc++];
        cpu.mem_access++; 
        cpu.inst_count++;

        switch (opcode) {
            case NOP: break;
            case STA: {
                uint8_t addr = cpu.mem[cpu.pc++];
                cpu.mem[addr] = cpu.ac;
                cpu.mem_access += 2; // +1 ender. instrução, +1 escrita dado
                break;
            }
            case LDA:
            case ADD:
            case OR:
            case AND: {
                uint8_t addr = cpu.mem[cpu.pc++];
                if (opcode == LDA) cpu.ac = cpu.mem[addr];
                else if (opcode == ADD) cpu.ac += cpu.mem[addr];
                else if (opcode == OR)  cpu.ac |= cpu.mem[addr];
                else if (opcode == AND) cpu.ac &= cpu.mem[addr];
                
                cpu.mem_access += 2; // +1 ender. instrução, +1 leitura dado
                update_flags(&cpu);
                break;
            }
            case NOT: {
                cpu.ac = ~cpu.ac;
                update_flags(&cpu);
                break;
            }
            case JMP: {
                cpu.pc = cpu.mem[cpu.pc]; // Salta para o endereço apontado
                cpu.mem_access++; 
                break;
            }
            case JN:
            case JZ: {
                bool condition = (opcode == JN) ? cpu.n : cpu.z;
                if (condition) {
                    cpu.pc = cpu.mem[cpu.pc];
                } else {
                    cpu.pc++; 
                }
                cpu.mem_access++;
                break;
            }
            case HLT: running = false; break;
            default:  running = false; break;
        }
    }

    FILE *f_out = fopen("resultado.txt", "w");
    if (f_out) {
        print_output(f_out, &cpu, original_mem, use_hex);
        fclose(f_out);
    }

    print_output(stdout, &cpu, original_mem, use_hex);

    return 0;
}