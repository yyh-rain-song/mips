#include "pre_operator.h"
#include "memory.h"
#include <fstream>
using namespace yyh;
std::string empty = "";
int main()
{
	token::initialize_keyword();
	reg_num_init();

	std::ifstream in("a.in");
	read_in(in);
	std::cout << at_label("label_1") << '\n';
	std::cout << at_label("label_2") << '\n';
	std::cout << at_label("ll") << '\n';
	std::cout << at_label("label_3") << '\n';
	std::cout << at_label("label_4") << '\n';
	std::cout << "over";
}