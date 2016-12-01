#pragma once

#ifdef KRGL_USE_D3D11
struct ID3D11SamplerState;
#else
#error Need implement
#endif

namespace kr
{

	namespace gl
	{

		class SamplerState
		{
		public:
			enum Enum
			{
				Linear       = 0,
				Nearest      = 1,
				Anisotropic  = 2,
				FilterMask   = 3,

				Repeat       = 0,
				Clamp        = 4,
				ClampBorder  = 8, // If unsupported Clamp is used instead.
				AddressMask  =12,

				Count        =16,
			};

			SamplerState() noexcept;
			SamplerState(Enum state) noexcept;
			void use(int slot) noexcept;

		private:
	#ifdef KRGL_USE_D3D11
			ID3D11SamplerState * m_ss;
	#endif

		};
	}
}

kr::gl::SamplerState::Enum operator |(kr::gl::SamplerState::Enum a, kr::gl::SamplerState::Enum b) noexcept;

