#pragma once

#include <KR3/main.h>
#include <KR3/data/switch.h>
#include <KR3/io/bufferedstream.h>
#include "parser.h"

namespace kr
{
	class JsonParser: public Parser
	{
	public:
		enum Type
		{
			Number,
			String,
			Array,
			Object
		};
		JsonParser(io::VIStream<char> is) noexcept;
		~JsonParser() noexcept;
		Type getNextType(); // kr::InvalidSourceException
		float floatNumber(); // kr::InvalidSourceException
		int integer(); // kr::InvalidSourceException
		qword getQword(); // kr::InvalidSourceException
		bool boolean(); // kr::InvalidSourceException
		ATextW text16(); // InvalidSourceException
		AText text(); // InvalidSourceException
		TText ttext(); // InvalidSourceException
		void skipValue(); // InvalidSourceException

		template <typename LAMBDA>
		void array(const LAMBDA & lambda) // InvalidSourceException
		{
			char oldchr = _open('[', ']');			
			size_t idx = 0;
			while (m_closeCharacter != 0)
			{
				lambda(idx++);
			}
			m_closeCharacter = oldchr;
			_closeTest();
		}

		template <typename LAMBDA>
		void object(const LAMBDA & lambda) // InvalidSourceException
		{
			char oldchr = _open('{', '}');
			while (m_closeCharacter != 0)
			{
				lambda(_getName());
			}
			m_closeCharacter = oldchr;
			_closeTest();
		}

	private:
		char _open(char chr, char closechr); // InvalidSourceException
		kr::TText _getName();  // kr::InvalidSourceException
		void _skipInteger(); // kr::InvalidSourceException
		void _skipIdentifier(); // kr::InvalidSourceException
		void _closeTest();  // kr::InvalidSourceException

		char m_closeCharacter;

		void _nextIs(char chr); // kr::InvalidSourceException
		char _nextIs_y(kr::Text chr); // kr::InvalidSourceException
	};
}