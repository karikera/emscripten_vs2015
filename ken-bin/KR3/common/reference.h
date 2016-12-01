#pragma once


namespace kr
{

	// �ڵ� ������Ʈ, ���� �Լ��� ���ø� �Լ��� �����Ǿ�,
	// ��� ������Ʈ ������ε� ���簡 ������ ������Ʈ�̴�.
	struct AutoComponent {};

	// �� Ŭ�����̴�.
	struct Empty {};


	// meta
	namespace meta
	{
		template <size_t ... nums>
		struct numbers;
		template <typename dorder, typename sorder, size_t count>
		struct order_util;
	}

	// coord
	namespace math
	{

		using OrderRGBA = meta::numbers<0, 1, 2, 3>;
		using OrderBasic = OrderRGBA;
		using OrderBGRA = meta::numbers<2, 1, 0, 3>;


		enum class matrix_data_type
		{
			none,
			right_bottom,
			width_height
		};

		template <typename T, size_t size, bool aligned = false, typename order = OrderBasic>
		class vector;
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type>
		class matrix_data;
		template <typename T, size_t rows, size_t cols, bool aligned, matrix_data_type type, typename vectors, typename scalars>
		class matrix_constructor;
		template <typename T, size_t rows, size_t cols, bool aligned = false, matrix_data_type type = matrix_data_type::none>
		class matrix_method;
		template <typename T, size_t rows, size_t cols, bool aligned = false, matrix_data_type type = matrix_data_type::none>
		class matrix;
		template <bool aligned>
		class quaternionT;

		using ipvec2 = vector<intp, 2, false, OrderRGBA>;
		using ipvec3 = vector<intp, 3, false, OrderRGBA>;
		using ipvec4 = vector<intp, 4, false, OrderRGBA>;
		using upvec2 = vector<uintp, 2, false, OrderRGBA>;
		using upvec3 = vector<uintp, 3, false, OrderRGBA>;
		using upvec4 = vector<uintp, 4, false, OrderRGBA>;
		using ivec2 = vector<int, 2, false, OrderRGBA>;
		using ivec3 = vector<int, 3, false, OrderRGBA>;
		using ivec4 = vector<int, 4, false, OrderRGBA>;
		using uvec2 = vector<unsigned int, 2, false, OrderRGBA>;
		using uvec3 = vector<unsigned int, 3, false, OrderRGBA>;
		using uvec4 = vector<unsigned int, 4, false, OrderRGBA>;
		using svec2 = vector<short, 2, false, OrderRGBA>;
		using svec3 = vector<short, 3, false, OrderRGBA>;
		using svec4 = vector<short, 4, false, OrderRGBA>;
		using usvec2 = vector<unsigned short, 2, false, OrderRGBA>;
		using usvec3 = vector<unsigned short, 3, false, OrderRGBA>;
		using usvec4 = vector<unsigned short, 4, false, OrderRGBA>;
		using vec4 = vector<float, 4, false, OrderRGBA>;
		using vec3 = vector<float, 3, false, OrderRGBA>;
		using vec2 = vector<float, 2, false, OrderRGBA>;
		using bvec4 = vector<byte, 4, false, OrderRGBA>;
		using bvec3 = vector<byte, 3, false, OrderRGBA>;
		using bvec2 = vector<byte, 2, false, OrderRGBA>;
		using color = vector<byte, 4, false, OrderBGRA>;
		using mat4 = matrix<float, 4, 4, false, matrix_data_type::none>;
		using mat3 = matrix<float, 3, 3, false, matrix_data_type::none>;
		using mat2 = matrix<float, 2, 2, false, matrix_data_type::none>;
		using quaternion = quaternionT<false>;

		using ivec4a = vector<int, 4, true, OrderRGBA>;
		using vec4a = vector<float, 4, true, OrderRGBA>;
		using imat4a = matrix<int, 4, 4, true, matrix_data_type::none>;
		using mat4a = matrix<float, 4, 4, true, matrix_data_type::none>;
		using quaterniona = quaternionT<true>;

		template <typename T, size_t size> using region = matrix<T, 2, size, false, matrix_data_type::right_bottom>;
		template <typename T, size_t size> using regionwh = matrix<T, 2, size, false, matrix_data_type::width_height>;
		template <typename T> using rect = matrix<T, 2, 2, false, matrix_data_type::right_bottom>;
		template <typename T> using rectwh = matrix<T, 2, 2, false, matrix_data_type::width_height>;
		template <typename T> using cube = matrix<T, 3, 2, false, matrix_data_type::right_bottom>;

