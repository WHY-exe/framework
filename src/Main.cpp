#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "Log/Log.h"
#include "Misc/DumpClient.h"

int main() {
    using tcp = boost::asio::ip::tcp;

    dmphelper::Client::Instancitiate(DMP_PATH("./dmp"));

    Log::Config config{};
    Log::Init(config);

    LOGI("hello world");
    boost::asio::io_context ioc;
    boost::asio::io_context::executor_type exec = ioc.get_executor();
    tcp::socket socket(exec);
    
    return 0;
}