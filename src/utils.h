#ifndef __UTILS_H
#define __UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <uWS/uWS.h>
#include <map>
#include <algorithm>
#include <atomic>
#include <thread>
#include <functional>
#include <future>
#include <random>
#include <cstdlib>
#include <ctime>

class Utils {
public:
	static int randInt(int min, int max) {
		return rand() % (max - min + 1) + min;
	}
	static bool randBool(double probFalse) {
		std::knuth_b reng;
		std::uniform_real_distribution<> uniform_0to1(0.0, 1.0);
	    return uniform_0to1(reng) < probFalse;
	}
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	    std::stringstream ss(s);
	    std::string item;
	    while (std::getline(ss, item, delim)) {
	        elems.push_back(item);
	    }
	    return elems;
	}
	static std::vector<std::string> split(const std::string &s, char delim) {
	    std::vector<std::string> elems;
	    split(s, delim, elems);
	    return elems;
	}
	template <class F, class... Args>
	static void setInterval(std::atomic_bool& cancelToken,size_t interval,F&& f, Args&&... args){
	  cancelToken.store(true);
	  auto cb = std::bind(std::forward<F>(f),std::forward<Args>(args)...);
	  std::async(std::launch::async,[=,&cancelToken]()mutable{
	     while (cancelToken.load()){
	        cb();
	        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	     }
	  });
	}
	static std::string extractCValue(std::string cookie) {
		std::vector<std::string> parts = Utils::split(cookie, '=');
		std::string part1 = parts.at(1);
		std::vector<std::string> parts2 = Utils::split(part1, ';');
		return std::string(parts2.at(0));
	}
	static std::string extractCKey(std::string cookie) {
		std::vector<std::string> parts = Utils::split(cookie, '=');
		return std::string(parts.at(0));
	}
	static std::string stringifyCookies(std::vector<std::pair<std::string, std::string>>* cookies) {
		std::string ret("");
		for (int i = 0; i < cookies->size(); ++i) {
			std::pair<std::string, std::string> c = cookies->at(i);
			ret += c.first;
			ret += "=";
			ret += c.second;
			if (i != cookies->size() - 1) {
				ret += "; ";
			}
		}
		return ret;
	}
};

#endif