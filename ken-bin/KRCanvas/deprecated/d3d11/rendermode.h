#pragma once

struct ID3D11RasterizerState;

namespace kr
{
	namespace gl
	{
		class RenderMode
		{
		public:
			enum Enum
			{
				CullNone = 0,
				CullBack = 1,
				CullFront = 2,
				Count = 3,
			};
			RenderMode() noexcept;
			RenderMode(Enum flags) noexcept;
			RenderMode use() noexcept;

		private:
			ID3D11RasterizerState * m_state;
		};

	}
}