		using irect = rect<int>;
		using irectwh = rectwh<int>;
		using icube = cube<int>;
		using urect = rect<unsigned int>;
		using urectwh = rectwh<unsigned int>;
		using ucube = cube<unsigned int>;
		using frect = rect<float>;
		using frectwh = rectwh<float>;
		using fcube = cube<float>;

	}

	using math::ipvec2;
	using math::ipvec3;
	using math::ipvec4;
	using math::upvec2;
	using math::upvec3;
	using math::upvec4;
	using math::ivec2;
	using math::ivec3;
	using math::ivec4;
	using math::uvec2;
	using math::uvec3;
	using math::uvec4;
	using math::svec2;
	using math::svec3;
	using math::svec4;
	using math::usvec2;
	using math::usvec3;
	using math::usvec4;
	using math::vec2;
	using math::vec3;
	using math::vec4;
	using math::bvec2;
	using math::bvec3;
	using math::bvec4;
	using math::color;
	using math::mat2;
	using math::mat3;
	using math::mat4;
	using math::irect;
	using math::irectwh;
	using math::icube;
	using math::urect;
	using math::urectwh;
	using math::ucube;
	using math::frect;
	using math::frectwh;
	using math::fcube;
	using math::quaternion;

	using math::vec4a;
	using math::ivec4a;
	using math::mat4a;
	using math::imat4a;
	using math::quaterniona;

	// util
	struct BitmapInfo;

	// win
	template <typename T = void> class Handle;

	namespace win
	{
		class Library;
		class Thread;
		class CriticalSection;
		class Window;
		class Dialog;
		class Static;
		class EditBox;
	}

	class EmbeddedBrowser;
	
	// net
	class Socket;
	class Client;
	class SocketException;

	// mt
	class EventHandle;
	class Event;

	// data
	template <typename C> class Chain;
	template <typename C> class LinkedList;
	template <typename C> class NodeLink;
	template <typename C> class Node;
	template <class ITERABLE> class TIterable;

	template <class DATA, size_t size = (size_t)-1> class ReadLock;
	template <class DATA, size_t size = (size_t)-1, bool nullterm = false> class WriteLock;

	namespace _pri_
	{
		template <class Derived, typename C, typename Info>
		class IStream_cmpAccessable;
		template <class Derived, typename C, typename Info>
		class OStream_cmpAccessable;
	}

	// ��Ʈ�� ����
	// ��Ʈ�� ������ �����Ѵ�.
	// _accessable: ���۷� ���� �׼��� ������ ����
	// Parent: �߰��� ��ӵǴ� Ŭ����
	template <bool _accessable = false, class Parent = Empty>
	class StreamInfo;

	// �б� ��Ʈ��
	// Derived: �ڽ� Ŭ����
	// Component: ������Ʈ Ÿ��
	// Info: ��Ʈ�� �� ����
	template <class Derived, typename Component, class Info = StreamInfo<>>
	class InStream;

	// ���� ��Ʈ��
	// Derived: �ڽ� Ŭ����
	// Component: ������Ʈ Ÿ��
	// Info: ��Ʈ�� �� ����
	template <class Derived, typename Component, class Info = StreamInfo<>>
	class OutStream;

	// io
	namespace io
	{
		// ���� ���� ��Ʈ��
		// Derived: �ڽ� Ŭ����
		// Base: ���ο� ������ ��Ʈ��
		// autoClose: �Ҹ��ڿ��� ���� ��Ʈ���� delete ���� ����
		// NewComponent: ��ȯ�� ������Ʈ ����
		template <class Derived, class Base, bool autoClose = false, typename NewComponent = typename Base::Component>
		class FilterOStream;

		// �б� ���� ��Ʈ��
		// Derived: �ڽ� Ŭ����
		// Base: ���ο� ������ ��Ʈ��
		// autoClose: �Ҹ��ڿ��� ���� ��Ʈ������ delete ���� ����
		// NewComponent: ��ȯ�� ������Ʈ ����
		template <class Derived, class Base, bool autoClose = false, typename NewComponent = typename Base::Component>
		class FilterIStream;


		// Ÿ���� ����� ��Ʈ���̴�.
		// void ��Ʈ������ ����ȴ�.
		template <class Derived, typename C>
		class RetypeStream;

