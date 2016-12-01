#pragma once

#include <KR3/main.h>
#include <KR3/data/map.h>

namespace kr
{
	class PrefixMap
	{
	public:
		static constexpr size_t ONEMAP_SIZE = ('Z' - 'A' + 1) + ('z' - 'a' + 1);

		PrefixMap() noexcept;

		template <typename C, size_t sz>
		void addText(const C(&text)[sz]); //InvalidSourceException, DuplicateException

		template <typename C>
		void addText(RefArray<C> text); //InvalidSourceException, DuplicateException

		template <typename C, size_t sz>
		RefArray<C> find(const C (&text)[sz]); //NotFoundException

		template <typename C>
		RefArray<C> find(RefArray<C> text); //NotFoundException

		template <typename C>
		static size_t toIndex(C chr); //NotFoundException

	private:
		Array<size_t> m_array;
	};

	template <typename C, size_t sz>
	void PrefixMap::addText(const C(&text)[sz])
	{
		return addText((RefArray<C>)text);
	}
	template <typename C>
	void PrefixMap::addText(RefArray<C> text)
	{
		if (text.empty())
			throw InvalidSourceException();

		size_t * array = m_array.data();
		for (;;)
		{
			size_t & next = array[toIndex(*text)];
			if (next == (size_t)-1)
				throw DuplicateException();

			text++;

			if (next != 0)
			{
				if (text.empty())
					throw DuplicateException();
				array = m_array.data() + next;
			}
			else
			{
				if (text.empty())
				{
					next = (size_t)-1;
					return;
				}

				next = m_array.size();
				array = m_array.prepare(ONEMAP_SIZE);
				mema::zero(array, ONEMAP_SIZE);
			}
		}
	}
	template <typename C, size_t sz>
	RefArray<C> PrefixMap::find(const C(&text)[sz])
	{
		return find(RefArray<C>(text));
	}
	template <typename C>
	RefArray<C> PrefixMap::find(RefArray<C> text)
	{
		size_t * array = m_array.data();
		for (;;)
		{
			if (text.empty())
				throw EofException();

			size_t next = array[toIndex(*text)];
			if (next == (size_t)-1)
				return text + 1;
			if (next == 0)
				throw NotFoundException();
			array = m_array.data() + next;
			text++;
		}
	}
	template <typename C>
	size_t PrefixMap::toIndex(C chr)
	{
		if (chr > (C)'Z')
		{
			if (chr < (C)'a')
				throw NotFoundException();
			if (chr >= (C)'z')
				throw NotFoundException();
			return ((size_t)chr - (size_t)'a') + (size_t)('Z' - 'A' + 1);
		}
		else
		{
			if (chr < (C)'A')
				throw NotFoundException();
			return (size_t)chr - (size_t)'A';
		}
	}

	template <typename C> class ParameterTokenizerT
	{
	public:
		using Text = RefArray<C>;
		ParameterTokenizerT(const C * params);

		template <typename LAMBDA> void left(LAMBDA lambda);
		Array<C> nextString();
		int nextInt();
		bool hasNext();

	private:
		const C * m_params;
	};

	enum ParamType
	{
		ParamNoValue,
		ParamPrefix,
		ParamValue
	};
	template <typename C> 
	template <typename LAMBDA>
	void ParameterTokenizerT<C>::left(LAMBDA lambda)
	{
		if(m_params == nullptr) throw NotFoundException();
		const wchar_t * i = m_params;
		for(;;)
		{
			wchar_t chr = *i++;
			if(chr == '\0') break;
			lambda(chr);
		}
		m_params = nullptr;
	}

	template <typename Component>
	class ParameterT
	{
	public:
		using Text = RefArray<Component>;
		using AText = Array<Component>;

		struct ParamInfo
		{
			ParamType type;
			Text name;
			char shortCut;

			inline ParamInfo(ParamType type, Text name, char shortCut=0) noexcept
				:type(type), name(name), shortCut(shortCut)
			{
			}
		};

		struct Value
		{
			Text name;
			Text value;
		};
		ParameterT(std::initializer_list<ParamInfo> paramNames) noexcept;
		~ParameterT() noexcept;

		void start(int argn, Component ** args) noexcept;

		Value next(); // EofException, InvalidSourceException

		template <typename LAMBDA>
		bool foreach(const LAMBDA & lambda) noexcept
		{
			try
			{
				for (;;)
				{
					Value value = next();
					lambda(value.name, value.value);
				}
			}
			catch (EofException&)
			{
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		void printPosition() noexcept;

	private:
		RefArray<Component> _getValue(Text name); //EofException
		RefArray<Component> _get(); //EofException
		void _checkName(Text name); //InvalidSourceException
		RefArray<Component> _checkShortCut(Component shortCut); //InvalidSourceException

		PrefixMap m_prefix;
		Map<Text, ParamInfo> m_map;
		Text m_shortCut[PrefixMap::ONEMAP_SIZE];
		Component ** m_begin;
		Component ** m_iter;
		Component ** m_end;
	};

	EXTERN_FULL_CHAR_CLASS(ParameterTokenizer);
	EXTERN_FULL_CHAR_CLASS(Parameter);
}
