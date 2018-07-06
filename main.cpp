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
	run_();
	std::cout << "over";
}
