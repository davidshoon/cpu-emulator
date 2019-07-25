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

	OP_JMP,
	OP_CMPJMPE,
	OP_CMPJMPGT,
	OP_CMPJMPGTE,
	OP_CMPJMPLT,
	OP_CMPJMPLTE,

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

inline int32_t silent_pop(Cpu &cpu)
{
	int32_t x = cpu.stack.back();
	cpu.stack.pop_back();

	return x;
}

inline int32_t op_pop(Cpu &cpu)
{
	cpu.pc++;

	return silent_pop(cpu);
}

inline void op_add(Cpu &cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x + y;

	cpu.stack.push_back(z);
	cpu.pc++;
}

inline void op_sub(Cpu &cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x - y;

	cpu.stack.push_back(z);
	cpu.pc++;
}

inline void op_mul(Cpu &cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x * y;

	cpu.stack.push_back(z);
	cpu.pc++;
}

inline void op_div(Cpu &cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (y == 0) {
		printf("ERROR: Divide by zero!\n");
		cpu.pc++;
		return;
	}

	int32_t z = x / y;

	cpu.stack.push_back(z);
	cpu.pc++;
}

inline void op_mod(Cpu &cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x % y;

	cpu.stack.push_back(z);
	cpu.pc++;
}

inline void op_jmp(Cpu &cpu)
{
	int32_t x = silent_pop(cpu);
	cpu.pc = x;
}

inline void op_cmpjmpe(Cpu &cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x == y) {
		cpu.pc = newpc;
	}
	else {
		cpu.pc++;
	}
}

inline void op_cmpjmpgt(Cpu &cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x > y) {
		cpu.pc = newpc;
	}
	else {
		cpu.pc++;
	}
}

inline void op_cmpjmpgte(Cpu &cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x >= y) {
		cpu.pc = newpc;
	}
	else {
		cpu.pc++;
	}
}

inline void op_cmpjmplt(Cpu &cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x < y) {
		cpu.pc = newpc;
	}
	else {
		cpu.pc++;
	}
}

inline void op_cmpjmplte(Cpu &cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x <= y) {
		cpu.pc = newpc;
	}
	else {
		cpu.pc++;
	}
}

inline void op_nop(Cpu &cpu)
{
	cpu.pc++;
}

inline void op_hlt(Cpu &cpu)
{
	exit(0);
}

void opcode_parse(Cpu &cpu)
{
	printf("CPU: pc = %d, sp = %ld\n", cpu.pc, cpu.stack.end() - cpu.stack.begin());

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

		case OP_SUB:
			printf("SUB\n");
			op_sub(cpu);
			break;

		case OP_MULT:
			printf("MUL\n");
			op_mul(cpu);
			break;

		case OP_DIV:
			printf("DIV\n");
			op_div(cpu);
			break;

		case OP_MOD:
			printf("MOD\n");
			op_mod(cpu);
			break;

		case OP_JMP:
			printf("JMP\n");
			op_jmp(cpu);
			break;

		case OP_CMPJMPE:
			printf("CMPJMPE\n");
			op_cmpjmpe(cpu);
			break;

		case OP_CMPJMPGT:
			printf("CMPJMPGT\n");
			op_cmpjmpgt(cpu);
			break;

		case OP_CMPJMPGTE:
			printf("CMPJMPGTE\n");
			op_cmpjmpgte(cpu);
			break;

		case OP_CMPJMPLT:
			printf("CMPJMPLT\n");
			op_cmpjmplt(cpu);
			break;

		case OP_CMPJMPLTE:
			printf("CMPJMPLTE\n");
			op_cmpjmplte(cpu);
			break;

		case OP_NOP:
			printf("NOP\n");
			op_nop(cpu);
			break;

		case OP_HLT:
			printf("HLT\n");
			op_hlt(cpu);
			break;

		default:
			printf("ERROR: Opcode invalid: %d\n", cpu.code[cpu.pc]);
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

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x01);
	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x01);

	cpu.code.push_back(OP_ADD); //add, result in stack.

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x02); // compare to 0x02

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x00); // jmp to this pc if equal

	cpu.code.push_back(OP_CMPJMPE);

	cpu.code.push_back(OP_HLT);

	while (1)
		opcode_parse(cpu);

}
