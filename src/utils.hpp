#ifndef UTILS
#define UTILS

#include <map>
#include <vector>
#include <string>
#include <iostream>

/** print num white spaces on the stream */
void print_ws(std::ostream& os, int num){
	for (int i = 0; i < num; i++){
		os.put(' ');
	}
}

#endif
