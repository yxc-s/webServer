#include "util.h"
#include "Socket.h"
#include <stdio.h>
#include <cstdlib> // 包含 exit 和 EXIT_FAILURE


void ErrorIf(bool condition, const char *errmsg) {
	if (condition) {
		perror(errmsg);
		exit(EXIT_FAILURE); // 退出程序
	}
}



void writeLogHelper(std::ofstream &outFile){
	outFile << std::endl;
}


void logErrorHelper(std::ofstream &outFile) {
    outFile << std::endl;  // 在每条日志后添加换行符
}




