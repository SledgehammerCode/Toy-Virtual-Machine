



#include <string>
#include <iostream>
#include <fstream>


const unsigned int INST_NOOP    = 0x00000000;
const unsigned int INST_BREAK   = 0x00000001;
const unsigned int INST_ADD     = 0x00000002;
// ADD [addr1] [addr2] : [addr1] = [addr1] + [addr2]
const unsigned int INST_SUB     = 0x00000003;
const unsigned int INST_JMP     = 0x00000004;
const unsigned int INST_JZ      = 0x00000005;
const unsigned int INST_JNZ     = 0x00000006;
const unsigned int INST_CMP     = 0x00000007;
// CMP [addr]
const unsigned int INST_CP      = 0x00000008;
const unsigned int INST_INCR    = 0x00000009;
const unsigned int INST_DECR    = 0x0000000A;
//const unsigned int INST_SUB     = 0x00000002;
//const unsigned int INST_SUB     = 0x00000002;


// Note: INST_CMP
// INST_CMP [operand]
// - resets the status register
// - if [operand] is zero then the first bit is set to 1
// - if [operand] is not zero the second bit is set to 1



class virtualmachine
{

    public:

    virtualmachine()
    {
        MEM_SIZE = 1024;
        memory = new char[MEM_SIZE];

        debug = false;
        debug_write = false;
    }

    virtual
    ~virtualmachine()
    {
        delete [] memory;
    }

    // note: to access byte 4, use addr=4
    // 
    int read(const std::size_t addr)
    {
        if((addr < 0) || (addr >= MEM_SIZE))
        {
            std::cerr << __func__ << " error: memory access violation (addr=" << addr << ")" << std::endl;
            throw "memory access violation";
        }
        unsigned int *p = (unsigned int*)&(memory[addr]);
        return *p;
    }

    int write(const std::size_t addr, const unsigned int value)
    {

        if(debug_write)
        {
            std::cout << __func__ << " addr=" << addr << " value=" << value << std::endl;
        }

        if((addr < 0) || (addr >= MEM_SIZE))
        {
            std::cerr << __func__ << " error: memory access violation (addr=" << addr << ")" << std::endl;
            throw "memory access violation";
            return 1;
        }
        unsigned int *p = (unsigned int*)&(memory[addr]);
        *p = value;
        if(debug_write)
        {
            std::cout << *p << std::endl;
        }
        return 0;
    }

    void print()
    {
        std::cout << std::hex;
        
        for(int addr = 0; addr < MEM_SIZE; addr += 4)
        {
            if(addr % 8 == 0)
                std::cout << addr << " :\t";
            std::cout << (unsigned int)memory[addr + 0] << " "
                      << (unsigned int)memory[addr + 1] << " "
                      << (unsigned int)memory[addr + 2] << " "
                      << (unsigned int)memory[addr + 3];
            std::cout << "    ";
            if(addr % 8 == 4) std::cout << "\n";
        }
        std::cout << std::endl;

        std::cout << std::dec;
    }

