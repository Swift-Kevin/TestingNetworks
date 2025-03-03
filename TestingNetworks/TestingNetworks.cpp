#include "NetCode.h"

int main()
{
	std::cout << "Server (1) or Client (2): ";
	
	int opt = 0;
	std::string buf = "";

	do 
	{ 
		std::getline(std::cin, buf, '\n');
		std::cin.clear();
		
		opt = (int)buf[0] - '0';

	} while (!(opt == 1 || opt == 2));

	opt == 1 ? NET::ServerRun() : NET::ClientRun();
}


