#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Definições de Instruções Neander
#define NOP  0x00
#define STA  0x10
#define LDA  0x20
#define ADD  0x30
#define OR   0x40
#define AND  0x50
#define NOT  0x60
#define JMP  0x80
#define JN   0x90
#define JZ   0xA0
#define HLT  0xF0

typedef struct {
    uint8_t mem[256];
    uint8_t ac;      // Acumulador
    uint8_t pc;      // Program Counter
    bool n, z;       // Flags
    int mem_access;  // Contador de acessos à memória
    int inst_count;  // Contador de instruções
} Neander;

void update_flags(Neander *n) {
    n->z = (n->ac == 0);
    n->n = (n->ac & 0x80) != 0; 
}

void print_mem_map(FILE *out, uint8_t *mem, bool hex) {
    fprintf(out, "Mapa de Memoria:\n");
    for (int i = 0; i < 256; i++) {
        if (hex) fprintf(out, "%02X ", mem[i]);
        else fprintf(out, "%d ", mem[i]);
        if ((i + 1) % 16 == 0) fprintf(out, "\n");
    }
    fprintf(out, "\n");
}

int main(int argc, char *argv[]) {
    bool use_hex = false;
    if (argc > 1 && strcmp(argv[1], "-h") == 0) use_hex = true;

    Neander cpu = {0};
    FILE *f_in = fopen("teste salvo.mem", "rb");
    FILE *f_out = fopen("resultado.txt", "w");

    if (!f_in || !f_out) {
        printf("Erro ao abrir arquivos.\n");
        return 1;
    }

    
    fseek(f_in, 4, SEEK_SET);
    fread(cpu.mem, 1, 256, f_in);
    fclose(f_in);

    fprintf(f_out, "--- ANTES DA EXECUCAO ---\n");
    print_mem_map(f_out, cpu.mem, use_hex);

    bool running = true;
    while (running && cpu.pc < 256) {
        uint8_t opcode = cpu.mem[cpu.pc++];
        cpu.mem_access++;
        cpu.inst_count++;

        switch (opcode) {
            case NOP: break;
            case STA: {
                uint8_t addr = cpu.mem[cpu.pc++];
                cpu.mem[addr] = cpu.ac;
                cpu.mem_access += 2; 
                break;
            }
            case LDA: {
                uint8_t addr = cpu.mem[cpu.pc++];
                cpu.ac = cpu.mem[addr];
                cpu.mem_access += 2;
                update_flags(&cpu);
                break;
            }
            case ADD: {
                uint8_t addr = cpu.mem[cpu.pc++];
                cpu.ac += cpu.mem[addr];
                cpu.mem_access += 2;
                update_flags(&cpu);
                break;
            }
            case OR: {
                uint8_t addr = cpu.mem[cpu.pc++];
                cpu.ac |= cpu.mem[addr];
                cpu.mem_access += 2;
                update_flags(&cpu);
                break;
            }
            case AND: {
                uint8_t addr = cpu.mem[cpu.pc++];
                cpu.ac &= cpu.mem[addr];
                cpu.mem_access += 2;
                update_flags(&cpu);
                break;
            }
            case NOT: {
                cpu.ac = ~cpu.ac;
                update_flags(&cpu);
                break;
            }
            case JMP: {
                cpu.pc = cpu.mem[cpu.pc];
                cpu.mem_access++;
                break;
            }
            case JN: {
                if (cpu.n) cpu.pc = cpu.mem[cpu.pc];
                else cpu.pc++;
                cpu.mem_access++;
                break;
            }
            case JZ: {
                if (cpu.z) cpu.pc = cpu.mem[cpu.pc];
                else cpu.pc++;
                cpu.mem_access++;
                break;
            }
            case HLT: running = false; break;
            default: running = false; break;
        }
    }

    fprintf(f_out, "--- STATUS FINAL ---\n");
    if (use_hex) {
        fprintf(f_out, "AC: %02X\nPC: %02X\n", cpu.ac, cpu.pc);
    } else {
        fprintf(f_out, "AC: %d\nPC: %d\n", cpu.ac, cpu.pc);
    }
    fprintf(f_out, "Flag N: %d\nFlag Z: %d\n", cpu.n, cpu.z);
    fprintf(f_out, "Acessos a memoria: %d\n", cpu.mem_access);
    fprintf(f_out, "Instrucoes executadas: %d\n\n", cpu.inst_count);

    fprintf(f_out, "--- DEPOIS DA EXECUCAO ---\n");
    print_mem_map(f_out, cpu.mem, use_hex);

    fclose(f_out);
    printf("Simulacao concluida. Verifique resultado.txt\n");
    return 0;
}