#pragma once

namespace kr
{
	enum class HttpStatus
	{
		Undefined = 0,
		Ok = 200,

		MovePermanently = 301,
		Found = 302,
		NotModified = 304,

		BadRequest = 400,
		Forbidden = 403,
		NotFound = 404,

		ServerInternalError = 500,
		MethodNotImplemented = 501,
		HTTPVersionNotSupported = 505,
		CustomError = 555,
	};

}