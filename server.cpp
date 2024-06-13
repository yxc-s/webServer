#include "Server.h"
#include <iostream>


int main(){
	Server* server = new Server();
	
	server->listen();


   
    delete server;
    
    return 0;
}
