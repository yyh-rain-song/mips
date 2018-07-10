#include "pre_operator.h"
using namespace yyh;


std::map<std::string, int> token::Key_word;
std::vector<operation> operation::text;
std::map<std::string, int> operation::text_label;
std::vector<std::string> operation::tmp_tl;
std::vector<std::string> operation::tmp_ll;

scanner::scanner(const std::string & instring)
{
	store = instring;
	//get away the #note
	std::string::iterator it = store.begin();
	while (it != store.end() && *it != '#') it++;
	if (it != store.end()) store.erase(it, store.end());
}

yyh::scanner::scanner()
{
}

bool scanner::nextToken(token & tout)//if continuous "      "??
{
	tout.store = "";
	if (store == "") return false;
	//special judge for "string with white space"

	std::string::iterator it = store.begin();
	while (it != store.end() && (*it == ' ' || *it == '\t')) it++;
	//delete white space
	store.erase(store.begin(), it);
	it = store.begin();
	if (it == store.end()) return false;

	if (*it == '\"')
	{
		tout.store += (*it);
		it++;
		while (it != store.end())
		{
			if (*it == '\"' && *(it - 1) != '\\') break;
			tout.store += (*it);
			it++;
		}
		tout.store += (*it);
		store.erase(store.begin(), it + 1);
	}
	else
	{
		std::istringstream in(store);
		in >> tout.store;

		//delete the read token in scanner(without ' ')
		std::string::iterator it = store.begin();
		while (it != store.end() && *it != ' ') it++;
		if (it != store.end()) it++;
		store.erase(store.begin(), it);

		//delete the front ' '
		it = store.begin();
		while (it != store.end() && *it == ' ')
			store.erase(it);
	}
	if (tout.store == "") return false;
	//deal with token
	tout.getType();
	return true;
}

void yyh::token::initialize_keyword()
{
	std::string KEY[60] = { ".align", ".ascii", ".asciiz", ".byte", ".half", ".word", ".space", ".data", ".text"
		, "add", "addu", "addiu", "sub", "subu", "mul", "mulu", "div", "divu", "xor", "xoru", "neg", "negu", "rem"
		,"remu", "li", "seq", "sge", "sgt", "sle", "slt", "sne", "b", "beq", "bne", "bge", "ble", "bgt", "blt", "beqz"
		, "bnez", "blez", "bgez", "bgtz", "bltz", "j", "jr", "jal", "jalr", "la", "lb", "lh", "lw", "sb", "sh", "sw"
		, "move", "mfhi", "mflo", "nop", "syscall"
	};
	for (int i = 0; i < 60; i++)
	{
		Key_word[KEY[i]] = i;
	}
}

token::token(std::string & s)
{
	store = s;
}

yyh::token::token()
{
	store = "";
}

void token::getType()
{
	std::string::iterator it = store.end() - 1;
	if (*it == ',') store.erase(it);
	if (store[0] == '\"')//string
	{
		type = String;
		//deal with '\n' '\t'
		std::string::iterator it = store.begin();
		store.erase(it);
		store.erase(store.end() - 1);
		while (it != store.end())
		{
			if (*it != '\\')
			{
				it++;
				continue;
			}
			else //deal with special cases
			{
				store.erase(it);
				if (*it == '\\')
				{
				}
				else if (*it == 'n')
					*it = '\n';
				else if (*it == 't')
					*it = '\t';
				else if (*it == '\'')
					*it = '\'';
				else if (*it == '\"')
					*it = '\"';
				else if (*it == '0')
					*it = '\0';
			}
		}
		return;
	}
	if (store[0] == '$')//register
	{
		type = Register;
		return;
	}
	if (*(store.end() - 1) == ':')//label
	{
		type = Label;
		store.erase(store.end() - 1);
		return;
	}
	//command
	if (Key_word.count(store))
	{
		type = Command;
		return;
	}
	bool flag = false;
	for (int i = 0; i < store.length(); i++)
	{
		if (i == 0 && store[0] == '-')
			continue;
		if (store[i] < '0' || store[i] > '9')
		{
			flag = true;
			break;
		}
	}
	if (flag)//pointer or label:search for '('
	{
		for (int i = 0; i < store.length(); i++)
		{
			if (store[i] == '(')
			{
				type = Pointer;
				return;
			}
		}
		type = Label;
	}
	else type = Number;
}

