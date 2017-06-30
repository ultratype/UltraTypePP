#include <iostream>
#include "NTClient.h"

int main(int argc, char** argv) {
	NTClient nclient = NTClient("asd4wderg", "123asd123"); // Throw-away account
	cout << "Logging account in...\n";
	nclient.login();
	return 0;
}