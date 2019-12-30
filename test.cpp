#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <vector>
#include <stack>

// Should we print debug messages?
//#define PRINT(...) printf(__VA_ARGS__)
#define PRINT(...)

// Use "FastVector" -- which is faster than STL vector.
#define USE_FASTVECTOR

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

const int OPCODESIZE = sizeof(int32_t);

#ifdef USE_FASTVECTOR

template <class T>
class MyFastVector
{
public:
	void setSize(int size) {
		vec = new T[size];
		last_index = 0;
	}

	T &operator[](int index) {
		return vec[index];
	}

	void push_back(T data) {
		vec[last_index++] = data;
	}

	void pop_back() {
		last_index--;
	}

	T &back() {
		return vec[last_index-1];
	}

	long int end() {
		return last_index;
	}

	long int begin() {
		return 0;
	}

private:
	T *vec{nullptr};
	int last_index{0};
};

#endif

class Cpu
{
public:
	Cpu(int size_of_heap) : pc(0) {
		heap.resize(size_of_heap);

#ifdef USE_FASTVECTOR
		code.setSize(size_of_heap);
		stack.setSize(size_of_heap);
#endif
	}

	// This cpu has separate code, stack, heap memory areas.
#ifndef USE_FASTVECTOR
	std::vector <int32_t> code;
	std::vector <int32_t> stack;
#else
	MyFastVector <int32_t> code;
	MyFastVector <int32_t> stack;
#endif
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
		PRINT("ERROR: Divide by zero!\n");
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

inline void op_call(Cpu &cpu)
{
	int32_t newpc = silent_pop(cpu);
	cpu.stack.push_back(cpu.pc + 1);
	cpu.pc = newpc;
}

inline void op_ret(Cpu &cpu)
{
	int32_t newpc = silent_pop(cpu);
	cpu.pc = newpc;
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
	PRINT("CPU: pc = %d, sp = %ld\n", cpu.pc, cpu.stack.end() - cpu.stack.begin());

	switch (cpu.code[cpu.pc]) {
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
			PRINT("ERROR: Opcode invalid: %d\n", cpu.code[cpu.pc]);
			exit(1);
	}
}

int main()
{
	Cpu cpu(1024*1024);

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(6);
	cpu.code.push_back(OP_CALL);

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(13);
	cpu.code.push_back(OP_JMP);

	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x01);
	cpu.code.push_back(OP_PUSH);
	cpu.code.push_back(0x02);

	cpu.code.push_back(OP_ADD);
	cpu.code.push_back(OP_POP);

	cpu.code.push_back(OP_RET);

	cpu.code.push_back(OP_NOP);

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

//	while (1)
	for (int i = 0; i < 1000000000; i++)
		opcode_parse(cpu);

}
