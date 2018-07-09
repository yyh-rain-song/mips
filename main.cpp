#include "pre_operator.h"
#include "memory.h"
#include <fstream>
using namespace yyh;
namespace yyh
{
	std::string empty = "";
	std::ofstream out("a.out");
	std::ifstream in("a.in");
}
int main()
{
	token::initialize_keyword();
	reg_num_init();
	std::ifstream text("1.s");
	read_in(text);
	run_();
	std::cout << "over";
}
