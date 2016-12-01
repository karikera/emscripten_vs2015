#pragma once

namespace kr
{
	template<class Derived, typename Component, class Info>
	inline OutStream<Derived, Component, Info>& endl(OutStream<Derived, Component, Info>& _Ostr)
	{
		static const Component buffer[] = {'\r', '\n'};
		_Ostr.write(buffer, 2);
		return (_Ostr);
	}
}