int yyh::string_to_int(const std::string & s)
{
	int p = 0;
	bool neg = false;
	int pass = 0;
	if (s[p] == '-')
	{
		neg = true;
		p++;
	}
	while (p < s.length())
	{
		pass = pass * 10 + s[p] - '0';
		p++;
	}
	if (neg) pass *= -1;
	return pass;
}

int yyh::string_to_pointer(const std::string & s)
{
	int p = 0;
	bool neg = false;
	int pass = 0;
	if (s[p] == '-')
	{
		neg = true;
		p++;
	}
	while (s[p] != '(')
	{
		pass = pass * 10 + s[p] - '0';
		p++;
	}
	if (neg) pass *= -1;
	p++;
	//find the register
	std::string reg = s.substr(p, s.length() - p - 1);
	int pointer = at_register(reg);
	pointer += pass;
	return pointer;
}

void yyh::read_in(std::ifstream & in)
{
	std::string str;
	scanner sentence;
	token take;
	bool data_state = true;
	int i = 0;
	while (std::getline(in, str))
	{
		sentence = scanner(str);
		if (!sentence.nextToken(take)) continue;
		if (take.type == token::Command && take.store == ".data")
		{
			data_state = true;
			continue;
		}
		if (take.type == token::Command && take.store == ".text")
		{
			data_state = false;
			continue;
		}
		if (take.type == token::Label)
		{
			if (data_state)
				insert_memory_label(take.store);
			else
				operation::push_label(take.store);
		}
		else if (take.type == token::Command)
		{
			if (data_state)
			{
				if (take.store == ".space")
				{
					sentence.nextToken(take);
					int n = string_to_int(take.store);
					_space(n);
				}
				else if (take.store == ".byte")
				{
					while (sentence.nextToken(take))
					{
						char c = (char)string_to_int(take.store);
						_byte(c);
					}
				}
				else if (take.store == ".half")
				{
					while (sentence.nextToken(take))
					{
						int n = string_to_int(take.store);
						_half((short)(n));
					}
				}
				else if (take.store == ".word")
				{
					while (sentence.nextToken(take))
					{
						int n = string_to_int(take.store);
						_word(n);
					}
				}
				else if (take.store == ".ascii")
				{
					sentence.nextToken(take);
					_ascii(take.store);
				}
				else if (take.store == ".asciiz")
				{
					sentence.nextToken(take);
					_asciiz(take.store);
				}
				else if (take.store == ".align")
				{
					sentence.nextToken(take);
					int n = string_to_int(take.store);
					_align(n);
				}
			}
			else
			{
				std::string cmd = take.store;
				operation::push_command(cmd, sentence);
			}
		}
	}
	//deal with labels (stored in context)
	for(int i = 0; i < operation::text.size(); i++)
	{
		operation *tt = &operation::text[i];
		//memory label
		if (tt->type >= 48 && tt->type <= 54 && tt->reg1 == -1)
		{
			token tmp;
			scanner sen(tt->context);
			sen.nextToken(tmp);
			std::string lab = tmp.store;
			tt->number = Labels[lab];
		}
		//text label
		else if ((tt->type >= 31 && tt->type <= 44) || (tt->type == 46))
		{
			token tmp;
			scanner sen(tt->context);
			sen.nextToken(tmp);
			std::string lab = tmp.store;
			tt->dest = operation::text_label[lab];
		}
	}
}

void yyh::run_()
{
	/*
	int current_line = operation::text_label["main"];
	operation* op = NULL;
	Registers[32] = current_line + 1;
	int i = 0;
	while (current_line < operation::text.size())
	{
		op = &operation::text[current_line];
		op->execute();
		if (op->type == 46 || op->type == 47)
			Registers[31] = current_line + 1;
		current_line = Registers[32];
		Registers[32]++;
	}*/

	operation *p2, *p3 = NULL, *p4 = NULL, *p5 = NULL;
	int start = operation::text_label["main"];
	p2 = &operation::text[start];

	while (p2 != NULL || p3 != NULL || p4 != NULL || p5 != NULL)
	{
		if (p5 != NULL) p5->WB();
		p5 = NULL;
		if (p4 != NULL) p4->MEM();
		p5 = p4; p4 = NULL;
		if (p3 != NULL) p3->EXE();
		p4 = p3; p3 = NULL;
		if (p2 != NULL)
		{
			if (p2->ID())
			{
				p3 = p2; p2 = NULL;
				if (Reg_access[32] == 0)
				{
					//IF
					p2 = &operation::text[Registers[32]];
#define debug
#ifdef debug
					std::cout << p2->context << '\n';
#endif // debug
					++Registers[32];
				}
			}
		}
		else
		{
			if (Reg_access[32] == 0)
			{
				//IF
				p2 = &operation::text[Registers[32]];
#ifdef debug
				std::cout << p2->context << '\n';
#endif // debug

				++Registers[32];
			}
		}
	}
}

