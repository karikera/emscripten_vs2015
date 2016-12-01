#pragma once

#include <KR3/main.h>
#include <KR3/math/coord.h>
#include "common.h"
#include "palette.h"
#include "format.h"

typedef enum _D3DFORMAT D3DFORMAT;

namespace kr
{
	namespace gl
	{
		template <PixelFormat pf> struct Pixel;

#pragma pack(push, 1)
		namespace px
		{
			template <int _bitcount, int _offset = -1>
			struct PixelBitInfo: public PixelBitInfo<_bitcount, -1>
			{
				static constexpr int offset = _offset;
				static constexpr int bitcount = _bitcount;
				static constexpr int mask = ((1 << (_offset + _bitcount)) - 1) ^ ((1 << _offset) - 1);

				template <int _tobit>
				static int fromValue(int _rawValue) noexcept
				{
					return fromComponent<_tobit>((_rawValue& mask) >> _offset);
				}

				template <int _frombit>
				static int toValue(int _fromValue) noexcept
				{
					return toComponent<_frombit>(_fromValue) << offset;
				}

				static float floatFromValue(int _rawValue) noexcept
				{
					return (float)(_rawValue & mask) / mask;
				}

				static int floatToValue(float _from) noexcept
				{
					return (int)math::lround(_from * mask) & mask;
				}
			};

			template <>
			struct PixelBitInfo<0, -1>
			{
				static constexpr int offset = 0;
				static constexpr int bitcount = 0;
				static constexpr int mask = 0;

				template <int _tobit>
				static int fromComponent(int _comp)
				{
					return (1 << _tobit) - 1;
				}

				template <int _tobit>
				static int fromValue(int _rawValue) noexcept
				{
					return fromComponent<_tobit>(0);
				}

				template <int _frombit>
				static int toValue(int _fromValue) noexcept
				{
					return 0;
				}

				template <int _frombit>
				static int toComponent(int _fromComp) noexcept
				{
					return 0;
				}

				static float floatFromValue(int _rawValue) noexcept
				{
					return 1.f;
				}

				static int floatToValue(float _from) noexcept
				{
					return 0;
				}
			};

			template <int _offset>
			struct PixelBitInfo<0, _offset> : PixelBitInfo<0, -1>
			{
			};

			template <int _bitcount>
			struct PixelBitInfo<_bitcount, -1>
			{
				static constexpr int offset = 0;
				static constexpr int bitcount = _bitcount;
				static constexpr int mask = (1 << (_bitcount)) - 1;

				template <int _tobit>
				static int fromComponent(int _comp)
				{
					constexpr int tovalue = (1 << _tobit) - 1;
					constexpr int fromvalue = (1 << _bitcount) - 1;
					return (_comp * tovalue + fromvalue / 2) / fromvalue;
				}

				template <>
				static int fromComponent<_bitcount>(int _comp)
				{
					return _comp;
				}

				template <int _frombit>
				static int toComponent(int _fromComp) noexcept
				{
					constexpr int tovalue = (1 << _bitcount) - 1;
					constexpr int fromvalue = (1 << _frombit) - 1;
					return (_fromComp * tovalue + fromvalue / 2) / fromvalue;
				}

				template <>
				static int toComponent<_bitcount>(int _fromComp) noexcept
				{
					return _fromComp;
				}

				static float floatFromComponent(int _comp) noexcept
				{
					return (float)_comp / mask;
				}

				static int floatToComponent(float _from) noexcept
				{
					return (int)math::lround(_from * mask);
				}
			};

			using PixelBit8 = PixelBitInfo<8>;

			template <typename Derived, int pxsz> 
			struct PixelCommon
			{
				static constexpr int size = pxsz;
				byte data[pxsz];

				const Derived* base() const noexcept
				{
					return static_cast<const Derived*>(this);
				}
				Derived* base() noexcept
				{
					return static_cast<Derived*>(this);
				}

				template <int _tobit> int alpha() const
				{
					return (1 << (_tobit + 1)) - 1;
				}
				float alphaf() const
				{
					return 1.f;
				}
				vec4a getf() const noexcept
				{
					vec4a out;
					out.r = base()->redf();
					out.g = base()->greenf();
					out.b = base()->bluef();
					out.a = base()->alphaf();
					return out;
				}
				color get() const noexcept
				{
					color out;
					out.r = (byte)base()->red<8>();
					out.g = (byte)base()->green<8>();
					out.b = (byte)base()->blue<8>();
					out.a = (byte)base()->alpha<8>();
					return out;
				}

