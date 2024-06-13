#include "ReadBuffer.h"
#include "util.h"
#include <cassert>
#include <cerrno>
#include <string.h>
#include <iostream>


ReadBuffer::ReadBuffer(size_t initialCapacity): read_index_(0), write_index_(0), errno_(0){
	buffer_.resize(initialCapacity);
	memset(iovector_, 0, sizeof(iovector_));
	ErrorIf(getReadableSpace() != 0, "Readable Space is not equal to 0 in Buffer constructor");
	ErrorIf(getTailSpace() != buffer_.size(), "Tail space is not eqaual to sizeof buffer");
	ErrorIf(getHeadSpace() != read_index_, "Head space is not correct");
	//std::cout << "BUFFER!" << std::endl;
}

ReadBuffer::~ReadBuffer(){
	if (write_index_ != read_index_){
		logError("In ReadBuffer: write_index_ != read_index_:	read_index_:", read_index_, " write_index_: ", write_index_);
	}
}


int ReadBuffer::getErrno() const{
	return errno_;
}

size_t ReadBuffer::getReadableSpace() const{
	return write_index_ - read_index_;
}


//假设对于非阻塞的iofd，如果数据已经读完了，那么read_bytes == 0,也会终止，否则会一直读
void ReadBuffer::read(int fd){
	while (true){
		memset(iovector_, 0, sizeof(iovector_));
		//感觉每次读应该要有个重置errno的操作，不然这次读成功了把上次读失败的errno返回取就完了
		errno_ = 0;
		char extra_buffer[65536];		//这个大小是基于栈的最大安全分配考虑的，旨在减少对额外堆内存的需要。
		const size_t writeable_size = getTailSpace();	
		iovector_[0].iov_base = getBufferPointer() + write_index_;
		iovector_[0].iov_len = writeable_size;
		const int iovector_nums = writeable_size >= sizeof(extra_buffer) ? 1 : 2;
		ErrorIf(write_index_ >= buffer_.size(), "write_index_ out ot bound");
		if (iovector_nums == 2){
			iovector_[1].iov_base = extra_buffer;
			iovector_[1].iov_len = 65536;
		}
		ssize_t read_bytes = readv(fd, iovector_, iovector_nums);
		errno_ = errno;
		if (read_bytes <= 0){
			std::string tmp;
			for (size_t i = read_index_; i < write_index_; ++i){
				tmp += buffer_[i];
			}
			writeLog(std::to_string(fd) + "数据已读入完成，下面展示http请求数据\n" + tmp + "\n");
			return;
		}
		else if (static_cast<size_t> (read_bytes) <= writeable_size){
			write_index_ += static_cast<size_t>(read_bytes);
		}
		else{
			write_index_ = buffer_.size();
			mergeBuffer(extra_buffer, static_cast<size_t> (read_bytes) - writeable_size);
		}
	}
}






//如果没找到要解析的行，返回空串d
std::string ReadBuffer::getRequest(){
	std::string request;
	for (size_t i = read_index_; i < write_index_ - 1; ++i){	
		if (buffer_[i] == crlf_[0] && buffer_[i + 1] == crlf_[1]){
			request = std::string(getBufferPointer() + read_index_, getBufferPointer() + i);
			read_index_ = i + 2;
			if (read_index_ == write_index_){
				read_index_ = write_index_ = 0;
			}
			break;
		}
	}
	//std::cout << "request::" << request << "  " << read_index_ << " " << write_index_ << std::endl;
	return request;
}



size_t ReadBuffer::getTailSpace() const{
	return buffer_.size() - write_index_;
}

size_t ReadBuffer::getHeadSpace() const{
	return read_index_;
}

char* ReadBuffer::getBufferPointer(){
	return buffer_.data();
}


//如果头+尾的空间不够用，那就分配。否则将可以读的空间放到最前面，然后移动writeable指针，直接写。
void ReadBuffer::mergeBuffer(char* extra_buffer, size_t merge_size){
	if (getTailSpace() + getHeadSpace() < merge_size){
		buffer_.resize(write_index_ + merge_size + 1);
	}
	else{
		size_t read_space = getReadableSpace();
		std::copy(getBufferPointer() + read_index_, getBufferPointer() + write_index_, getBufferPointer());
		read_index_ = 0;
		write_index_ = read_space;
	}
	std::copy(extra_buffer, extra_buffer + merge_size, write_index_ + getBufferPointer());
	write_index_ += merge_size;
}






