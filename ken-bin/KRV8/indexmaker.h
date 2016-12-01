#pragma once

namespace kr
{
	// 인덱스 생성기
	// 여러가지 값들을 합성하여 인덱스로 만들어낸다
	class IndexMaker
	{
	private:
		int index;
		int maximum;

	public:
		// 특정 값을 인덱스로 만들 때, 규칙을 설정한다.
		// 템플릿 특수화를 이용하여, 규칙을 변경할 수 있다.
		template <typename T> inline int make(T value);

		inline IndexMaker()
		{
			index = 0;
			maximum = 1;
		}
		template <typename T>
		inline IndexMaker(T value)
		{
			index = make(value);
			maximum = StateConst<T>::count;
		}

		// 값 합성, 대입
		template <typename T>
		inline IndexMaker& operator |=(T value)
		{
			typedef StateConst<T> C;
			index += make(value) * maximum;
			maximum *= C::count;
			return *this;
		}

		// 값 합성
		template <typename T>
		inline const IndexMaker operator |(T value) const
		{
			IndexMaker out = *this;
			out |= value;
			return out;
		}

		//// XXX: 인텔리 센스의 오작동으로, 사용하지 않는다
		//template <typename T>
		//inline friend const IndexMaker operator |(T value, const IndexMaker im)
		//{
		//	im |= value;
		//	return im;
		//}

		// 숫자로 가져온다
		inline operator int() const
		{
			return index;
		}
	};

	template <typename T> inline int IndexMaker::make(T value)
	{
		typedef StateConst<T> C;
		int res = value - C::offset;
		assert(res < C::count);
		return res;
	}

}