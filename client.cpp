#include "Socket.h"
#include <iostream>
#include <functional>
#include <string>
#include <unistd.h>
#include "util.h"
#include <thread>


int main(){
	std::unique_ptr<Address> addr = std::make_unique<Address>("127.0.0.1", 8888);
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	std::unique_ptr<Socket> sock = std::make_unique<Socket>(sockfd, std::move(addr));

	int cnt = 1;
	std::string s(10, '*');
	sock->connect();
	/*
	int idx = 0;
    	while (true){if (cnt % 1000000 == 0){ cnt = 0; std::cout << idx ++ << std::endl;}
		sock->connect();
		write(sock->getFd(), s.c_str(), s.size());
		sock->close();	
	}
	*/

   	char buf[1024];
	bool flag = false;
	std::function<void()> fun = [&](){
		while (true){
			ssize_t sta = read(sockfd, buf, sizeof(buf));
			if (sta == 0){
				flag = true;
			}
		}
	};
	std::thread p(fun);
	
	std::cout << "connection successfuly" << std::endl;
	while (std::cin >> s && s != "!"){
		if (flag == true){
			std::cout << "断线了，重连"<< std::endl;
			std::cout << "prev sockfd" << sockfd << std::endl;
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			sock->close();
			std::cout << "new sockfd " << sockfd << std::endl;
			sock = std::make_unique<Socket>(sockfd, std::move(std::make_unique<Address>("127.0.0.1", 8888)));
			std::cout << sock->getFd() << std::endl;			
			sock->connect();
			std::cout << "重连成功"<<std::endl;
			flag = false;
			write(sockfd, s.c_str(), s.size());
			continue;
		}
		write(sockfd, s.c_str(), s.size());
	}

	return 0;
}

