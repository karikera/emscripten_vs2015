#pragma once

namespace kr
{
	
	namespace meta
	{
		constexpr size_t ilog2(size_t v)
		{
			return (v == 1) ? 0 : ilog2(v >> 1) + 1;
		}

	}

}