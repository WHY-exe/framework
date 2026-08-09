#pragma once

#include <boost/asio/ip/icmp.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/beast/http.hpp>
#include <boost/system/error_code.hpp>

namespace misc {
namespace asio = boost::asio;
namespace http = boost::beast::http;

using tcp  = asio::ip::tcp;
using udp  = asio::ip::udp;
using icmp = asio::ip::icmp;

using ErrorCode = boost::system::error_code;
} //namespace misc