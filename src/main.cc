#include <iostream>
#include "NTClient.h"

NTClient cli;
void initBot() {
	NTClient nclient = NTClient();
	cout << "Logging account in...\n";
	nclient.login("gfsiosd", "123asd123"); // Throw-away account
	nclient.connect();
}

int main(int argc, char** argv) {
	initBot();
	return 0;
}