    void run()
    {
        write(0, 8); // set the program counter to offset 8 (the first instruction)
        write(4, 0); // clear status register

        std::size_t counter = 0;
        for(;; ++ counter)
        {

            if(counter >= 500) break;

            unsigned int progc = read(0);
            unsigned int instruction = read(progc);

            std::cout << "counter=" << counter << " progc=" << progc << " inst=" << instruction << std::endl;

            if(instruction == INST_NOOP)
            {
                // do nothing
                // NOOP
            }
            else if(instruction == INST_BREAK)
            {
                break;
            }
            else if(instruction == INST_ADD)
            {
                progc += 4;
                unsigned int addr_a = read(progc);
                int a = read(addr_a);
                progc += 4;
                unsigned int addr_b = read(progc);
                int b = read(addr_b);
                a = a + b;
                if(debug)
                {
                    std::cout << "ADD [" << std::hex
                              << addr_a << "] ["
                              << addr_b << std::dec
                              << "] a=" << a << " b=" << b << std::endl;
                }
                write(addr_a, a);
                progc += 4;
            }
            else if(instruction == INST_SUB)
            {   
                // SUB [addr1] [addr2]
                // subtracts [addr2] from [addr1] and stores back
                // in the location pointed at by addr1
                progc += 4;
                unsigned int addr_a = read(progc);
                int a = read(addr_a);
                progc += 4;
                unsigned int addr_b = read(progc);
                int b = read(addr_b);
                a = a - b;
                if(debug)
                {
                    std::cout << "SUB [" << std::hex
                              << addr_a << "] ["
                              << addr_b << std::dec
                              << "] a=" << a << " b=" << b << std::endl;
                }
                write(addr_a, a);
                progc += 4;
            }
            else if(instruction == INST_JMP)
            {
                progc += 4;
                unsigned int addr = read(progc);
                if(debug)
                {
                    std::cout << "JMP " << std::hex << addr << std::dec << std::endl;
                }
                //write(0, addr);
                progc = addr;
            }
            else if(instruction == INST_JZ)
            {
                //progc += 4;
                unsigned int s = read(4);
                progc += 4;
                unsigned int addr = read(progc);
                progc += 4;
                if(debug)
                {
                    std::cout << "JZ " << std::hex << addr << " status=" << s << std::dec << std::endl;
                }
                if(s &= 0x00000001)
                {
                    //write(0, addr);
                    progc = addr;
                }
                else
                {
                    // do nothing
                }
            }
            else if(instruction == INST_JNZ)
            {
                //progc += 4;
                unsigned int s = read(4);
                progc += 4;
                unsigned int addr = read(progc);
                progc += 4;
                if(debug)
                {
                    std::cout << "JNZ " << std::hex << addr << " status=" << s << std::dec << std::endl;
                }
                if(s &= 0x00000010)
                {
                    //write(0, addr);
                    progc = addr;
                }
                else
                {
                    // do nothing
                }
            }
            else if(instruction == INST_CMP)
            {
                progc += 4;
                write(4, 0); // clear the status register
                unsigned int addr = read(progc);
                unsigned int a = read(addr);
                unsigned int s = 0x00000000;
                if(a == 0) s |= 0x00000001;
                else if(a != 0) s |= 0x00000010;
                if(debug)
                {
                    std::cout << "CMP [" << std::hex << addr << "] a=" << std::dec << a << " status=" << std::hex << s << std::dec << std::endl;
                }
                write(4, s);
                progc += 4;
            }
            else if(instruction == INST_CP)
            {
                // CP [addr1] [addr2]
                // copies the data pointed to by addr1 into the location
                // pointed at by addr2
                progc += 4;
                unsigned int addr1 = read(progc);
                progc += 4;
                unsigned int addr2 = read(progc);
                int a = read(addr2);
                if(debug)
                {
                    std::cout << "CP [" << std::hex << addr1 << "] [" << addr2 << std::dec << "] a=" << a << std::endl;
                }
                write(addr1, a);
                progc += 4;
            }
            else if(instruction == INST_INCR)
            {
                // INCR [addr]
                progc += 4;
                unsigned int addr = read(progc);
                int a = read(addr);
                if(debug)
                {
                    std::cout << "INCR [" << std::hex << addr << "] a=" << std::dec << a << std::endl;
                }
                ++ a;
                write(addr, a);
                progc += 4;
            }
            else if(instruction == INST_DECR)
            {
                // INCR [addr]
                progc += 4;
                unsigned int addr = read(progc);
                int a = read(addr);
                if(debug)
                {
                    std::cout << "INCR [" << std::hex << addr << "] a=" << std::dec << a << std::endl;
                }
                -- a;
                write(addr, a);
                progc += 4;
            }

            // TODO: change such that the jump instructions use write(0, addr)
            // and all other instructions which need to save the program counter
            // do so in their respective blocks
            // (aka: all blocks are responsible for managing progc)
            write(0, progc);
        }
    }


    char *memory;
    std::size_t MEM_SIZE;
    
    bool debug;
    bool debug_write;

};



int main()
{


    std::string filename = "prog1.asm";
    std::ifstream ifs;

    ifs.open(filename, std::ios::in);
    if(ifs.is_open())
    {

        // assemble

        virtualmachine vm;
        // Note: address 0 is reserved for the program counter
        // Note: address 4 is reserved for the status register

        #if 0
        // text
        // add the numbers 1 and 2
        vm.write(8,     INST_ADD);
        vm.write(12,    0x200);
        vm.write(16,    0x204);
        // data
        vm.write(0x200, 1);
        vm.write(0x204, 2);
        #endif

        // text
        // add all the numbers between 1 and 10
        vm.write(0x208, 0); // initialize the counter (sum)        
        
        vm.write(8, INST_CP); // copy the data in 0x200 to 0x208
        vm.write(12, 0x208); // CP [x208] [x200]
        vm.write(16, 0x200);
        vm.write(20, INST_SUB); // SUB [x208] [x204]
        vm.write(24, 0x208);
        vm.write(28, 0x204);
        vm.write(32, INST_CMP); // CMP [x208]
        vm.write(36, 0x208);
        vm.write(40, INST_JZ);
        vm.write(44, 76); // jump to some terminating memory location
                           // where we will put the instruction NOOP
        vm.write(48, INST_ADD); // add to the sum
        vm.write(52, 0x20C);
        vm.write(56, 0x200);
        vm.write(60, INST_INCR); // increment the loop counter
        vm.write(64, 0x200);
        vm.write(68, INST_JMP); // jump back to the begining of the code
        vm.write(72, 8);

        vm.write(76, INST_BREAK); // finish

        /*
        int sum = 0;
        for(int i = 1; i < 10; ++ i)
        {
            sum += 1
        }
        */

    
        // data
        vm.write(0x200, 1); // start index for the for loop
        vm.write(0x204, 10); // end index for the for loop
        vm.write(0x208, 0); // location to store the subtract result
        vm.write(0x20C, 0); // sum





        vm.print();
        vm.debug = true;
        vm.run();
        vm.debug = false;
        vm.print();
        

    }
    else
    {
        std::cerr << __func__ << " error: could not open file " << filename << std::endl;
    }


    return 0;
}
