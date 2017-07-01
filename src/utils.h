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

class Utils {
public:
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
	void setInterval(std::atomic_bool& cancelToken,size_t interval,F&& f, Args&&... args){
	  cancelToken.store(true);
	  auto cb = std::bind(std::forward<F>(f),std::forward<Args>(args)...);
	  std::async(std::launch::async,[=,&cancelToken]()mutable{
	     while (cancelToken.load()){
	        cb();
	        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	     }
	  });
	}
};