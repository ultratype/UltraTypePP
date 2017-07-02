#include <iostream>
#include "NTClient.h"

int main(int argc, char** argv) {
	NTClient nclient = NTClient(); // Throw-away account
	cout << "Logging account in...\n";
	nclient.login("gfsiosd", "123asd123");
	nclient.connect();
	return 0;
}