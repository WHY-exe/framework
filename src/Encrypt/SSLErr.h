#pragma once

#include <openssl/err.h>
#include <boost/system/error_code.hpp>

#include <string>

namespace sslerr {

enum class errc {
	operation_failed = 1,
	buffer_too_small,
	invalid_context,
};

class openssl_error_category final : public boost::system::error_category {
public:
	const char* name() const noexcept override {
		return "openssl";
	}

	std::string message(int value) const override {
		if (value == 0) {
			return "no OpenSSL error";
		}

		std::string error(256, 0);
		ERR_error_string_n(static_cast<unsigned long>(value),
			&error[0],
			error.length());
		error.resize(error.find_first_of('\0'));

		return error;
	}
};

class ssl_error_category final : public boost::system::error_category {
public:
	const char* name() const noexcept override {
		return "ssl";
	}

	std::string message(int value) const override {
		switch (static_cast<errc>(value)) {
			case errc::operation_failed:
				return "SSL operation failed";
			case errc::buffer_too_small:
				return "SSL output buffer too small";
			case errc::invalid_context:
				return "SSL evp context invalid";
			default:
				return "unknown SSL error";
		}
	}
};

inline const boost::system::error_category& openssl_category() {
	static openssl_error_category category;
	return category;
}

inline const boost::system::error_category& ssl_category() {
	static ssl_error_category category;
	return category;
}

inline boost::system::error_code MakeEc(unsigned long openssl_error) {
	return {
		static_cast<int>(openssl_error),
		openssl_category()
	};
}

inline boost::system::error_code MakeEc(errc error) {
	return {
		static_cast<int>(error),
		ssl_category()
	};
}

inline boost::system::error_code LastError() {
	return MakeEc(ERR_get_error());
}

inline boost::system::error_code ConsumeError() {
	unsigned long err = ERR_get_error();

	if (err != 0) {
		return MakeEc(err);
	}

	return MakeEc(errc::operation_failed);
}

} // namespace sslerr

namespace boost {
namespace system {

template <>
struct is_error_code_enum<sslerr::errc> {
	static const bool value = true;
};

} // namespace system
} // namespace boost