		// �б����� ���ۿ� �ѹ��� �о���� �Ѵ�.
		// �о�� ������ �����͸� ������ �� �ִ� �Լ��� �����Ѵ�.
		// �ٷ� �о ���� ��, ������ �����Ϳ� ���ؼ� �޸� �����ϴ� ������ ���� �ȴ�.
		template <typename Derived, bool autoClose = false>
		class SelfBufferedIStream;

		// �б����� ���ۿ� �ѹ��� �о���� �Ѵ�.
		template <typename Derived, bool autoClose = false, size_t BUFFER_SIZE = 8192>
		class BufferedIStream;

		// �������� ���ۿ� ��Ƽ� �ѹ��� ���⸦ �ϰԵȴ�.
		template <typename Derived, bool endFlush = false, bool autoClose = false, size_t BUFFER_SIZE = 8192>
		class BufferedOStream;
	}

	// fs
	class File;

	namespace io
	{
		// ���� ��Ʈ��
		// ���۸� ������� �ʰ�, ���� �׼����Ѵ�.
		// ���۸� ����Ϸ���, FIStream Ȥ�� FOStream�� ����ؾ��Ѵ�.
		template <typename C>
		using FileStream = RetypeStream<File, C>;

		// ���� �б� ��Ʈ��
		// ���ۿ� �ѹ��� �о ��������,
		// SelfBufferedIStream�� ����Ѵ�.
		template <typename C, bool autoClose = true>
		using FIStream = io::SelfBufferedIStream<io::FileStream<C>, autoClose>;
		// ���� ���� ��Ʈ��
		// ���۸� ��� ����ϸ�,
		// BufferedOStream�� ����Ѵ�.
		template <typename C, bool endFlush = true, bool autoClose = true>
		using FOStream = io::BufferedOStream<io::FileStream<C>, endFlush, autoClose>;
	}


	// text
	namespace ary
	{
		namespace _pri_
		{
			template <class Data, typename C> class WrapImpl;
		}

		namespace data
		{
			template <size_t CAP, typename C, class Parent> class BufferedData;
			template <typename C, class Parent> class AllocatedData;
			template <typename C, class Parent> class TemporaryData;
			template <typename C, class Parent> class ReadableData;
			template <typename C, class Parent> class WritableData;
			template <typename C, class Parent> class AccessableData;
		}
		namespace method
		{
			template <class Parent> class BufferIOMethod;
			template <class Parent> class BufferIMethod;
			template <class Parent> class IStreamMethod;
			template <class Parent> class OStreamMethod;
			template <class Parent> class IRStreamMethod;
		}

		template <class Data> using Wrap = _pri_::WrapImpl<Data, typename Data::Component>;
	}

	// �����̳�
	// ���۳�, ��Ʈ���� ��� ����ϰ� ������, ������Ʈ Ÿ���� �����Ѵ�.
	// C: ������Ʈ Ÿ��
	// rdonly: �б� �������� ����
	// Parent: �߰��� ��ӵǴ� Ŭ����
	template <typename C, bool rdonly = true, class Parent = Empty>
	class Container;
	
	// ������ ����
	// C: ������Ʈ�� Ÿ��
	// _accessable: ���ΰ� �����ͷ� ������ �� �ִ� �������� �����̴�.
	//            true �ϰ�� begin, end, size �Լ��� �����ؾ��ϸ�,
	//            false �� ��� copyTo, size �Լ��� �����ؾ��Ѵ�.
	// _nullspace: �������� null ���� ���ڿ��� ������ �����̴�.
	// _readonly: �б� �������� �����̴�.
	// Parent: �߰��� ����� �θ� Ŭ����
	template <typename C, bool _accessable = false, bool _nullspace = false, bool _readonly = true, class Parent = Empty>
	class BufferInfo;

	// ������ �� �ִ� Ŭ����
	// Derived: �ڽ� Ŭ����
	// BufferInfo: ������ �� ����
	template <class Derived, typename BufferInfo> 
	class Bufferable;

	// ��� ������ Ŭ����
	// Derived: �ڽ� Ŭ����
	// Component: Ÿ��
	// Parent: �߰��� ����� �θ� Ŭ����
	template <class Derived, typename Component, typename Parent = Empty>
	class Printable;
		
	// �����ϴ� �迭(���� ����), �ٿ��� ������� �ȵȴ�.
	template <typename C> using WRefArray = ary::_pri_::WrapImpl<ary::data::AccessableData<C, Empty>, C>;

