#pragma once
#include <string>
#include <unordered_map>
#include <memory>

class ReadBuffer;

class RequestHandler{
public:
	enum State{
		LINE,
		HEAD,
		BODY,
		FINISH
	};
	RequestHandler();
	
	~RequestHandler();


	size_t readRequest(int fd);

	bool parseRequest();

	bool isAlive() const;		
	
	bool isParsed() const;		
	
	std::string getUri() const;	

	int getErrno() const;
	

private:
	std::unique_ptr<ReadBuffer> buffer_;
	State state_;
	std::string method_;
   	std::string uri_;		//Uniform Resource Identifier
    	std::string version_;
	std::string body_;
	std::unordered_map<std::string, std::string> headers_;
	std::unordered_map<std::string, std::string> post_;
	bool alive_;	
	bool parsed_;
	
	//carriage_return_line_feed
	
	void clear();
	
	std::string urlDecode(const std::string& uri);
	
	void parseLine(std::string& request);
	
	void parseHead(std::string& request);
	
	void parseBody(std::string& request);	//先不实现，body涉及post相关的知识，我们先处理get
	
};






