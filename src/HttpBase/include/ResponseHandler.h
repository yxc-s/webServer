#pragma once
#include <string>
#include <sys/stat.h>
#include <sys/mman.h>

class RequestHandler;
class WriteBuffer;

class ResponseHandler{
public:
	ResponseHandler(RequestHandler* request_handler);

	~ResponseHandler();
	

	void makeResponse();
	
	void write(int fd);

	int getErrno() const;
	
	bool isWriteable() const;

	
//简而言之就是将用户空间的一段内存区域映射到内核空间，映射成功后，用户对这段内存区域的修改可以直接反映到内核空间，同样，内核空间对这段区域的修改也直接反映给用户空间，对于用户空间和内核空间两者之间需要进行大量数据传输等操作的话效率是非常高的
private:
	RequestHandler* request_handler_;
	
	WriteBuffer* buffer_;
	int code_;
	int errno_;
	bool alive_;
	std::string src_;
	std::string uri_;
	std::string responsor_;		//感觉可以使用写好的buffer类，毕竟有个初始内存，不需要string一直修改内存
	char* memory_map_pointer_;
	struct stat mapped_file_state_;
	
	void clear();
	void makeLine();
	void makeHead();
	void makeContent();
};



