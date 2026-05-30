#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

int main() {
    using tcp = boost::asio::ip::tcp;

    std::cout << "hello world\n";
    boost::asio::io_context ioc;
    boost::asio::io_context::executor_type exec = ioc.get_executor();
    tcp::socket socket(exec);
    
    return 0;
}