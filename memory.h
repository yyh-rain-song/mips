#include <map>
#include <string>

namespace yyh
{
	extern std::map<std::string, int> reg_num;
	extern int Registers[33];
	extern int memory_pos;
	extern char Memory[4 * 1024 * 1024];
	extern std::map<std::string, int> Labels;
	void reg_num_init();
	int at_register(const std::string& s);
	void insert_memory_label(const std::string& s);
	int at_label(const std::string& s);
	void _align(int n);
	void _ascii(const std::string &s);
	void _asciiz(const std::string &s);
	void _byte(const char a);
	void _half(short int a);
	void _word(int a);
	void _space(int n);
}
