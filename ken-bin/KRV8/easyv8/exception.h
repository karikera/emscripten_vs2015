#pragma once

namespace kr
{
	// 자바스크립트 예외 정보를 가지고 있는 클래스이다.
	// 각 필드는 예외가 발생한 위치에 대한 정보이다.
	class JsException
	{
		friend V8Object;
	public:
		// 파일 명
		JsString fileName;
		// 파일 라인
		int lineNumber;
		// 가로 위치 시작
		int startColumn;
		// 가로 위치 끝
		int endColumn;
		// 해당 라인의 소스
		JsString sourceLine;
		// 함수 호출 스택
		JsString stackTrace;
		// 오류에 대한 정보
		JsString message;

		CBS_EASYV8_DLLEXPORT JsException(v8::TryCatch * trycatch);
		inline JsException(JsString message)
		{
			this->message = move(message);
			lineNumber = startColumn = endColumn = 0;
		}
		inline JsException(Text16 message)
		{
			this->message = JsString(message);
			lineNumber = startColumn = endColumn = 0;
		}
		inline JsException(JsString strName, int line, int startcolumn, int endcolumn, JsString sourceline, JsString stacktrace, JsString message)
		{
			fileName = move(strName);
			endColumn = endcolumn;
			startColumn = startcolumn;
			lineNumber = line;
			this->message = move(message);
			sourceLine = move(sourceline);
			stackTrace = move(stacktrace);
		}
		inline JsException(const JsException& _copy)
		{
			fileName = _copy.fileName;
			lineNumber = _copy.lineNumber;
			startColumn = _copy.startColumn;
			endColumn = _copy.endColumn;
			sourceLine = _copy.sourceLine;
			stackTrace = _copy.stackTrace;
			message = _copy.message;
		}
		inline JsException(JsException&& _move)
		{
			fileName = move(_move.fileName);
			lineNumber = _move.lineNumber;
			startColumn = _move.startColumn;
			endColumn = _move.endColumn;
			sourceLine = move(_move.sourceLine);
			stackTrace = move(_move.stackTrace);
			message = move(_move.message);
		}
		inline JsException& operator =(const JsException& _copy)
		{
			this->~JsException();
			new(this) JsException(_copy);
			return *this;
		}
		inline JsException& operator =(JsException&& _move)
		{
			this->~JsException();
			new(this) JsException(move(_move));
			return *this;
		}
	};
}