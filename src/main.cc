#include <iostream>
#include "NTClient.h"
using namespace std;

void initBot() {
	NTClient nclient = NTClient(120, 0.98);
	nclient.login("gfsiosd", "123asd123"); // Throw-away account
	nclient.connect();
}
int main(int argc, char** argv) {
	initBot();
	return 0;
}