#pragma once
#ifndef scanner_h
#define scanner_h
/*
 * The following are classes and functions about pre_operation
 */
#include "memory.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstring>
namespace yyh
{
	class token
	{
	public:
		static 	std::map<std::string, int> Key_word;
		/* initialize Key_word map */
		static void initialize_keyword();

		/* creat a new token */
		token(std::string& s);

		token();

		enum tokenType { Register, Label, Number, String, Pointer, Command};

		/*
		 * get the type of the token
		 * drop unwanted codes (like ","":" "" )
		 * if the type is string, there's no '\0' in the end
		 */
		void getType();

		/* store the token */
		std::string store;

		tokenType type;
	};
	extern int pc_tmp;
	/*
	 * this is a class dealing with every sentence
	 */
	class scanner
	{
	public:
		/*
		 * creat a new scanner and link it with string
		 * the string was divided by '\n'
		 */
		scanner(const std::string& instring);

		scanner();
		/*
		 * return the next token(string with no white space) in sout
		 * skip the white space
		 * return false if the file gets end
		 * return true in other situation
		 */
		bool nextToken(token& tout);

		std::string store;
	};

	int string_to_int(const std::string& s);
	int string_to_pointer(const std::string& s);

	class pointer
	{
	public:
		int reg_number;
		int pass;
		pointer();
		pointer(const std::string& s);
	};
	
	class operation
	{
	public:
		static std::vector<operation> text;
		static std::map<std::string, int>text_label;
		static std::vector<std::string> tmp_tl;
		static std::vector<std::string> tmp_ll;
		static void push_command(const std::string& com, scanner& sen);
		static void push_label(const std::string& lab);
	public:
		std::string context;
		int reg1, reg2;//the operated two registers
		int number;
		int dest;//the number of the dest register
		int type;
		int anser;//lo
		int anser2;//hi
		int opp1, opp2;//the fetched number
		int history;//for branch jump orders
		operation();
		operation(const std::string& com, scanner& sen);
		void execute();
		bool ID();
		void EXE();
		void MEM();
		void WB();
	};

	/*
	 * read in the text, store program and prepare the space
	 */
	void read_in(std::ifstream& in);

	void run_(); 
}
#endif // !pre_operation_h