void yyh::operation::push_command(const std::string & com, scanner & sen)
{
	operation op(com, sen);
	if (op.type == 46 || op.type == 47)
		op.anser = text.size() + 1;
	text.push_back(op);
}

void yyh::operation::push_label(const std::string & lab)
{
	text_label[lab] = text.size();
}

yyh::operation::operation()
{
}

yyh::operation::operation(const std::string & com, scanner & sen)//sen: the remaining sentence
{
	context = com + sen.store;
	type = token::Key_word[com];
	dest = reg1 = reg2 = -1;
	number = 0;
	token tmp;
	switch (type)
	{
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 18:
	case 19:
	case 22:
	case 23:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
		/*
		 * cases that fetch two register and a destination
		 */
		sen.nextToken(tmp);//dest register
		dest = reg_num[tmp.store];
		sen.nextToken(tmp);//register1
		reg1 = reg_num[tmp.store];
		sen.nextToken(tmp);//register2 or immediate number
		if (tmp.type == token::Register)
		{
			reg2 = reg_num[tmp.store];
		}
		else
		{
			reg2 = -1;
			number = string_to_int(tmp.store);
		}
		break;
	case 14:
	case 15:
	case 16:
	case 17:
		/*
		* cases that has two options (mul series)
		*/
		sen.nextToken(tmp);//dest register
		dest = reg_num[tmp.store];

		sen.nextToken(tmp);//register1
		if (tmp.type == token::Register)
			reg1 = reg_num[tmp.store];
		else//reg + num
		{
			reg1 = -1;
			number = string_to_int(tmp.store);
			break;
		}

		if (sen.nextToken(tmp))//have register2 or immediate number)
		{
			if (tmp.type == token::Register)
				reg2 = reg_num[tmp.store];
			else
			{
				reg2 = -1;
				number = string_to_int(tmp.store);
			}
		}
		else//reg reg
		{
			reg2 = reg1;
			reg1 = -1;
		}
		break;
	case 20:
	case 21:
	case 24:
	case 55:
		/*
		* cases that fetch one register and a destination
		*/
		sen.nextToken(tmp);//dest register
		dest = reg_num[tmp.store];
		sen.nextToken(tmp);//register1 or immediate number
		if (tmp.type == token::Register)
		{
			reg1 = reg_num[tmp.store];
		}
		else
		{
			reg1 = -1;
			number = string_to_int(tmp.store);
		}
		break;
	case 31:
	case 44:
	case 46:
		/*
		* cases that fetch a label
		*/
		sen.nextToken(tmp);//label
		context = tmp.store + " " + context;
		break;
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
		/*
		* cases that fetch two register and a label
		*/
		sen.nextToken(tmp);//register1
		reg1 = reg_num[tmp.store];
		sen.nextToken(tmp);//register2 or immediate number
		if (tmp.type == token::Register)
		{
			reg2 = reg_num[tmp.store];
		}
		else
		{
			reg2 = -1;
			number = string_to_int(tmp.store);
		}
		sen.nextToken(tmp);//label
		context = tmp.store + " " + context;
		break;
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
		/*
		* cases that fetch one register and a label
		*/
		sen.nextToken(tmp);//register1
		reg1 = reg_num[tmp.store];
		sen.nextToken(tmp);//label
		context = tmp.store + " " + context;
		break;
	case 45:
	case 47:
	case 56:
	case 57:
		/*
		* cases that fetch one register
		*/
		sen.nextToken(tmp);//dest register
		dest = reg_num[tmp.store];
		break;
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
		/*
		* cases that fetch one register and a address
		*/
		sen.nextToken(tmp);//register1
		dest = reg_num[tmp.store];
		sen.nextToken(tmp);//pointer
		if (tmp.type == token::Pointer)
		{
			pointer po(tmp.store);
			reg1 = po.reg_number;
			number = po.pass;
		}
		else//label
		{
			reg1 = -1;
			context = tmp.store + " " + context;
		}
		break;
	default://nop syscall
		break;
	}
}

