#include "pre_operator.h"
#include "memory.h"
#include <fstream>
using namespace yyh;
namespace yyh
{
	std::string empty = "";
}
int main(int argc, char *argv[])
{
//	freopen("a.out", "w", stdout);
	token::initialize_keyword();
	reg_num_init();
	std::ifstream text("1.s");
	read_in(text);
	run_();
	text.close();
}
