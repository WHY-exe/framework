#include "Error.h"

namespace error {
namespace detail {
std::string Error::Message() const { return "no_error"; }

std::string Error::Type() const { return "basic_error"; }

Error::operator bool() const { return false; }

ErrorCode::ErrorCode(misc::ErrorCode ec, std::string detail)
	: code_(ec)
	, detail_(std::move(detail)) {}

std::string ErrorCode::Message() const {
	if (code_) {
		return fmt::format("error: {}; [{}]", code_.message(), detail_);
	} else {
		return "no error";
	}
}

std::string ErrorCode::Type() const {
	return "error_code";
}

ErrorCode::operator bool() const {
	return code_ || !detail_.empty();
}

Exception::Exception(std::string type, std::string what, std::string detail)
	: type_(std::move(type))
	, what_(std::move(what))
	, detail_(std::move(detail)) {}

std::string Exception::Message() const {
	if (*this) {
		return "no error";
	}
	return fmt::format("except_tpe: {}; what{}; [{}]", type_, what_, detail_);
}

std::string Exception::Type() const {
	return "exception";
}

Exception::operator bool() const {
	return !type_.empty() && !what_.empty();
}
} //namespace detail

ErrorCode MakeErrorCode(misc::ErrorCode code, std::string detail) {
	return std::make_unique<detail::ErrorCode>(code, std::move(detail));
}

Exception MakeException(std::string type, std::string what, std::string detail) {
	return std::make_unique<detail::Exception>(std::move(type), std::move(what), std::move(detail));
}

} //namespace error
