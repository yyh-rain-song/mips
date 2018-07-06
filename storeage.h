#ifndef storeage_h
#define storeage_h
#include <string>
#include <map>
namespace yyh
{
	class Registers
	{
	public:
		int store[33];
		std::map<std::string, int> reg_num;
		void map_init();
		Registers();
		int operator[](const std::string& s);
	};

	class Memory
	{
	public:
		char store[4 * 1024 * 1024];
		std::map<std::string, char*> Labels;
		void insert_label(std::string& s);
	};
}
#endif // !storeage_h

