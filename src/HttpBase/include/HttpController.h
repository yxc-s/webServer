#include <map>
#include <cstdint>
#include <memory>
#include <mutex>
#include <condition_variable>


class Bridge;
class HttpHandler;

class HttpController{
public:
	HttpController();

	~HttpController();
	
	void addConnection(int fd);
	
	void closeConnection(int fd);

	void handleEvent(int fd, const uint32_t event);

	void setBridge(std::shared_ptr<Bridge> bridge);


private:
	std::map<int, HttpHandler*> http_handlers_;
	std::shared_ptr<Bridge> bridge_;
	std::mutex add_mutex_;
	std::mutex del_mutex_;
};




