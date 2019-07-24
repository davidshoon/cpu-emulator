#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <vector>
#include <stack>

enum {
	OP_NULL = 0,
	OP_PUSH,
	OP_POP,
	OP_ADD,
	OP_SUB,
	OP_MULT,
	OP_DIV,
	OP_MOD,
	OP_NOP,
	OP_HLT,
	OP_LASTOPCODE
};

const int OPCODESIZE = sizeof(int32_t);

class Cpu
{
public:
	Cpu(int size_of_heap) : pc(0) {
		heap.resize(size_of_heap);
	}

	// This cpu has separate code, stack, heap memory areas.
	std::vector <int32_t> code;
	std::vector <int32_t> stack;
	std::vector <int8_t> heap;

	int pc; // program counter / instruction pointer.
};

inline void op_push(Cpu &cpu)
{
	cpu.stack.push_back(cpu.code[cpu.pc + 1]);
	cpu.pc += 2;
}

inline int32_t op_pop(Cpu &cpu)
{
	int32_t x = cpu.stack.back();
	cpu.stack.pop_back();
	cpu.pc++;

	return x;
}

inline int32_t silent_pop(Cpu &cpu)
{
	int32_t x = cpu.stack.back();
	cpu.stack.pop_back();

	return x;
}

inline void op_add(Cpu &cpu)
{
	int32_t x = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t z = x + y;

	cpu.stack.push_back(z);
	cpu.pc++;
}

void opcode_parse(Cpu &cpu)
{
	printf("CPU: pc = %d, sp = %d\n", cpu.pc, cpu.stack.end() - cpu.stack.begin());

	switch (cpu.code[cpu.pc]) {
		case OP_PUSH:
			printf("PUSH\n");
			op_push(cpu);
			break;

		case OP_POP: {
			int32_t x = op_pop(cpu);
			printf("POP: %d\n", x);
			break;
		}

		case OP_ADD:
			printf("ADD\n");
			op_add(cpu);
			break;

/*
		case OP_SUB:
			op_sub(cpu):
			break;

		case OP_MULT:
			op_mult(cpu);
			break;

		case OP_DIV:
			op_div(cpu);
			break;

		case OP_MOD:
			op_mod(cpu);
			break;

*/
		case OP_HLT:
			exit(0);
			break;

		default:
			printf("ERR: Opcode invalid: %d\n", cpu.code[cpu.pc]);
			exit(1);
	}
}

int main()
{
	Cpu cpu(1024*1024);

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x01);

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x02);

	cpu.code.push_back(OP_ADD);
	cpu.code.push_back(OP_POP);
	cpu.code.push_back(OP_HLT);

	while (1)
		opcode_parse(cpu);

}