	// �����ϴ� �迭(�б� ����), �ٿ��� ������� �ȵȴ�.
	template <typename C> using RefArray = ary::_pri_::WrapImpl<ary::data::ReadableData<C, Empty>, C>;

	// �Ҵ�� �迭, ����� �޸𸮰� �Ҵ�Ǿ�����.
	template <typename C> using Array = ary::_pri_::WrapImpl<ary::data::AllocatedData<C, Empty>, C>;

	// �ӽ� �迭, �޸𸮰� ������ �ӽ� ���ۿ� �Ҵ�ȴ�.
	// �ӽ� �޸� ������ �Ҵ�� ���� ������ ���� ���������Ѵ�.
	template <typename C> using TmpArray = ary::_pri_::WrapImpl<ary::data::TemporaryData<C, Empty>, C>;

	// �迭 ������, C�� �̷���� �迭� ��Ͻ�Ų��.
	template <typename C> using ArrayWriter = ary::_pri_::WrapImpl<ary::data::WritableData<C, Empty>, C>;

	// ���� �迭, �ִ� CAP ���� ��Ҹ� ���� �� �ִ�.
	template <typename C, size_t CAP> using BArray = ary::_pri_::WrapImpl<ary::data::BufferedData<CAP, C, Empty>, C>;

	// 2D �Ҵ� �迭, ����� �޸𸮰� �Ҵ�Ǹ�, 2���� �迭�̴�.
	template <typename T> using Array2D = Array<Array<T>>;

	// 2D ���� �迭, �Ҵ� �迭�� �����̴�, ���� ��Ҵ� �Ҵ� �迭�̴�.
	template <typename T> using RefArray2D = RefArray<Array<T>>;

	// �ӽ� �� ���� ���ڿ� �ؽ�Ʈ, �޸𸮰� ������ �ӽ� ���ۿ� �Ҵ�ȴ�.
	// �ӽ� �޸� ������ �Ҵ�� ���� ������ ���� ���������Ѵ�.
	// const C * �� �ڵ� ĳ���� �����ϸ�, ĳ���ý� �� ���� ���ڿ��� ���δ�.
	// 1024���� ������Ʈ�� �̸� �Ҵ��Ͽ� ���´�.
	template <typename C> class TempSzText;

	// ���۸� +������ ���� ��� ��ȯ�ȴ�.
	// 1024���� ������Ʈ�� �̸� �Ҵ��Ͽ� ���´�.
	template <typename C> class RightArray;

	// �ӽ� ����, �޸𸮰� ������ �ӽ� ���ۿ� �Ҵ�ȴ�.
	// �ӽ� �޸� ������ �Ҵ�� ���� ������ ���� ���������Ѵ�.
	using TBuffer = TmpArray<void>;

	// �����ϴ� ����(�б� ����), �ٿ��� ������� �ȵȴ�.
	using Buffer = RefArray<void>;

	// �Ҵ�� ����, ����� �޸𸮰� �Ҵ�Ǿ�����.
	using ABuffer = Array<void>;

	// �����ϴ� �ؽ�Ʈ(�б� ����), �ٿ��� ������� �ȵȴ�.
	using Text = RefArray<char>;
	
	// �����ϴ� UTF-16 �ؽ�Ʈ(�б� ����), �ٿ��� ������� �ȵȴ�.
	using Text16 = RefArray<char16>;

	// �����ϴ� UTF-32 �ؽ�Ʈ(�б� ����), �ٿ��� ������� �ȵȴ�.
	using Text32 = RefArray<char32>;

	// �����ϴ� Wide �ؽ�Ʈ(�б� ����), �ٿ��� ������� �ȵȴ�.
	using TextW = RefArray<wchar>;

	// �ؽ�Ʈ ������, char �迭� ��Ͻ�Ų��.
	using Writer = ArrayWriter<char>;

	// UTF-16 �ؽ�Ʈ ������, char16 �迭� ��Ͻ�Ų��.
	using Writer16 = ArrayWriter<char16>;

	// UTF-32 �ؽ�Ʈ ������, char32 �迭� ��Ͻ�Ų��.
	using Writer32 = ArrayWriter<char32>;

	// Wide �ؽ�Ʈ ������, wchar �迭� ��Ͻ�Ų��.
	using WriterW = ArrayWriter<wchar>;

	// �Ҵ�� �ؽ�Ʈ, ����� �޸𸮰� �Ҵ�Ǿ�����.
	using AText = Array<char>;

