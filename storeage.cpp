#include "storeage.h"

void yyh::Registers::map_init()
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
}

yyh::Registers::Registers()
{
	map_init();
	for (int i = 0; i < 32; i++)
		store[i] = 0;
	store[29] = 0xfff;//the top of the stack (wrong!)
}

int yyh::Registers::operator[](const std::string & s)
{
	int idx = reg_num[s];
	return store[idx];
}

void yyh::Memory::insert_label(std::string & s)
{
}
