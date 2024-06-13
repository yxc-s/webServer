#include "ResponseHandler.h"
#include "RequestHandler.h"
#include "util.h"
#include "WriteBuffer.h"
#include <unordered_map>
#include <fcntl.h>		//baohan open
#include <unistd.h>		//close
#include <sys/uio.h>
#include <iostream>
#include <cerrno>

const std::unordered_map<int, std::string> CODE_STATUS =
{
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};


const std::unordered_map<int, std::string> CODE_PATH =
{
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

/*
const std::unordered_map<std::string, std::string> MIME_TYPE=
{
    { "html",  "text/html" },
    { "xml",   "text/xml" },
    { "xhtml", "application/xhtml+xml" },
    { "txt",   "text/plain" },
    { "rtf",   "application/rtf" },
    { "pdf",   "application/pdf" },
    { "word",  "application/msword" },
    { "png",   "image/png" },
    { "gif",   "image/gif" },
    { "jpg",   "image/jpeg" },
    { "jpeg",  "image/jpeg" },
    { "au",    "audio/basic" },
    { "mpeg",  "video/mpeg" },
    { "mpg",   "video/mpeg" },
    { "avi",   "video/x-msvideo" },
    { "mp4",   "video/mp4" },
    { "gz",    "application/x-gzip" },
    { "tar",   "application/x-tar" },
    { "css",   "text/css"},
    { "js",    "text/javascript"}
};*/

const std::unordered_map<std::string, std::string> MIME_TYPE=
{
    { "html",  "text/html" },
    { "xml",   "text/xml" },
    { "xhtml", "application/xhtml+xml" },
    { "txt",   "text/plain" },
    { "rtf",   "application/rtf" },
    { "pdf",   "application/pdf" },
    { "word",  "application/msword" },
    { "png",   "image/png" },
    { "gif",   "image/gif" },
    { "jpg",   "image/jpeg" },
    { "jpeg",  "image/jpeg" },
    { "au",    "audio/basic" },
    { "mpeg",  "video/mpeg" },
    { "mpg",   "video/mpeg" },
    { "avi",   "video/x-msvideo" },
    { "mp4",   "video/mp4" },
    { "gz",    "application/x-gzip" },
    { "tar",   "application/x-tar" },
    { "css",   "text/css"},
    { "js",   "text/javascript"},
	{"eot",   "application/vnd.ms-fontobject"},
	{"svg",   "image/svg+xml"},
	{"ttf",   "font/ttf"},
	{"woff",   "font/woff"},
	{"woff2",   "font/woff2"},
	{"scss",   "text/x-scss"}
};

const std::string FILE_PATH = "../../static/pages";

ResponseHandler::ResponseHandler(RequestHandler* request_handler): 
	request_handler_(request_handler),
	buffer_(new WriteBuffer),
	code_(-1),
	errno_(0),
	alive_(false),
	src_(FILE_PATH),
	memory_map_pointer_(nullptr)
	{
		responsor_.reserve(150);
	}

ResponseHandler::~ResponseHandler(){
	if (memory_map_pointer_ != nullptr){
		munmap(memory_map_pointer_, mapped_file_state_.st_size);
		memory_map_pointer_ = nullptr;
	}
	request_handler_ = nullptr;
	delete buffer_;
	buffer_ = nullptr;
}	





void ResponseHandler::makeResponse(){
	clear();
	std::string file_path = src_ + uri_;
	int t = code_;
	auto tt = uri_;
	if(stat(file_path.c_str(), &mapped_file_state_) < 0 || S_ISDIR(mapped_file_state_.st_mode)){
		code_ = 404;
	}
	else if (!(mapped_file_state_.st_mode & S_IROTH)){
		code_ = 403;		//其他用户不可读(403禁止访问)
	}
	else if(code_ == -1){
		code_ = 400;
	}
	if (CODE_STATUS.count(code_) == 0){
		code_ = 400;
	}
	if (CODE_PATH.count(code_)){
		uri_ = CODE_PATH.at(code_);
		stat((src_ + uri_).c_str(), &mapped_file_state_);
	}
	if (code_ != t){
		writeLog("web客户端发起请求:" + tt +"无法被识别，code_由" + std::to_string(t) + "变为" + std::to_string(code_));
	}
	makeLine();
	makeHead();
	makeContent();
	writeLog("响应准备完成，准备更新epoll事件\n");
}


void ResponseHandler::write(int fd){
	//std::cout << responsor_ << std::endl;
	writeLog("对客户端fd" + std::to_string(fd) + "进行写请求, 下面是响应信息:\n");
	struct iovec iovector[2];
	int iovector_nums = 1;
	//iovector[0].iov_base = responsor_.data();//;buffer_->getWriteablePointer();
	//iovector[0].iov_len = responsor_.size();//;buffer_->getWriteableSpace();
	iovector[0].iov_base = buffer_->getWriteablePointer();
	iovector[0].iov_len = buffer_->getWriteableSpace();
	writeLog(std::string((char*)iovector[0].iov_base, (char*)iovector[0].iov_base + iovector[0].iov_len));
	size_t cnt = -iovector[0].iov_len;
	size_t t = iovector[0].iov_len;
	if (mapped_file_state_.st_size > 0 && memory_map_pointer_){
		iovector_nums ++;
		iovector[1].iov_base = memory_map_pointer_;	
		iovector[1].iov_len = mapped_file_state_.st_size;
	}
	while (true){
		ssize_t write_bytes = writev(fd, iovector, iovector_nums);
		errno_ = errno;
		if (write_bytes >= 0){
			cnt += static_cast<int> (write_bytes);
		}
		if (write_bytes < 0){
			break;
		}
		else if (iovector[0].iov_len + iovector[1].iov_len == 0){
			break;
		}
		else if (static_cast<size_t>(write_bytes) > iovector[0].iov_len){
			iovector[1].iov_base = static_cast<char*>(iovector[1].iov_base) + (write_bytes - iovector[0].iov_len);
			iovector[1].iov_len -= (write_bytes - iovector[0].iov_len);
			iovector[0].iov_len = 0;
		}
		else{
			iovector[0].iov_base = static_cast<char*>(iovector[0].iov_base) + write_bytes;
			iovector[0].iov_len -= write_bytes;
			buffer_->moveRight(static_cast<size_t>(write_bytes));
		}
	}
	if (iovector_nums == 2){
		writeLog("第二部分实际发送数据, block2:" + std::to_string(cnt) + "\n");
	}
	writeLog("发送数据总量:" + std::to_string(cnt + t) + "\n");
}

bool ResponseHandler::isWriteable() const{
	//return false;//responsor_.size() > 0;
	return buffer_->getWriteableSpace() != 0;
}

int ResponseHandler::getErrno() const{
	return errno_;
}

void ResponseHandler::clear(){
	if (memory_map_pointer_ != nullptr){
		munmap(memory_map_pointer_, mapped_file_state_.st_size);
		memory_map_pointer_ = nullptr;
	}
	//src_?
	uri_ = request_handler_->getUri();
	alive_ = request_handler_->isAlive();
	code_ = request_handler_->isParsed() ? 200 : 400;
	errno_ = 0;
	responsor_.clear();
}


void ResponseHandler::makeLine(){
	responsor_ += "HTTP/1.1 ";
	buffer_->append("HTTP/1.1 ");
	responsor_ += std::to_string(code_);
	buffer_->append((std::to_string(code_)).c_str());
	responsor_ += ' ';
	buffer_->append(" ");
	responsor_ += CODE_STATUS.at(code_);
	buffer_->append(CODE_STATUS.at(code_).c_str());
	responsor_ += "\r\n";
	buffer_->append("\r\n");
}


void ResponseHandler::makeHead(){
	responsor_ += "Connection: ";
	buffer_->append("Connection: ");
	responsor_ += (alive_ ? "keep-alive" : "close");
	buffer_->append(alive_ ? "keep-alive\r\n" : "close\r\n");
	responsor_ += "Content-type: ";
	buffer_->append("Content-type: ");
	size_t pos = uri_.find_last_of('.');
	//ErrorIf(uri_.back() == '.', "sri with invalid : " + uri_);
	if (pos == uri_.npos || !MIME_TYPE.count(uri_.substr(pos + 1))){
		responsor_ += "text/plain";
		buffer_->append("text/plain");
	}
	else{
		responsor_ += MIME_TYPE.at(uri_.substr(pos + 1));
		buffer_->append((uri_.substr(pos + 1)).c_str());	
	}
	responsor_ += "\r\n";
	buffer_->append("\r\n");
}


void ResponseHandler::makeContent(){
	int fd = open((src_ + uri_).c_str(), O_RDONLY);
	if (fd == -1){
		std::cout << "make content -1, uri---------------------:" << uri_ <<  std::endl;
		return;	
	}
	void* memory_map_res = mmap(0, mapped_file_state_.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(memory_map_res == MAP_FAILED)
	{
		std::cout << "make content MAP_FAILED, uri---------------------:" << uri_ <<  std::endl;
		return ;
	}
	memory_map_pointer_ = (char*)memory_map_res;		//需要在响应发送完毕后释放
	close(fd);
	responsor_ += "Content-length: ";
	buffer_->append("Content-length: ");
	responsor_ += std::to_string(mapped_file_state_.st_size);
	buffer_->append((std::to_string(mapped_file_state_.st_size)).c_str());	
	responsor_ += "\r\n\r\n";
	buffer_->append("\r\n\r\n");
}





