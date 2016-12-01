#pragma once

struct ID3D11BlendState;

namespace kr
{
	namespace gl
	{
		class BlendMode
		{
		public:
			enum Enum
			{
				None = 0,
				Standard = 1,
				Add = 2,
				Count = 3,
			};
			BlendMode() noexcept;
			BlendMode(Enum flags) noexcept;
			BlendMode use() noexcept;

		private:
			ID3D11BlendState * m_state;
		};
	}

}

kr::gl::BlendMode::Enum operator |(kr::gl::BlendMode::Enum a, kr::gl::BlendMode::Enum b) noexcept;
