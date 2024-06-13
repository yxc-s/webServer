#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>


class Socket;
//输出自定义错误信息与errno，在连接的处理上可以作为全剧函数使用
void ErrorIf(bool condition, const char* message);



template<typename T>
std::string getHexAddr(T pointer){
	std::stringstream ss;
  	ss << std::hex << reinterpret_cast<uintptr_t>(pointer);
  	std::string address_in_hex = ss.str();
	return address_in_hex;
}

void writeLogHelper(std::ofstream &outFile);

// 递归地处理每个参数
template <typename T, typename... Args>
void writeLogHelper(std::ofstream &outFile, const T& first, const Args&... args) {
    outFile << first; 
    writeLogHelper(outFile, args...);
}

// 用户调用的函数
template <typename... Args>
void writeLog(const Args&... args) {
    std::ofstream outFile("log.txt", std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "无法打开文件进行写入" << std::endl;
        return;
    }
    writeLogHelper(outFile, args...); 
    outFile.close();
}




void logErrorHelper(std::ofstream &outFile);

// 递归地处理每个参数
template <typename T, typename... Args>
void logErrorHelper(std::ofstream &outFile, const T& first, const Args&... args) {
    outFile << first;  // 将当前参数写入文件
    logErrorHelper(outFile, args...);  // 递归调用以处理剩余参数
}

// 用户调用的函数
template <typename... Args>
void logError(const Args&... args) {
    std::ofstream outFile("logError.txt", std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "无法打开文件进行写入" << std::endl;
        return;
    }
    logErrorHelper(outFile, args...);  // 调用辅助函数来处理参数
    outFile.close();
}

