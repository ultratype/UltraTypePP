#include <iostream>
#include <cstdlib>
#include <fstream>
#include "NTClient.h"
#include "colors.h"
#include "json.hpp"
using namespace std;

void initBot(string username, string password, int wpm, double acc) {
	NTClient nclient = NTClient(wpm, acc);
	nclient.login(username, password);
	nclient.connect();
}
void initlog(string msg) {
	cout << CLR_GRN << STYLE_BOLD << "[INIT] " << CLR_RESET << CLR_WHT << msg << CLR_RESET;
}
void errlog(string msg) {
	cout << CLR_RED << STYLE_BOLD << "[ERR!] " << CLR_RESET << CLR_WHT << msg << CLR_RESET;
}
int main(int argc, char** argv) {
	srand(static_cast<unsigned int>(time(0)));
	ifstream configf;
	configf.exceptions(std::ios::failbit | std::ios::badbit);
	try {
		configf.open("config.json");
	} catch(const exception& e) {
		errlog("Failed to open the JSON config. For help, read the UltraType++ repository README.\n");
		return 1;
	}
	return 0;
}