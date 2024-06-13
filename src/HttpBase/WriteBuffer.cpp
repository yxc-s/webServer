#include "WriteBuffer.h"
#include "util.h"
#include <cassert>
#include <cerrno>
#include <string.h>
#include <iostream>


WriteBuffer::WriteBuffer(size_t initialCapacity): read_index_(0), write_index_(0), errno_(0){
	buffer_.resize(initialCapacity);
	ErrorIf(getWriteableSpace() != 0, "Readable Space is not equal to 0 in Buffer constructor");
	ErrorIf(getTailSpace() != buffer_.size(), "Tail space is not eqaual to sizeof buffer");
	ErrorIf(getHeadSpace() != write_index_, "Head space is not correct");
}

WriteBuffer::~WriteBuffer(){
	//ErrorIf(write_index_ != read_index_, "wrong index happened in writebuffer");
	if (write_index_ != read_index_){
		logError("In ReadBuffer: write_index_ != read_index_:	write_index_:", write_index_, " read_index_: ", read_index_);
	}
}


void WriteBuffer::append(const char* data){
	size_t data_size = 0;
	while (data[data_size] != '\0'){
		data_size ++;
	}
	size_t tail_size = getTailSpace();
	size_t head_size = getHeadSpace();
	if (tail_size + head_size < data_size){
		buffer_.resize(read_index_ + data_size + 1);
	}
	else if (tail_size < data_size){
		size_t write_space = getWriteableSpace();
		std::copy(getBufferPointer() + write_index_, getBufferPointer() + read_index_, getBufferPointer());
		write_index_ = 0;
		read_index_ = write_space;
		
	}
	std::copy(data, data + data_size, getBufferPointer() + read_index_);
	read_index_ += data_size;
}

void WriteBuffer::resetIndex(){
	write_index_ = read_index_ = 0;
}

void WriteBuffer::moveRight(size_t length){
	write_index_ += length;
	if (write_index_ == read_index_){
		resetIndex();
	}
	ErrorIf(write_index_ >= buffer_.size(), "write buffer has a invalid write_index");
}

int WriteBuffer::getErrno() const{
	return errno_;
}

size_t WriteBuffer::getWriteableSpace() const{
	return read_index_ - write_index_;
}

char* WriteBuffer::getWriteablePointer(){
	return getBufferPointer() + write_index_;
}





size_t WriteBuffer::getTailSpace() const{
	return buffer_.size() - read_index_;
}

size_t WriteBuffer::getHeadSpace() const{
	return write_index_;
}

char* WriteBuffer::getBufferPointer(){
	return buffer_.data();
}






