#pragma once

struct ID3D11DepthStencilState;

namespace kr
{
	namespace gl
	{
		class DepthMode
		{
		public:
			enum Enum
			{
				None = 0,
				Write = 1,

				Always = 0,
				Less = 2,

				OperatorMask = 2,

				Count = 4
			};
			DepthMode() noexcept;
			DepthMode(Enum flags) noexcept;
			DepthMode use() noexcept;

		private:
			ID3D11DepthStencilState* m_state;
		};
	}
}

kr::gl::DepthMode::Enum operator |(kr::gl::DepthMode::Enum a, kr::gl::DepthMode::Enum b) noexcept;
