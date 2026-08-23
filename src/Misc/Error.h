#pragma once
#include "NameHelper.h"
#include <spdlog/fmt/bundled/format.h>
#include <string>
#include <tl/expected.hpp>

namespace error {
namespace detail {
class Error {
public:
	virtual std::string Message() const;
	virtual std::string Type() const;

	virtual explicit operator bool() const;
};

class ErrorCode final : public Error {
public:
	ErrorCode(misc::ErrorCode ec, std::string detail);
	std::string Message() const override;
	std::string Type() const override;

	explicit operator bool() const override;

private:
	misc::ErrorCode code_;
	std::string		detail_;
};

class Exception final : public Error {
public:
	Exception(std::string type, std::string what, std::string detail);
	std::string Message() const override;
	std::string Type() const override;

	explicit operator bool() const override;

private:
	std::string type_;
	std::string what_;
	std::string detail_;
};
} //namespace detail
// enable Error with correct polymorphism
using ErrorCode = std::unique_ptr<detail::ErrorCode>;
using Exception = std::unique_ptr<detail::Exception>;
using Error		= std::unique_ptr<detail::Error>;
ErrorCode MakeErrorCode(misc::ErrorCode ec, std::string detail = {});
Exception MakeException(std::string type, std::string what, std::string detail = {});
} //namespace error

template <>
class fmt::formatter<error::Error> {
public:
	constexpr auto parse(fmt::format_parse_context& context) {
		auto	   iter{ context.begin() };
		const auto end{ context.end() };
		if (iter != end && *iter != '}') {
			throw fmt::format_error{ "Invalid error::Error format specifier." };
		}
		return iter;
	};

	auto format(const error::Error& error, fmt::format_context& context) const {
		return fmt::format_to(context.out(), error->Message());
	}
};

template <typename T>
using Result = tl::expected<T, error::Error>;

#define MAKE_EXCEPT(type, what) \
	tl::make_unexpected(error::MakeException(type, what, FUNC_NAME))

#define MAKE_EC(ec) \
	tl::make_unexpected(error::MakeErrorCode(ec))

#define MAKE_EC_V(ec, detail) \
	tl::make_unexpected(error::MakeErrorCode(ec, detail))

#define RET_ERROR(expected)                                           \
	{                                                                 \
		auto exp_ref = expected;                                     \
		if (!(exp_ref)) {                                             \
			return tl::make_unexpected(std::move((exp_ref).error())); \
		}                                                             \
	}

#define ASSIGN(instance, expected) \
	auto(instance) = expected;     \
	{                                                                 \
		auto& exp_ref = instance;                                     \
		if (!(exp_ref)) {                                             \
			return tl::make_unexpected(std::move((exp_ref).error())); \
		}                                                             \
	}

#ifndef MY_CATCH
#	define MY_CATCH(except_type, detail)                                                          \
		catch (const except_type& except) {                                                        \
			return tl::make_unexpected(error::MakeException(#except_type, except.what(), detail)); \
		}

#	define CATCH_EXCEPTION(detail)                                                                  \
		catch (const std::system_error& except) {                                                    \
			return MAKE_EC(except.code(), detail);                                                   \
		}                                                                                            \
		MY_CATCH(std::runtime_error, detail)                                                         \
		MY_CATCH(std::logic_error, detail)                                                           \
		MY_CATCH(std::exception, detail)                                                             \
		catch (...) {                                                                                \
			return tl::make_unexpected(error::MakeException("unknown", "unkown exception", detail)); \
		}
#endif
