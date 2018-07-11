#include "operator.h"
#include "memory.h"
#include <fstream>
using namespace yyh;
namespace yyh
{
	std::string empty = "";
}
int main(int argc, char *argv[])
{
	//freopen("aa.out", "w", stdout);
	token::initialize_keyword();
	reg_num_init();
	std::ifstream text(argv[1]);
	read_in(text);
	run_();
	text.close();
}