	// �Ҵ�� UTF-16 �ؽ�Ʈ, ����� �޸𸮰� �Ҵ�Ǿ�����.
	using AText16 = Array<char16>;

	// �Ҵ�� UTF-32 �ؽ�Ʈ, ����� �޸𸮰� �Ҵ�Ǿ�����.
	using AText32 = Array<char32>;

	// �Ҵ�� Wide �ؽ�Ʈ, ����� �޸𸮰� �Ҵ�Ǿ�����.
	using ATextW = Array<wchar>;

	// �ӽ� �ؽ�Ʈ, �޸𸮰� ������ �ӽ� ���ۿ� �Ҵ�ȴ�.
	// �ӽ� �޸� ������ �Ҵ�� ���� ������ ���� ���������Ѵ�.
	using TText = TmpArray<char>;

	// �ӽ� UTF-16 �ؽ�Ʈ, �޸𸮰� ������ �ӽ� ���ۿ� �Ҵ�ȴ�.
	// �ӽ� �޸� ������ �Ҵ�� ���� ������ ���� ���������Ѵ�.
	using TText16 = TmpArray<char16>;

	// �ӽ� UTF-32 �ؽ�Ʈ, �޸𸮰� ������ �ӽ� ���ۿ� �Ҵ�ȴ�.
	// �ӽ� �޸� ������ �Ҵ�� ���� ������ ���� ���������Ѵ�.
	using TText32 = TmpArray<char32>;

	// �ӽ� Wide �ؽ�Ʈ, �޸𸮰� ������ �ӽ� ���ۿ� �Ҵ�ȴ�.
	// �ӽ� �޸� ������ �Ҵ�� ���� ������ ���� ���������Ѵ�.
	using TTextW = TmpArray<wchar>;

	// ���� ����, �ִ� CAP ����Ʈ�� ���� �� �ִ�.
	template <size_t size> using BBuffer = BArray<void, size>;

	// ���� �ؽ�Ʈ, �ִ� CAP ���� char�� ���� �� �ִ�.
	template <size_t size> using BText = BArray<char, size>;

	// ���� UTF-16 �ؽ�Ʈ, �ִ� CAP ���� char16�� ���� �� �ִ�.
	template <size_t size> using BText16 = BArray<char16, size>;

	// ���� UTF-32 �ؽ�Ʈ, �ִ� CAP ���� char32�� ���� �� �ִ�.
	template <size_t size> using BText32 = BArray<char32, size>;

	// ���� Wide �ؽ�Ʈ, �ִ� CAP ���� wchar�� ���� �� �ִ�.
	template <size_t size> using BTextW = BArray<wchar, size>;

	// �ӽ� �ؽ�Ʈ, ������ �޸𸮿� �����Ҵ� �Ǹ�, ���� ��Ģ�� ���� �Ҵ�/���� �Ǿ���Ѵ�.
	using TSZ = TempSzText<char>;

	// �ӽ� UTF-16 �ؽ�Ʈ, ������ �޸𸮿� �����Ҵ� �Ǹ�, ���� ��Ģ�� ���� �Ҵ�/���� �Ǿ���Ѵ�.
	using TSZ16 = TempSzText<char16>;

	// �ӽ� UTF-32 �ؽ�Ʈ, ������ �޸𸮿� �����Ҵ� �Ǹ�, ���� ��Ģ�� ���� �Ҵ�/���� �Ǿ���Ѵ�.
	using TSZ32 = TempSzText<char32>;

	// �ӽ� Wide �ؽ�Ʈ, ������ �޸𸮿� �����Ҵ� �Ǹ�, ���� ��Ģ�� ���� �Ҵ�/���� �Ǿ���Ѵ�.
	using TSZW = TempSzText<wchar>;

	// ����
	template <typename T> class Number;
	template <typename T> class NumberFixed;
	class NumberAddress;
	template <dword FIXED, typename T = float>
	class NumberFloat;

	namespace _pri_
	{
		template <typename T>
		inline Number<T> numberBufferize(T v);
		inline NumberFloat<0> numberBufferize(float v);
		inline NumberFloat<0, double> numberBufferize(double v);
		inline NumberFloat<0, long double> numberBufferize(long double v);
	}

	template <typename T, typename C>
	struct Bufferize;
	template <typename T, typename C>
	using bufferize_t = typename Bufferize<T, C>::type;

	class nullterm_t;
	static nullterm_t &nullterm = nullref;
}