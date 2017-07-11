#ifndef NTLOGGER_H
#define NTLOGGER_H
#define LOG_HTTP (unsigned char)1
#define LOG_RACE (unsigned char)2
#define LOG_INFO (unsigned char)3
#define LOG_CONN (unsigned char)4
#define lln 	 (unsigned char)5

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "colors.h"
using namespace std;

class NTLogger {
public:
	NTLogger(string uname) {
		closed = false;
		hasFile = false;
		username = uname;
	}
	void setUsername(string newUname) {
		username = newUname;
	}
	void close() {
		if (closed == false && hasFile == true) {
			closed = true;
		}
	}
	void operator<<(string msg) {
		writeTxt(msg);
	}
	void operator<<(int msgi) {
		cout << STYLE_BOLD << CLR_RED << msgi << CLR_RESET;
	}
	void operator<<(double msgi) {
		cout << STYLE_BOLD << CLR_RED << msgi << CLR_RESET;
	}
	void operator<<(unsigned char type) { // Unsigned char type to allow integers to be written to the stream
		writeType(type);
	}
	void type(unsigned char type) {
		writeType(type);
	}
	void ln() {
		writeLine();
	}
	void wr(string msg) {
		writeTxt(msg);
	}
	void wrs(string msg) {
		cout << CLR_WHT << msg << CLR_RESET;
	}
protected:
	string fname;
	bool closed;
	bool hasFile;
	string username;
	void writeTxt(string msg) {
		cout << STYLE_BOLD << STYLE_UNDERLN << CLR_YEL << username << ":" << CLR_RESET
		<< " " << CLR_WHT << msg << CLR_RESET;
	}
	void writeLine() {
		cout << '\n';
	}
	void writeType(unsigned char type) { // Unsigned char type to allow integers to be written to the stream
		if (type == lln) {
			writeLine();
			return;
		}
		string out;
		string color;
		switch (type) {
			case LOG_HTTP:
				color = CLR_MAG;
				out += "[HTTP] ";
			break;
			case LOG_RACE:
				color = CLR_RED;
				out += "[RACE] ";
			break;
			case LOG_INFO:
				color = CLR_GRN;
				out += "[INFO] ";
				break;
			case LOG_CONN:
				color = CLR_BLU;
				out += "[CONN] ";
				break;
			default:
				cout << "Ignoring invalid log type!\n";
				break;
		}
		cout << STYLE_ITALIC << color << out << CLR_RESET;
	}
};

#endif