void yyh::operation::execute()
{
	/*
	int ob1 = -111, ob2 = -111;
	if ((type >= 9 && type <= 19) || (type >= 22 && type <= 23) || (type >= 32 && type <= 37)
		|| (type >= 25 && type <= 30))//binary operation
	{
		if (reg1 != -1) ob1 = Registers[reg1];
		if (reg2 != -1) ob2 = Registers[reg2];
		else ob2 = number;
		switch (type)
		{
			//case 9-19 22 23 25-30 are logical operatiors,thus we store anser in operation
		case 9:
			anser = ob1 + ob2;
			Registers[dest] = anser;
			break;
		case 10:
		case 11:
			anser = ((unsigned int)(ob1)+(unsigned int)(ob2));
			Registers[dest] = (unsigned int)anser;
			break;
		case 12:
			anser = ob1 - ob2;
			Registers[dest] = anser;
			break;
		case 13:
			anser = ((unsigned int)(ob1)-(unsigned int)(ob2));
			Registers[dest] = (unsigned int)anser;
			break;
		case 14:
			if (reg1 != -1)
			{
				long long anser = (long long)(ob1) * (long long)(ob2);
				Registers[dest] = anser;
			}
			else
			{
				ob1 = Registers[dest];
				if(reg2 != -1)
					ob2 = Registers[reg2];
				else ob2 = number;
				long long tmp = (long long)(ob1) * (long long)(ob2);
				Registers[34] = tmp;
				Registers[33] = tmp >> 32;
			}
			break;
		case 15:
			if (reg1 != -1)
			{
				anser = ((unsigned int)(ob1) * (unsigned int)(ob2));
				Registers[dest] = (unsigned int)(anser);
			}
			else
			{
				ob1 = Registers[dest];
				if (reg2 != -1)
					ob2 = Registers[reg2];
				else ob2 = number;
				unsigned long long tmp = (unsigned long long)(ob1) * (unsigned long long)(ob2);
				Registers[34] = tmp;
				Registers[33] = tmp >> 32;
			}
			break;
		case 16:
			if (reg1 != -1)
			{
				anser = ob1 / ob2;
				Registers[dest] = anser;
			}
			else
			{
				ob1 = Registers[dest];
				if (reg2 != -1)
					ob2 = Registers[reg2];
				else ob2 = number;
				Registers[34] = ob1 / ob2;
				Registers[33] = ob1 % ob2;
			}
			break;
		case 17:
			if (reg1 != -1)
			{
				anser = ((unsigned int)(ob1) / (unsigned int)(ob2));
				Registers[dest] = (unsigned int)(anser);
			}
			else
			{
				ob1 = Registers[dest];
				if (reg2 != -1)
					ob2 = Registers[reg2];
				else ob2 = number;
				Registers[34] = (unsigned int)(ob1) / (unsigned int)(ob2);
				Registers[33] = (unsigned int)(ob1) % (unsigned int)(ob2);
			}
			break;
		case 18:
		case 19:
			anser = ob1 ^ ob2;
			Registers[dest] = anser;
			break;
		case 22:
			anser = ob1 % ob2;
			Registers[dest] = anser;
			break;
		case 23:
			anser = ((unsigned int)(ob1) % (unsigned int)(ob2));
			Registers[dest] = (unsigned int)anser;
			break;
		case 25:
			anser = (ob1 == ob2);
			Registers[dest] = anser;
			break;
		case 26:
			anser = (ob1 >= ob2);
			Registers[dest] = anser;
			break;
		case 27:
			anser = (ob1 > ob2);
			Registers[dest] = anser;
			break;
		case 28:
			anser = (ob1 <= ob2);
			Registers[dest] = anser;
			break;
		case 29:
			anser = (ob1 < ob2);
			Registers[dest] = anser;
			break;
		case 30:
			anser = (ob1 != ob2);
			Registers[dest] = anser;
			break;
			//case 32-37 are b operations, anser stores the outcome of calculation, and label stores in dest
		case 32:
			anser = (ob1 == ob2);
			if (anser)
				Registers[32] = dest;
			break;
		case 33:
			anser = (ob1 != ob2);
			if (anser)
				Registers[32] = dest;
			break;
		case 34:
			anser = (ob1 >= ob2);
			if (anser)
				Registers[32] = dest;
			break;
		case 35:
			anser = (ob1 <= ob2);
			if (anser)
				Registers[32] = dest;
			break;
		case 36:
			anser = (ob1 > ob2);
			if (anser)
				Registers[32] = dest;
			break;
		case 37:
			anser = (ob1 < ob2);
			if (anser)
				Registers[32] = dest;
			break;
		default: break;
		}
	}
	//in the following four cases, there's only one register (or number) involved
	else if (type == 20 || type == 21 || type == 24 || type == 55)
	{
		if (reg1 != -1) ob1 = Registers[reg1];
		else ob1 = number;
		switch (type)
		{
		case 20:
			anser = -1 * ob1;
			Registers[dest] = anser;
			break;
		case 21:
			anser = ~ob1;
			Registers[dest] = anser;
			break;
		case 24://li Register[dest] = (the number stored in operation, ob1)

		case 55:
			anser = ob1;
			Registers[dest] = anser;
			break;
		default:
			break;
		}
	}
	//b j jar
	else if (type == 31 || type == 44 || type == 46)
	{
		ob1 = dest;
		Registers[32] = ob1;
		anser = ob1;
	}
	//load store commands
	else if (type >= 48 && type <= 54)
	{
		if (reg1 == -1)
			ob1 = number;//label
		else ob1 = Registers[reg1] + number;//pointer
		switch (type)
		{
		case 48:
			Registers[dest] = ob1;
			break;
		case 49:
			Registers[dest] = Memory[ob1];
			break;
		case 50:
			Registers[dest] = *(short*)(Memory + ob1);
			break;
		case 51:
			Registers[dest] = *(int*)(Memory + ob1);
			break;
		case 52:
			Memory[ob1] = Registers[dest];
			break;
		case 53:
			*(short*)(Memory + ob1) = Registers[dest];
			break;
		case 54:
			*(int*)(Memory + ob1) = Registers[dest];
			break;
		default:
			break;
		}
	}
	//break and jump command
	else if (type >= 38 && type <= 43)
	{
		ob1 = Registers[reg1];
		switch (type)
		{
		case 38:
			if (ob1 == 0)
				Registers[32] = dest;
			break;
		case 39:
			if (ob1 != 0)
				Registers[32] = dest;
			break;
		case 40:
			if (ob1 <= 0)
				Registers[32] = dest;
			break;
		case 41:
			if (ob1 >= 0)
				Registers[32] = dest;
			break;
		case 42:
			if (ob1 > 0)
				Registers[32] = dest;
			break;
		case 43:
			if (ob1 < 0)
				Registers[32] = dest;
			break;
		default:
			break;
		}
	}
	//jr jar
	else if (type == 45 || type == 47)
	{
		Registers[32] = Registers[dest];
	}
	else if (type == 56)//mfhi
	{
		Registers[dest] = Registers[33];
	}
	else if (type == 57)//mflo
	{
		Registers[dest] = Registers[34];
	}
	else if (type == 59)//syscall
	{
		std::string ss;
		char *p;
		switch (Registers[2])
		{
		case 1:
			std::cout << Registers[4];
			break;
		case 4:
			std::cout << (char*)(Memory + Registers[4]);
			break;
		case 5:
			std::cin >> Registers[2];
			break;
		case 8:
			std::cin >> ss;
			ss += '\0';
			p = Memory + Registers[4];
			strncpy(p, ss.c_str(), ss.length());
			Registers[5] = ss.length();
			break;
		case 9:
			Registers[2] = memory_pos;
			memory_pos += Registers[4];
			break;
		case 10:
			exit(0);
		case 17:
			exit(Registers[4]);
		default:
			break;
		}
	}
	*/
	ID();
	EXE();
	MEM();
	WB();
}
bool yyh::operation::ID()
{
	if ((type >= 9 && type <= 19) || (type >= 22 && type <= 23) || (type >= 32 && type <= 37)
		|| (type >= 25 && type <= 30))
	{
		if (reg1 != -1)
		{
			if (Reg_access[reg1] == 0) opp1 = Registers[reg1];
			else return false;
		}
		if (reg2 != -1)
		{
			if (Reg_access[reg2] == 0) opp2 = Registers[reg2];
			else return false;
		}
		else opp2 = number;
		if (reg1 == -1)
		{
			if (Reg_access[dest] == 0) opp1 = Registers[dest];
			else return false;
		}
	}
	else if (type >= 38 && type <= 43)
	{
		if (Reg_access[reg1] == 0) opp1 = Registers[reg1];
		else return false;
		opp2 = 0;
	}
	else if (type >= 48 && type <= 54)
	{
		if (reg1 == -1) opp1 = -1;
		else if (Reg_access[reg1] == 0) opp1 = Registers[reg1];
		else return false;
		if (Reg_access[dest] == 0) opp2 = Registers[dest];
		else return false;
	}
	else if (type == 20 || type == 21 || type == 24 || type == 55)
	{
		if (reg1 != -1)
		{
			if (Reg_access[reg1] == 0) opp1 = Registers[reg1];
			else return false;
		}
		else opp1 = number;
	}
	else if (type == 56)//mfhi
	{
		if (Reg_access[33] == 0) opp1 = Registers[33];
		else return false;
	}
	else if (type == 57)//mflo
	{
		if (Reg_access[34] == 0) opp1 = Registers[34];
		else return false;
	}
	else if (type == 45 || type == 47)
	{
		if (Reg_access[dest] == 0) opp1 = Registers[dest];
		else return false;
	}
	else if (type == 59)
	{
		if (Reg_access[2] == 0) opp1 = Registers[2];
		else return false;
		switch (opp1)
		{
		case 1:
		case 4:
		case 8:
		case 9:	
		case 17:
			if (Reg_access[4] == 0) opp2 = Registers[4];
			else return false;
			break;
		default:
			break;
		}
		if (opp1 == 1)
			std::cout << opp2;
		if (opp1 == 17)
			exit(opp2);
	}

	if ((type >= 9 && type <= 30) || (type >= 48 && type <= 51) || (type >= 55 && type <= 57))
	{
		if (type >= 14 && type <= 17 && reg1 == -1)
		{
			++Reg_access[34];
			++Reg_access[33];
			return true;
		}
		++Reg_access[dest];
		return true;
	}
	else if (type >= 31 && type <= 45)//break and jump
		++Reg_access[32];
	else if (type == 46 || type == 47)//jar jarl
	{
		++Reg_access[31];
		++Reg_access[32];
	}
	else if (type == 59)
	{
		switch (opp1)
		{
		case 5:
			++Reg_access[2];
			break;
		case 8:
			++Reg_access[5];
			break;
		case 9:
			++Reg_access[2];
			break;
		}
	}
	return true;
}
void yyh::operation::EXE()
{
	long long tmp = 0;
	switch (type)
	{
	case 9:
		anser = opp1 + opp2;
		break;
	case 10:
	case 11:
		anser = ((unsigned int)(opp1) + (unsigned int)(opp2));
		break;
	case 12:
		anser = opp1 - opp2;
		break;
	case 13:
		anser = ((unsigned int)(opp1)-(unsigned int)(opp2));
		break;
	case 14:
		tmp = (long long)(opp1) * (long long)(opp2);
		anser = tmp;
		anser2 = tmp >> 32;
		break;
	case 15:
		tmp = (unsigned long long)(opp1) * (unsigned long long)(opp2);
		anser = tmp;
		anser2 = tmp >> 32;
		break;
	case 16:
		anser = opp1 / opp2;
		anser2 = opp1 % opp2;
		break;
	case 17:
		anser = ((unsigned int)(opp1) / (unsigned int)(opp2));
		anser2 = ((unsigned int)(opp1) % (unsigned int)(opp2));
		break;
	case 18:
	case 19:
		anser = opp1 ^ opp2;
		break;
	case 22:
		anser = opp1 % opp2;
		break;
	case 23:
		anser = ((unsigned int)(opp1) % (unsigned int)(opp2));
		break;
	case 25:
	case 32:
	case 38:
		anser = (opp1 == opp2);
		break;
	case 26:
	case 34:
	case 41:
		anser = (opp1 >= opp2);
		break;
	case 27:
	case 36:
	case 42:
		anser = (opp1 > opp2);
		break;
	case 28:
	case 35:
	case 40:
		anser = (opp1 <= opp2);
		break;
	case 29:
	case 37:
	case 43:
		anser = (opp1 < opp2);
		break;
	case 30:
	case 33:
	case 39:
		anser = (opp1 != opp2);
		break;

		//load store commands
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
		if (reg1 == -1) anser = number;//label
		else anser = opp1 + number;//pointer
		break;

	case 20:
		anser = -1 * opp1;
		break;
	case 21:
		anser = ~opp1;
		break;
	case 24:
	case 55:
	case 56:
	case 57:
		anser = opp1;
		break;
	case 31:
	case 44:
	case 45:
	case 46:
	case 47:
		anser = 1;
		break;
	default:
		break;
	}
}

