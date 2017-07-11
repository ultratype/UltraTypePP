#include <thread>
#include <vector>
#include <string>

#pragma once
using namespace std;

class Account {
public:
	int wpm;
	string name;
	string pass;
	double acc;
	Account(string n, string p, int w, double a) {
		wpm = w;
		name = n;
		pass = p;
		acc = a;
	}
};