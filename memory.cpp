#include "memory.h"
namespace yyh
{
	std::map<std::string, int> reg_num;
	int Registers[33];
	char Memory[4 * 1024 * 1024];
	std::map<std::string, int> Labels;
	int memory_pos = 0;
}

void yyh::reg_num_init()
{
	reg_num["$0"] = reg_num["$zero"] = 0;
	reg_num["$1"] = reg_num["$at"] = 1;
	reg_num["$2"] = reg_num["$v0"] = 2;
	reg_num["$3"] = reg_num["$v1"] = 3;
	reg_num["$4"] = reg_num["$a0"] = 4;
	reg_num["$5"] = reg_num["$a1"] = 5;
	reg_num["$6"] = reg_num["$a2"] = 6;
	reg_num["$7"] = reg_num["$a3"] = 7;
	reg_num["$8"] = reg_num["$t0"] = 8;
	reg_num["$9"] = reg_num["$t1"] = 9;
	reg_num["$10"] = reg_num["$t2"] = 10;
	reg_num["$11"] = reg_num["$t3"] = 11;
	reg_num["$12"] = reg_num["$t4"] = 12;
	reg_num["$13"] = reg_num["$t5"] = 13;
	reg_num["$14"] = reg_num["$t6"] = 14;
	reg_num["$15"] = reg_num["$t7"] = 15;
	reg_num["$16"] = reg_num["$s0"] = 16;
	reg_num["$17"] = reg_num["$s1"] = 17;
	reg_num["$18"] = reg_num["$s2"] = 18;
	reg_num["$19"] = reg_num["$s3"] = 19;
	reg_num["$20"] = reg_num["$s4"] = 20;
	reg_num["$21"] = reg_num["$s5"] = 21;
	reg_num["$22"] = reg_num["$s6"] = 22;
	reg_num["$23"] = reg_num["$s7"] = 23;
	reg_num["$24"] = reg_num["$t8"] = 24;
	reg_num["$25"] = reg_num["$t9"] = 25;
	reg_num["$28"] = reg_num["$gp"] = 28;
	reg_num["$29"] = reg_num["$sp"] = 29;
	reg_num["$30"] = reg_num["$fp"] = 30;
	reg_num["$31"] = reg_num["$ra"] = 31;
	reg_num["$hi"] = 26;
	reg_num["$lo"] = 27;
	reg_num["$pc"] = 32;

	for (int i = 0; i < 32; i++)
		Registers[i] = 0;
	Registers[29] = 4 * 1024 * 1024;//the top of the stack
	Registers[30] = 0;//the bottom of the store
}

int yyh::at_register(const std::string & s)
{
	int idx = reg_num[s];
	return Registers[idx];
}

void yyh::insert_memory_label(const std::string & s)
{
	Labels[s] = memory_pos;
}

int yyh::at_label(const std::string & s)
{
	return Labels[s];
}
void yyh::_align(int n)
{
	memory_pos = 1 << n;
}
void yyh::_ascii(const std::string & s)
{
	for (int i = 0; i < s.length(); i++)
		_byte(s[i]);
}

void yyh::_asciiz(const std::string & s)
{
	_ascii(s);
	_byte('\0');
}

void yyh::_byte(const char a)
{
	char* pointer = Memory + memory_pos;
	*pointer = a;
	++memory_pos;
}

void yyh::_half(short int a)
{
	short int* pointer = (short int*)(Memory + memory_pos);
	*pointer = a;
	memory_pos += 2;
}

void yyh::_word(int a)
{
	int* pointer = (int*)(Memory + memory_pos);
	*pointer = a;
	memory_pos += 4;
}

void yyh::_space(int n)
{
	memory_pos += n;
}