				void set(const vec4a& _color) noexcept
				{
					static_cast<Derived*>(this)->set((Pixel<PixelFormat::RGBA32F>&)_color);
				}
				void setAlpha(byte _alpha) noexcept
				{
				}
			};

			template <typename Derived, typename Type, int pxsz, int abit, int rbit, int gbit, int bbit>
			struct BitPixel: PixelCommon<Derived, pxsz>
			{
				using Super = PixelCommon<Derived, pxsz>;

				using A = PixelBitInfo<abit, rbit + gbit + bbit>;
				using R = PixelBitInfo<rbit, gbit + bbit>;
				using G = PixelBitInfo<gbit, bbit>;
				using B = PixelBitInfo<bbit, 0>;

				Type & rawValue() noexcept
				{
					return *(Type*)data;
				}
				const Type & rawValue() const noexcept
				{
					return *(Type*)data;
				}
				template <int _tobit>
				int alpha() const noexcept
				{
					return A::fromValue<_tobit>(rawValue());
				}
				template <int _tobit>
				int red() const noexcept
				{
					return R::fromValue<_tobit>(rawValue());
				}
				template <int _tobit>
				int green() const noexcept
				{
					return G::fromValue<_tobit>(rawValue());
				}
				template <int _tobit>
				int blue() const noexcept
				{
					return B::fromValue<_tobit>(rawValue());
				}
				float alphaf() const noexcept
				{
					return A::floatFromValue(rawValue());
				}
				float redf() const noexcept
				{
					return R::floatFromValue(rawValue());
				}
				float greenf() const noexcept
				{
					return G::floatFromValue(rawValue());
				}
				float bluef() const noexcept
				{
					return B::floatFromValue(rawValue());
				}

				void setAlpha(byte _alpha) noexcept
				{
					rawValue() = (Type)((*(Type*)data & (R::mask | G::mask | B::mask)) | A::toValue<8>(_alpha));
				}
				void set(const vec4a & _color) noexcept
				{
					int res = A::floatToValue(_color.a);
					res |= R::floatToValue(_color.r);
					res |= G::floatToValue(_color.g);
					res |= B::floatToValue(_color.b);
					rawValue() = (Type)res;
				}
				template <typename _Parent, int _pxsize>
				void set(const px::PixelCommon<_Parent, _pxsize> &_src) noexcept
				{
					int res = _src.base()->alpha<abit>() << A::offset;
					res |= _src.base()->red<rbit>() << R::offset;
					res |= _src.base()->green<gbit>() << G::offset;
					res |= _src.base()->blue<bbit>() << B::offset;
					rawValue() = (Type)res;
				}
			};

			struct NullColorByteData
			{
				NullColorByteData() noexcept = default;
				NullColorByteData(byte) noexcept
				{
				}

				inline NullColorByteData& operator =(byte data) noexcept
				{
					return *this;
				}
				operator byte() const noexcept
				{
					return 0xff;
				}
			};

			template <int offset>
			struct ColorByteData
			{
				using type = byte;
			};

			template <>
			struct ColorByteData<-1>
			{
				using type = NullColorByteData;
			};

			template <int offset>
			using color_byte = typename ColorByteData<offset>::type;

			template <typename Derived, int pxsz, int a, int r, int g, int b>
			struct BytePixel: PixelCommon<Derived, pxsz>
			{
				using Super = PixelCommon<Derived, pxsz>;

				using AByte = color_byte<a>;
				using RByte = color_byte<r>;
				using GByte = color_byte<g>;
				using BByte = color_byte<b>;
				AByte & rawAlpha() noexcept
				{
					return (AByte&)data[a];
				}
				const AByte & rawAlpha() const noexcept
				{
					return (AByte&)data[a];
				}
				RByte & rawRed() noexcept
				{
					return (RByte&)data[r];
				}
				const RByte & rawRed() const noexcept
				{
					return (RByte&)data[r];
				}
				GByte & rawGreen() noexcept
				{
					return (GByte&)data[g];
				}
				const GByte & rawGreen() const noexcept
				{
					return (GByte&)data[g];
				}
				BByte & rawBlue() noexcept
				{
					return (BByte&)data[b];
				}
				const BByte & rawBlue() const noexcept
				{
					return (BByte&)data[b];
				}

