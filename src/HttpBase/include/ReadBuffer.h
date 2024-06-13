#pragma once

#include <vector>
#include <string>
#include <sys/uio.h>


//使用vector<char>来缓冲数据，因为vector内存连续，并且容易拓展
//不使用dequeue：内存不连续
//不使用string：虽然内存连续，但是额外开销太多（主要用于字符串处理）


class ReadBuffer{
public:
	ReadBuffer(size_t initialCapacity = 1024);

	~ReadBuffer();
	
	//返回读到的数据数
	void read(int fd);

	size_t getReadableSpace() const;

	char* getReadablePointer() const;

	std::string getRequest();
	
	int getErrno() const;

private:
	std::vector<char> buffer_;
	size_t read_index_;
	size_t write_index_;
	int errno_;
	struct iovec iovector_[2];
	const char* crlf_ = "\r\n";

	
	size_t getTailSpace() const;
	size_t getHeadSpace () const;
	
	char* getBufferPointer();
	void mergeBuffer(char* extra_buffer, size_t merge_size);
};





