#include <iostream>
#include <cstdlib>
#include "NTClient.h"
using namespace std;

void initBot() {
	NTClient nclient = NTClient(20, 0.89);
	nclient.login("sascf3", "123asd123"); // Throw-away account
	nclient.connect();
}
int main(int argc, char** argv) {
	srand(static_cast<unsigned int>(time(0)));
	initBot();
	return 0;
}