void yyh::operation::MEM()
{
	switch (type)
	{
	case 48:
		opp1 = anser;
		break;
	case 49:
		opp1 = Memory[anser];
		break;
	case 50:
		opp1 = *(short*)(Memory + anser);
		break;
	case 51:
		opp1 = *(int*)(Memory + anser);
		break;
	case 52:
		Memory[anser] = opp2;
		break;
	case 53:
		*(short*)(Memory + anser) = opp2;
		break;
	case 54:
		*(int*)(Memory + anser) = opp2;
		break;
	default:
		break;
	}
	if (type == 59)
	{
		if (opp1 == 4)
		{
			std::cout << (char*)(Memory + opp2);
		}
		else if (opp1 == 8)
		{
			std::string ss;
			std::cin >> ss;
			ss += '\0';
			char* p = Memory + opp2;
			strncpy(p, ss.c_str(), ss.length());
			anser = ss.length();
		}
	}
}

void yyh::operation::WB()
{
	if ((type >= 9 && type <= 13) || (type >= 18 && type <= 30) || (type >= 55 && type <= 57))
	{
		Registers[dest] = anser;
		--Reg_access[dest];
	}
	else if (type >= 14 && type <= 17)
	{
		if (reg1 != -1)
		{
			Registers[dest] = anser;
			--Reg_access[dest];
		}
		else
		{
			Registers[34] = anser;
			Registers[33] = anser2;
			--Reg_access[34];
			--Reg_access[33];
		}
	}
	else if (type >= 31 && type <= 44)//break and jump
	{
		if (anser)
		{
			Registers[32] = dest;
			--Reg_access[32];
		}
	}
	else if (type >= 48 && type <= 51)
	{
		Registers[dest] = opp1;
		--Reg_access[dest];
	}
	else if (type == 45)
	{
		Registers[32] = opp1;
		--Reg_access[32];
	}
	else if (type == 46)
	{
		Registers[32] = dest;
		--Reg_access[32];
		Registers[31] = anser;
		--Reg_access[31];
	}
	else if (type == 47)
	{
		Registers[32] = opp1;
		--Reg_access[32];
		Registers[31] = anser;
		--Reg_access[31];
	}
	else if (type == 59)
	{
		switch (opp1)
		{
		case 5:
			std::cin >> Registers[2];
			--Reg_access[2];
			break;
		case 8:
			Registers[5] = anser;
			--Reg_access[5];
			break;
		case 9:
			Registers[2] = memory_pos;
			memory_pos += opp2;
			--Reg_access[2];
			break;
		}
	}
	//jal jalr uncomplete
}

yyh::pointer::pointer()
{
	reg_number = -1;
	pass = -1;
}

yyh::pointer::pointer(const std::string & s)
{
	int p = 0;
	bool neg = false;
	pass = 0;
	if (s[p] == '-')
	{
		neg = true;
		p++;
	}
	while (s[p] != '(')
	{
		pass = pass * 10 + s[p] - '0';
		p++;
	}
	if (neg) pass *= -1;
	p++;

	//find the register
	std::string reg = s.substr(p, s.length() - p - 1);
	reg_number = reg_num[reg];
}
