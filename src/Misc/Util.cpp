#include "Util.h"

namespace misc {
std::string ToHex(const std::string& bytes) {
	static constexpr char hex[] = "0123456789abcdef";
	std::string			  result(bytes.size() * 2, '\0');
	for (size_t i = 0; i < bytes.size(); ++i) {
		auto value		  = static_cast<unsigned char>(bytes[i]);
		result[i * 2]	  = hex[value >> 4];
		result[i * 2 + 1] = hex[value & 0x0f];
	}
	return result;
}
} //namespace misc
