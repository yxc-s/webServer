#pragma once

#include <vector>
#include <string>
#include <sys/uio.h>


//使用vector<char>来缓冲数据，因为vector内存连续，并且容易拓展
//不使用dequeue：内存不连续
//不使用string：虽然内存连续，但是额外开销太多（主要用于字符串处理）


class WriteBuffer{
public:
	WriteBuffer(size_t initialCapacity = 150);

	~WriteBuffer();


	void append(const char* data);
	
	void resetIndex();

	void moveRight(size_t length);

	size_t getWriteableSpace() const;

	char* getWriteablePointer();

	int getErrno() const;


private:
	std::vector<char> buffer_;
	size_t read_index_;
	size_t write_index_;
	int errno_;
	
	size_t getTailSpace() const;
	size_t getHeadSpace () const;
	char* getBufferPointer();
};





