#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

enum {HLT, LDA, STR, ADD, SUB, JZ, JP, JN, JMP, GET, PTR};

typedef unsigned short word;
typedef unsigned char byte;

word getval(string s) {
    word value;
    cout << ">> " << s << endl;
    cout << ">> ";
    cin >> value;

    return value;
}

class CPU {
    public:
        byte PC, MAR;
        word ACC, R1, IR, MBR;
        bool running, pause;

    CPU() {
        PC = MAR = ACC = R1 = IR = MBR = 0;
        running = pause = false;
    }
};

class Emulator {

    private:
        word memory[256] = {0};
        CPU cpu;

    public:
        void run() {

            cpu.running = true;

            do {

                // Coloca o endereço da próxima instrução a ser executada
                // no registrador de barramento de endereço
                cpu.MAR = cpu.PC;

                // Pega o endereço no registrador no bus de endereço e lê a célula de memória
                // Colocando o valor da célula no registrador de barramento de dados
                cpu.MBR = memory[cpu.MAR];

                // Copia o valor do reg. do bus de dados para o registrador R1
                cpu.R1 = cpu.MBR;

                cpu.PC++; // Aponta para próxima instrução

                // Decodificação
                byte opcode = (cpu.R1 & 0xF00) >> 8;
                byte operand = cpu.R1 & 0xFF;

                switch (opcode) {

                    case HLT:
                        cpu.running = false;
                        break;

                    case LDA:
                        cpu.ACC = memory[operand];
                        break;
                    
                    case STR:
                        memory[operand] = cpu.ACC;
                        break;

                    case ADD:
                        cpu.ACC += memory[operand];
                        break;

                    case SUB:
                        cpu.ACC -= memory[operand];
                        break;

                    case JZ:
                        cpu.PC = (cpu.ACC == 0) ? operand : cpu.PC;
                        break;

                    case JP:
                        cpu.PC = (cpu.ACC > 0) ? operand : cpu.PC;
                        break;

                    case JN:
                        cpu.PC = (cpu.ACC < 0) ? operand : cpu.PC;
                        break;

                    case JMP:
                        cpu.PC = operand;
                        break;

                    case GET:
                        break;
                    
                    case PTR:
                        break;

                    default:
                        cout << "Invalid opcode!" << "(" << opcode << ")" << endl;

                }

            } while (cpu.running);
    
        }  

    Emulator() {
    }

    void load(const char *filename) {
        
        word inst;
        ifstream file;
        file.open(filename, ios::in);

        for (size_t i = 0; file >> inst ; i++){
            memory[i] = inst;
        }

        file.close();
    }

    void dbg() {

        word action;
        cpu.pause = true;

        while (cpu.pause) {

            cout << "\n\n\n|-------[ Debug Mode ]-------|" << endl;
            cout << "|  1: Set PC" << endl;
            cout << "|  2: Set MAR" << endl;
            cout << "|  3: Set ACC" << endl;
            cout << "|  4: Set R1" << endl;
            cout << "|  5: Set IR" << endl;
            cout << "|  6: Modo MBR" << endl;
            cout << "|  7: Set Memory" << endl;
            cout << "|  8: Desable debug" << endl;
            cout << "|  9: Registers" << endl;
            cout << "| 10: Dump memory" << endl;
            cout << ">> ";
            cin >> action;

            switch (action) {

                case 1:
                    cpu.PC = getval("New PC value"); break;
                
                case 2:
                    cpu.MAR = getval("New MAR value"); break;
                
                case 3: 
                    cpu.ACC = getval("New ACC value"); break;
                
                case 4: 
                    cpu.R1 = getval("New R1 value"); break;
                
                case 5: 
                    cpu.IR = getval("New IR value"); break;
                
                case 6: 
                    cpu.MBR = getval("New MBR value"); break;
                
                case 8: cpu.pause = false; break;
                
                case 9: status(true, false); break;

                case 10: status(false, true); break;
                
                case 7:
                    word addr, value;
                    addr = getval("Memory address");
                    value = getval("new value");        
                    memory[addr] = value;
                    break;

            }
        }
    }   

    void status(bool regiters=false, bool memd=false) {

        cout << endl;
        cout << "|----------------------------------" << endl;
        cout << "| SIMPLE EMULATOR " << endl;
        cout << "|----------------------------------" << endl;

        if(regiters) {
            cout << "| Regiters: " << endl;
            cout << "| ACC:   " << cpu.ACC << endl;
            cout << "| R1:    " << cpu.R1 << endl;
            cout << "| IR:    " << cpu.IR << endl;
            cout << "| MBR:   " << cpu.MBR << endl;
            cout << "| PC:    " << (word)cpu.PC << endl;
            cout << "| MAR:   " << (word)cpu.MAR << endl;
            cout << "|----------------------------------" << endl;
        }

        if(memd){

            cout << "| Memory dump:" << endl;
            cout << "|----------------------------------" << endl;
            
            for (size_t i = 0; i < 16; i++){
                cout << "|";
                size_t f = i * 16;
                for (size_t j = i * 16; j < f + 16; j++){
                    cout << setw(3) << memory[j] << " ";
                }
                cout << endl;
            }
            cout << "|----------------------------------" << endl;
        }

    }
};

int main(int argc, char **argv) {

    if(argc != 2){
        fprintf(stderr, "Use:\n%s prog.bin\n", argv[0]);
        return 1;
    }

    Emulator emu;

    emu.load(argv[1]);

    emu.dbg();

    emu.run();

    emu.status(true, true);
    
    return 0;
}