				template <int _tobit>
				int alpha() const noexcept
				{
					return PixelBit8::fromComponent<_tobit>(rawAlpha());
				}
				template <int _tobit>
				int red() const noexcept
				{
					return PixelBit8::fromComponent<_tobit>(rawRed());
				}
				template <int _tobit>
				int green() const noexcept
				{
					return PixelBit8::fromComponent<_tobit>(rawGreen());
				}
				template <int _tobit>
				int blue() const noexcept
				{
					return PixelBit8::fromComponent<_tobit>(rawBlue());
				}
				float alphaf() const noexcept
				{
					return PixelBit8::floatFromComponent(rawAlpha());
				}
				float redf() const noexcept
				{
					return PixelBit8::floatFromComponent(rawRed());
				}
				float greenf() const noexcept
				{
					return PixelBit8::floatFromComponent(rawGreen());
				}
				float bluef() const noexcept
				{
					return PixelBit8::floatFromComponent(rawBlue());
				}
				color get() const noexcept
				{
					color out;
					out.r = rawRed();
					out.g = rawGreen();
					out.b = rawBlue();
					out.a = rawAlpha();
					return out;
				}

				void setAlpha(byte _alpha) noexcept
				{
					rawAlpha() = _alpha;
				}
				using Super::set;
				template <typename _Parent, int _pxsize>
				void set(const px::PixelCommon<_Parent, _pxsize> &_src) noexcept
				{
					base()->rawAlpha() = _src.base()->alpha<8>();
					base()->rawRed() = _src.base()->red<8>();
					base()->rawGreen() = _src.base()->green<8>();
					base()->rawBlue() = _src.base()->blue<8>();
				}
				template <>
				void set<Derived>(const px::PixelCommon<Derived, pxsz> &_src)
				{
					*base() = *_src.base();
				}
			};
		}

