#include "RequestHandler.h"
#include "ReadBuffer.h"
#include <sstream>
#include "util.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>


const std::unordered_set<std::string> DEFAULT_HTML = {"/index"};
//{"/about", "/blog", "/collection", "/contact", "/services", "/index", "/single"};

const std::string HOME_PAGE = "/index.html";

RequestHandler::RequestHandler():	
	state_(State::LINE),	
	alive_(false),	
	parsed_(false)
{
	buffer_ = std::make_unique<ReadBuffer>(static_cast<size_t>(1024));
}

RequestHandler::~RequestHandler(){
	//delete buffer_;
	//buffer_ = nullptr;
}


size_t RequestHandler::readRequest(int fd){
	buffer_->read(fd);
	return buffer_->getReadableSpace();
}

//先把reset放在解析前吧，后期优化可以考虑放到刚respond完就重
bool RequestHandler::parseRequest(){
	clear();
	if (buffer_->getReadableSpace() <= 0){
		std::cout << "尝试解析，但是缓冲区为空" << std::endl;
		return false;	
	}
	writeLog("开始解析该请求\n");
	while (buffer_->getReadableSpace() && state_ != State::FINISH){
		std::string request = buffer_->getRequest();	
		//下面的语句好像可以去掉，等回头response的时候解析失败返回无法解析到web客户端即可
		//ErrorIf(request.empty(), "可读数据中没有找到C-R-L-F结束符");
		switch (state_){
			case LINE:
				parseLine(request);
				state_ = RequestHandler::State::HEAD;
				break;
			case HEAD:
				if (request == "\r\n"){
					state_ = RequestHandler::State::BODY;
					break;			
				}
				parseHead(request);
				break;
			case BODY:
				parseBody(request);
				state_ = RequestHandler::State::FINISH;
				break;
			default:
				break;	
		}
	}
	writeLog("请求解析完成，准备响应并写入\n");
	return parsed_ = true;
}

bool RequestHandler::isAlive() const{
	return alive_;
}

bool RequestHandler::isParsed() const{
	return parsed_;
}

std::string RequestHandler::getUri() const{
	return uri_;
}

int RequestHandler::getErrno() const{
	return buffer_->getErrno();
}



void RequestHandler::clear(){
	state_ = RequestHandler::State::LINE;
	method_.clear();
	uri_.clear();
	version_.clear();
	headers_.clear();
	body_.clear();
	parsed_ = false;
	alive_ = false;
}




// 可选：URL解码函数（如果需要）
std::string RequestHandler::urlDecode(const std::string& uri) {
	std::string decoded;
	char ch;
	int i, ii;
	for (i = 0; i < uri.length(); i++) {
		if (int(uri[i]) == 37) {
			sscanf(uri.substr(i+1,2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			decoded += ch;
			i = i+2;
		} 
		else {
			decoded += uri[i];
		}
	}
	return decoded;
}


void RequestHandler::parseLine(std::string& request){
	std::istringstream stream(request);
	std::getline(stream, method_, ' ');
	std::getline(stream, uri_, ' ');
	stream >> version_;
	//ErrorIf(method_.empty() || uri_.empty() || version_.empty(), "parseLine with empty value");
	//URI解码
	auto r = uri_;
	//uri_ = urlDecode(uri_);
	if (uri_ == "/"){
		uri_ = HOME_PAGE;//"/home.html";
	}
	else if (DEFAULT_HTML.count(uri_)){
		uri_ += ".html";
	}
}
/*
void RequestHandler::parseHead(std::string& request){
	std::istringstream stream(request);
	std::string line;
	while (std::getline(stream, line) && !line.empty()) {
		std::istringstream line_stream(line);
		std::string header_name;
		std::string header_value;
		std::getline(line_stream, header_name, ':');
		std::getline(line_stream, header_value);

     	   // 删除头部名称末尾的空格
		header_name.erase(header_name.find_last_not_of(" \n\r\t") + 1);
		headers_[header_name] = header_value;
		if (header_name == "Connection"){
			alive_ = (header_value == "keep-alive");
		}
	}
}*/

void RequestHandler::parseHead(std::string& request){
	std::istringstream stream(request);
	std::string line;
	while (std::getline(stream, line) && !line.empty()) {
		std::istringstream line_stream(line);
		std::string header_name;
		std::string header_value;
		std::getline(line_stream, header_name, ':');
		std::getline(line_stream, header_value);

        // 删除头部名称末尾的空格
		header_name.erase(header_name.find_last_not_of(" \n\r\t") + 1);
		
		// 删除头部值前后的空格
		size_t start = header_value.find_first_not_of(" \n\r\t");
		if (start != std::string::npos) {
			size_t end = header_value.find_last_not_of(" \n\r\t");
			header_value = header_value.substr(start, end - start + 1);
		}

		// 将头部名称转换为小写
		std::transform(header_name.begin(), header_name.end(), header_name.begin(), ::tolower);
		
		headers_[header_name] = header_value;
		//std::cout << header_name << ":" << header_value << std::endl;
		if (header_name == "connection"){
			alive_ = (header_value == "keep-alive");
		}
	}
}

void RequestHandler::parseBody(std::string& request){
	// 解析 application/x-www-form-urlencoded 格式
	if(method_ =="POST" && headers_["content-type"] == "application/x-www-form-urlencoded"){
		std::istringstream stream(request);
		std::string token;
		while (std::getline(stream, token, '&')) {
			std::string key;
			std::string value;
			std::istringstream tokenStream(token);
			std::getline(tokenStream, key, '=');
			std::getline(tokenStream, value);

		// 可以选择在这里进行URL解码
		// key = urlDecode(key);
		// value = urlDecode(value);

		post_[key] = value;
		std::cout << "key " << key << "value" << value << std::endl;
		uri_ = "/403.html";
	    }
	}
}


