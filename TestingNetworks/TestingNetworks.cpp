#include "NetCode.h"

int main()
{
	std::cout << "Server (1) or Client (2): ";
	int opt = 0;

	do 
	{ 
		std::cin >> opt; 
	} while (!(opt == 1 || opt == 2));

	opt == 1 ? NET::ServerRun() : NET::ClientRun();
}