		template <> struct Pixel<PixelFormat::Index>
			: px::PixelCommon<Pixel<PixelFormat::Index>, 1>
		{
			using Super = PixelCommon<Pixel<PixelFormat::Index>, 1>;

			vec4a getf() noexcept
			{
				color c = Palette::defaultPalette->get(*data);
				vec4a out;
				out.r = c.r / 255.f;
				out.g = c.g / 255.f;
				out.b = c.b / 255.f;
				out.a = c.a / 255.f;
				return out;
			}
			color get() const noexcept
			{
				return Palette::defaultPalette->get(*data);
			}

			template <int _tobit>
			int alpha() const noexcept
			{
				return px::PixelBit8::fromComponent<_tobit>(Palette::defaultPalette->get(*data).a);
			}
			template <int _tobit>
			int red() const noexcept
			{
				return px::PixelBit8::fromComponent<_tobit>(Palette::defaultPalette->get(*data).r);
			}
			template <int _tobit>
			int green() const noexcept
			{
				return px::PixelBit8::fromComponent<_tobit>(Palette::defaultPalette->get(*data).g);
			}
			template <int _tobit>
			int blue() const noexcept
			{
				return px::PixelBit8::fromComponent<_tobit>(Palette::defaultPalette->get(*data).b);
			}

			using Super::set;
			template <typename _Parent, int _pxsize>
			void set(const px::PixelCommon<_Parent, _pxsize> &_src) noexcept
			{
				color c;
				c.r = (byte)_src.base()->red<8>();
				c.g = (byte)_src.base()->green<8>();
				c.b = (byte)_src.base()->blue<8>();
				c.a = (byte)_src.base()->alpha<8>();
				Palette::defaultPalette->getNearstColor(c);
			}
		};
		template <> struct Pixel<PixelFormat::A8>
			: px::BytePixel<Pixel<PixelFormat::A8>, 1, 0, -1, -1, -1>
		{
		};
		template <> struct Pixel<PixelFormat::R5G6B5>
			: px::BitPixel<Pixel<PixelFormat::R5G6B5>, word, 2, 0, 5, 6, 5>
		{
		};
		template <> struct Pixel<PixelFormat::A1RGB5> 
			:px::BitPixel<Pixel<PixelFormat::A1RGB5>, word, 2, 1, 5, 5, 5>
		{
		};
		template <> struct Pixel<PixelFormat::X1RGB5>
			:px::BitPixel<Pixel<PixelFormat::X1RGB5>, word, 2, 0, 5, 5, 5>
		{
			using Super = px::BitPixel<Pixel<PixelFormat::X1RGB5>, word, 2, 0, 5, 5, 5>;
			using Super::set;

			void set(const Pixel<PixelFormat::A1RGB5> &_src) noexcept
			{
				*this = (Pixel&)_src;
			}
		};
		template <> struct Pixel<PixelFormat::ARGB4>
			:px::BitPixel<Pixel<PixelFormat::ARGB4>, word, 2, 4, 4, 4, 4>
		{
		};
		template <> struct Pixel<PixelFormat::RGB8>
			:px::BytePixel<Pixel<PixelFormat::RGB8>, 3, -1, 2, 1, 0>
		{

		};
		template <> struct Pixel<PixelFormat::ARGB8>
			:px::BytePixel<Pixel<PixelFormat::ARGB8>, 4, 3, 2, 1, 0>
		{
			color get() const noexcept
			{
				return *(color*)data;
			}
		};
		template <> struct Pixel<PixelFormat::XRGB8>
			:px::BytePixel<Pixel<PixelFormat::XRGB8>, 4, -1, 2, 1, 0>
		{
			using Super = px::BytePixel<Pixel<PixelFormat::XRGB8>, 4, -1, 2, 1, 0>;
			using Super::set;

			void set(const Pixel<PixelFormat::ARGB8> &_src) noexcept
			{
				*this = (Pixel&)_src;
			}
			color get() const noexcept
			{
				color out = *(color*)data;
				out.a = 255;
				return out;
			}
		};
		template <> struct Pixel<PixelFormat::BGR8>
			:px::BytePixel<Pixel<PixelFormat::BGR8>, 3, -1, 0, 1, 2>
		{
		};
		template <> struct Pixel<PixelFormat::ABGR8>
			:px::BytePixel<Pixel<PixelFormat::ABGR8>, 4, 3, 0, 1, 2>
		{
		};
		template <> struct Pixel<PixelFormat::XBGR8>
			:px::BytePixel<Pixel<PixelFormat::XBGR8>, 4, -1, 0, 1, 2>
		{
			using Super = px::BytePixel<Pixel<PixelFormat::XBGR8>, 4, -1, 0, 1, 2>;
			using Super::set;

			void set(const Pixel<PixelFormat::ABGR8> &_src) noexcept
			{
				*this = (Pixel&)_src;
			}
		};
		template <> struct Pixel<PixelFormat::RGBA32F> :
			px::PixelCommon<Pixel<PixelFormat::RGBA32F>, 4>
		{	
			using Super = px::PixelCommon<Pixel<PixelFormat::RGBA32F>, 4>;

			template <int _tobit> int alpha() const noexcept
			{
				return math::lround(alphaf() * ((1 << _tobit) - 1));
			}
			template <int _tobit> int red() const noexcept
			{
				return math::lround(((float*)data)[0] * ((1 << _tobit) - 1));
			}
			template <int _tobit> int green() const noexcept
			{
				return math::lround(((float*)data)[1] * ((1 << _tobit) - 1));
			}
			template <int _tobit> int blue() const noexcept
			{
				return math::lround(((float*)data)[2] * ((1 << _tobit) - 1));
			}
			
			float alphaf() const noexcept
			{
				return ((float*)data)[3];
			}
			float redf() const noexcept
			{
				return ((float*)data)[0];
			}
			float greenf() const noexcept
			{
				return ((float*)data)[1];
			}
			float bluef() const noexcept
			{
				return ((float*)data)[2];
			}

			void setAlpha(byte _alpha) noexcept
			{
				((float*)data)[3] = _alpha / 255.f;
			}

			using Super::set;
			template <typename _Parent, int _pxsize>
			void set(const px::PixelCommon<_Parent, _pxsize> &_src) noexcept
			{
				((float*)data)[0] = _src.base()->redf();
				((float*)data)[1] = _src.base()->greenf();
				((float*)data)[2] = _src.base()->bluef();
				((float*)data)[3] = _src.base()->alphaf();
			}
		};
#pragma pack(pop)
		
		struct FormatInfo
		{
			int size;
			D3DFORMAT d3d9Format;
			bool hasAlpha;

			void(*reformat[(size_t)PixelFormat::Count])(ImageData * _dest, const ImageData * _src);
			dword(*makeColor)(color);
			void(*imageProcessing)(ImageData * _dest, const ImageData * _src, const float * weightTable, int _weightX, int _weightY, int _weightWidth, int _weightHeight);
			void(*setAlpha)(ImageData * _dest, const ImageData * _alpha);
			void(*fill)(ImageData * _dest, color _color);
			void(*maskedRecolor)(ImageData * _dest, color _color, const ImageData * _mask);
			void(*recolor)(ImageData * _dest, color _color);
		};

		const FormatInfo * getFormatInfo(PixelFormat pf) noexcept;
		PixelFormat getFromD3D9Format(D3DFORMAT format) noexcept;
		void reformat(ImageData * dest, const ImageData * src) noexcept;
	}
}