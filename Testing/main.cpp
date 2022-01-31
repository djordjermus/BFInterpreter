#include <iostream>
#include"../BFInterpreter/BFInterpreter.h"
int main(int argc, char* argv[]) {
	BFInterpreter parser(1, 1);
	const char data[] =
		"INPUT:       ";

	const char text[] =
		">>>>>>>"
		",>,>,>,>,>"
		"<<<<<"
		"<<<<<<<"
		".>.>.>.>.>.>.>.>.>.>.>.";
	parser.SetData(0, data, sizeof(data) + 1);
	parser.SetText(0, text, sizeof(text) + 1);
	auto x = parser.Run();



}