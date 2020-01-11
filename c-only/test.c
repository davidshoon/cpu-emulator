/*

	C version of the cpu-emulator.

	It runs abit slower than the C++ one, possibly cause the opcode functions aren't inlined here.

	by David Shoon
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// Should we print debug messages?
//#define PRINT(...) printf(__VA_ARGS__)
#define PRINT(...)

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

	OP_CALL,
	OP_RET,

	OP_NOP,
	OP_HLT,

	OP_LASTOPCODE
};

#define OPCODESIZE sizeof(int32_t)
#define CODESIZE 10*1024
#define STACKSIZE 10*1024

#define STACK_PUSH_BACK(x) cpu->stack[cpu->sp] = x; cpu->sp++
#define CODE_PUSH_BACK(x) cpu->code[cpu->pc] = x; cpu->pc++

struct Cpu
{
	int32_t *code;
	int32_t *stack;

	int sp; // stack pointer
	int pc; // program counter / instruction pointer.
};

void op_push(struct Cpu *cpu)
{
	STACK_PUSH_BACK(cpu->code[cpu->pc + 1]);
	cpu->pc += 2;
}

int32_t silent_pop(struct Cpu *cpu)
{
	int32_t x = cpu->stack[cpu->sp-1];
	cpu->sp--;

	return x;
}

int32_t op_pop(struct Cpu *cpu)
{
	cpu->pc++;

	return silent_pop(cpu);
}

void op_add(struct Cpu *cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x + y;

	STACK_PUSH_BACK(z);
	cpu->pc++;
}

void op_sub(struct Cpu *cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x - y;

	STACK_PUSH_BACK(z);
	cpu->pc++;
}

void op_mul(struct Cpu *cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x * y;

	STACK_PUSH_BACK(z);
	cpu->pc++;
}

void op_div(struct Cpu *cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (y == 0) {
		PRINT("ERROR: Divide by zero!\n");
		cpu->pc++;
		return;
	}

	int32_t z = x / y;

	STACK_PUSH_BACK(z);
	cpu->pc++;
}

void op_mod(struct Cpu *cpu)
{
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);
	int32_t z = x % y;

	STACK_PUSH_BACK(z);
	cpu->pc++;
}

void op_jmp(struct Cpu *cpu)
{
	int32_t x = silent_pop(cpu);
	cpu->pc = x;
}

void op_cmpjmpe(struct Cpu *cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x == y) {
		cpu->pc = newpc;
	}
	else {
		cpu->pc++;
	}
}

void op_cmpjmpgt(struct Cpu *cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x > y) {
		cpu->pc = newpc;
	}
	else {
		cpu->pc++;
	}
}

void op_cmpjmpgte(struct Cpu *cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x >= y) {
		cpu->pc = newpc;
	}
	else {
		cpu->pc++;
	}
}

void op_cmpjmplt(struct Cpu *cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x < y) {
		cpu->pc = newpc;
	}
	else {
		cpu->pc++;
	}
}

void op_cmpjmplte(struct Cpu *cpu)
{
	int32_t newpc = silent_pop(cpu);
	int32_t y = silent_pop(cpu);
	int32_t x = silent_pop(cpu);

	if (x <= y) {
		cpu->pc = newpc;
	}
	else {
		cpu->pc++;
	}
}

void op_call(struct Cpu *cpu)
{
	int32_t newpc = silent_pop(cpu);
	STACK_PUSH_BACK(cpu->pc + 1);
	cpu->pc = newpc;
}

void op_ret(struct Cpu *cpu)
{
	int32_t newpc = silent_pop(cpu);
	cpu->pc = newpc;
}

void op_nop(struct Cpu *cpu)
{
	cpu->pc++;
}

void op_hlt(struct Cpu *cpu)
{
	exit(0);
}

void opcode_parse(struct Cpu *cpu)
{
	PRINT("CPU: pc = %d, sp = %d\n", cpu->pc, cpu->sp);

	switch (cpu->code[cpu->pc]) {
		case OP_PUSH:
			PRINT("PUSH\n");
			op_push(cpu);
			break;

		case OP_POP: {
			int32_t x = op_pop(cpu);
			PRINT("POP: %d\n", x);
			break;
		}

		case OP_ADD:
			PRINT("ADD\n");
			op_add(cpu);
			break;

		case OP_SUB:
			PRINT("SUB\n");
			op_sub(cpu);
			break;

		case OP_MULT:
			PRINT("MUL\n");
			op_mul(cpu);
			break;

		case OP_DIV:
			PRINT("DIV\n");
			op_div(cpu);
			break;

		case OP_MOD:
			PRINT("MOD\n");
			op_mod(cpu);
			break;

		case OP_JMP:
			PRINT("JMP\n");
			op_jmp(cpu);
			break;

		case OP_CMPJMPE:
			PRINT("CMPJMPE\n");
			op_cmpjmpe(cpu);
			break;

		case OP_CMPJMPGT:
			PRINT("CMPJMPGT\n");
			op_cmpjmpgt(cpu);
			break;

		case OP_CMPJMPGTE:
			PRINT("CMPJMPGTE\n");
			op_cmpjmpgte(cpu);
			break;

		case OP_CMPJMPLT:
			PRINT("CMPJMPLT\n");
			op_cmpjmplt(cpu);
			break;

		case OP_CMPJMPLTE:
			PRINT("CMPJMPLTE\n");
			op_cmpjmplte(cpu);
			break;

		case OP_CALL:
			PRINT("CALL\n");
			op_call(cpu);
			break;

		case OP_RET:
			PRINT("RET\n");
			op_ret(cpu);
			break;

		case OP_NOP:
			PRINT("NOP\n");
			op_nop(cpu);
			break;

		case OP_HLT:
			PRINT("HLT\n");
			op_hlt(cpu);
			break;

		default:
			PRINT("ERROR: Opcode invalid: %d\n", cpu->code[cpu->pc]);
			exit(1);
	}
}

void init_cpu(struct Cpu *cpu)
{
	printf("Allocating code: %d x %lu\n", CODESIZE, OPCODESIZE);
	printf("Allocating stack: %d x %lu\n", STACKSIZE, OPCODESIZE);
	cpu->code = calloc(CODESIZE, OPCODESIZE);
	cpu->stack = calloc(STACKSIZE, OPCODESIZE);
	cpu->pc = 0;
	cpu->sp = 0;
}

int main()
{
	struct Cpu cpu1;
	struct Cpu *cpu = &cpu1;

	init_cpu(cpu);

	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(6);
	CODE_PUSH_BACK(OP_CALL);

	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(13);
	CODE_PUSH_BACK(OP_JMP);

	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(0x01);
	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(0x02);

	CODE_PUSH_BACK(OP_ADD);
	CODE_PUSH_BACK(OP_POP);

	CODE_PUSH_BACK(OP_RET);

	CODE_PUSH_BACK(OP_NOP);

	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(0x01);
	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(0x01);

	CODE_PUSH_BACK(OP_ADD); //add, result in stack.

	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(0x02); // compare to 0x02

	CODE_PUSH_BACK(OP_PUSH);
	CODE_PUSH_BACK(0x00); // jmp to this pc if equal

	CODE_PUSH_BACK(OP_CMPJMPE);

	CODE_PUSH_BACK(OP_HLT);

	cpu->pc = 0;
	cpu->sp = 0;

	for (int i = 0; i < 1000000000; i++)
		opcode_parse(cpu);

}
