#include <stdio.h>
#include <string.h>

#define MAX_INSTRUCTIONS 100
#define DATA_REGISTER_COUNT 8
#define WORD_SIZE 16
#define BYTE_SIZE 8

#define OPCODE_MASK 0xF000
#define DR_MASK 0x0E00
#define SR_MASK 0x01C0
#define SR1_MASK 0x01C0
#define SR2_MASK 0x0007
#define BASE_MASK 0x01C0
#define PC_OFFSET9_MASK 0x01FF

#define BIT_MASK 0x0001
#define HALT_INSTRUCTION 0xF025


void initialiseState(unsigned short dataRegisters[], unsigned short *ir, unsigned short *pc, char *cc){
    for (int i=0; i<DATA_REGISTER_COUNT; i++) {
        dataRegisters[i] = 0; 
    }
    *ir = 0;
    *pc = 0;
    *cc = 'Z';
    
}
void printState (unsigned short dataRegisters[], unsigned short ir, unsigned short pc, unsigned short cc){
    int i;
    for (i=0; i<DATA_REGISTER_COUNT; i++){
        printf("R%d\t0x%04x\n", i, dataRegisters[i]);
    }
    printf("PC\t0x%04x\n", pc);
    printf("IR\t0x%04x\n", ir);
    printf("CC\t%c\n", cc);
}

int loadMemory(char *filename, unsigned short *pc, unsigned short memory[]){
    FILE *file;
    file = fopen(filename, "r");
    
    int byteCount = 0;
    int c;

    unsigned short data = 0;
    int dataCount = -1;

    while((c = getc(file))!=EOF){
        data = data << BYTE_SIZE;
        data = data | c;
        byteCount = byteCount + 1;

        if (byteCount % (WORD_SIZE/BYTE_SIZE) == 0){
            if (dataCount <0){
                *pc = data;
            }else{
            memory[dataCount] = data;
            }
            data = 0;
            dataCount = dataCount + 1;
        }
    }
    fclose(file);
    return byteCount;
}

void setCC (unsigned dr, char *cc, unsigned short dataRegisters[]){ 

    signed short value = dataRegisters[dr];

    if (value > 0){
        *cc = 'P';
    } else if (value < 0){
        *cc = 'N';
    } else{
        *cc = 'Z';
    }
}

unsigned short sext(unsigned short data, int length){
    return((signed short)(data << (WORD_SIZE - length))) >> (WORD_SIZE - length);
}

void processInstruction (unsigned short dataRegisters[], unsigned short *ir, unsigned short *pc, char *cc, unsigned short memory[], unsigned short startingAddress){
    *ir = memory[*pc - startingAddress];
    *pc = *pc +1;

    unsigned opCode = (((*ir) & OPCODE_MASK) >> 12);
    unsigned dr = (((*ir) & DR_MASK) >> 9);
    unsigned pcOffset9 = ((*ir) & PC_OFFSET9_MASK);
    

    switch (opCode){
        case 0x2: //LD
            dataRegisters[dr] = memory[(*pc + sext(pcOffset9, 9) - startingAddress)];
            setCC(dr, cc, dataRegisters);
            break;
        case 0xE: //LEA
            dataRegisters[dr] = *pc + sext(pcOffset9, 9);
            setCC(dr, cc, dataRegisters);
            break;
        default:
            break;
    }
}


int main(int argc, char *argv[]){
    unsigned short dataRegisters[DATA_REGISTER_COUNT];
    unsigned short memory[MAX_INSTRUCTIONS];

    unsigned short ir, pc;
    char cc;

    initialiseState(dataRegisters, &ir, &pc, &cc);
    
    int byteCount;
    byteCount = loadMemory(argv[1], &pc, memory);

    /*
    printf("Initial state\n");
    printState(dataRegisters, ir, pc, cc);
    printf("==================\n");
    */
    
    unsigned short startingAddress = pc;

    int executionCount = 0;
    while(ir != HALT_INSTRUCTION){
        unsigned short instruction = ir;
        processInstruction(dataRegisters, &ir, &pc, &cc, memory, startingAddress);
        unsigned opCode = ((ir & OPCODE_MASK) >> 12);
	
        if (opCode == 0xE){
            printf("after executing instruction\t0x%04x\n", ir);
            printState(dataRegisters, ir, pc, cc);
            printf("==================\n");
            executionCount = executionCount + 1;
        }
    }

    return 0;
}



