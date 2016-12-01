#pragma once


#include "external.h"

namespace kr
{

	template <class Class, class Parent = NativeObject>
	class JsObject :public Parent
	{
	private:
		static byte s_classObjectBuffer[sizeof(JsClass<Class>)] alignas(alignof(JsClass<Class>));

	public:
		static JsClass<Class> &classObject;

		JsObject(const JsArguments & args) :Parent(args)
		{
		}
		JsObject(const JsObject&) = delete;

		static Text16 getClassName() noexcept = delete;

		static void initMethods(JsClass<Class> * cls)
		{
		}

		// 객체를 생성합니다
		static inline V8Object newInstance(JsArgumentsIn args)
		{
			return classObject.newInstance(args);
		}
		virtual V8Class * getClass() override
		{
			return &classObject;
		}
		inline virtual void finallize() noexcept override
		{
			deleteAligned(static_cast<Class*>(this));
		}

		// 객체를 생성합니다
		// 기본적으로 Weak 상태로 생성되어, GC에 의하여 지워질 수 있습니다.
		template <typename ... ARGS> 
		static inline Class* newInstance(const ARGS & ... args)
		{
			return classObject.newInstance(args ...);
		}
	};

	template <> class JsObject<NativeObject> :public V8Object
	{
	public:
		inline JsObject(const JsArguments & args);
		inline ~JsObject() noexcept;
		// 객체를 생성합니다
		static inline V8Object newInstanceRaw(JsArgumentsIn args);
		CBS_EASYV8_DLLEXPORT static JsClass<NativeObject> & classObject;
		virtual inline V8Class * getClass();
		inline virtual void finallize() noexcept;

		// 객체를 생성합니다
		// 기본적으로 Weak 상태로 생성되어, GC에 의하여 지워질 수 있습니다.
		template <typename ... ARGS> static inline NativeObject* newInstance(const ARGS & ... args)
		{
			return classObject.newInstance(args ...);
		}

	};
	class NativeObject :public JsObject<NativeObject>
	{
		friend V8Class;
	public:
		NativeObject(const NativeObject &) = delete;

		CBS_EASYV8_DLLEXPORT NativeObject(const JsArguments & args);
		CBS_EASYV8_DLLEXPORT ~NativeObject();
		static Text16 getClassName() noexcept
		{
			return u"NativeObject";
		}
		static void initMethods(JsClass<NativeObject> * cls) noexcept
		{
		}
		CBS_EASYV8_DLLEXPORT void CT_FASTCALL setWeak();
		CBS_EASYV8_DLLEXPORT bool CT_FASTCALL deleted() noexcept;

	};

	template <class Class, class Parent>
	byte JsObject<Class, Parent>::s_classObjectBuffer[sizeof(JsClass<Class>)] alignas(alignof(JsClass<Class>));
	template <class Class, class Parent>
	JsClass<Class> &JsObject<Class, Parent>::classObject = *new((JsClass<Class>*)s_classObjectBuffer) JsClass<Class>(Class::getClassName(), &Parent::classObject);

	class JsFilterData :public JsExternalData
	{
		friend V8Class;
	private:
		int m_index;

	public:
		inline JsFilterData(int index) noexcept
			: m_index(index)
		{
		}
		virtual JsAny filter(const JsAny & _v) noexcept = 0;

	};

	class JsFilter:public Keep<JsFilterData>
	{
	private:
		template <typename FILTER> 
		class Wrap :public JsFilterData
		{
		private:
			const FILTER m_filter;

		public:
			Wrap(int index, FILTER filter) noexcept
				: JsFilterData(index), m_filter(filter)
			{
			}
			virtual void remove() noexcept override
			{
				delete this;
			}
			virtual JsAny filter(const JsAny & _v) noexcept override
			{
				return m_filter(_v);
			}

		};

	public:
		using Keep::Keep;
		using Keep::operator ->;

		inline JsFilter() noexcept
		{
		}
		template <typename FILTER>
		inline JsFilter(int index, FILTER filter):
			Keep(_new Wrap<FILTER>(index, filter))
		{
		}
	};

	class JsAccessorData : public JsExternalData
	{
	public:
		virtual void set(const V8Object &_this, const JsAny &v) noexcept = 0;
		virtual JsAny get(const V8Object &_this) noexcept = 0;
	};

	class JsAccessor :public Keep<JsAccessorData>
	{
	private:
		template <typename GET, typename SET> class Wrap :public JsAccessorData
		{
		private:
			const GET m_get;
			const SET m_set;

		public:
			Wrap(GET get, SET set) noexcept
				: m_get(get), m_set(set)
			{
			}
			virtual void remove() noexcept override
			{
				delete this;
			}
			virtual JsAny get(const V8Object &_this) noexcept override
			{
				return m_get(_this);
			}
			virtual void set(const V8Object &_this, const JsAny &v) noexcept override
			{
				return m_set(_this, v);
			}

		};

		template <typename GET> class ReadWrap :public JsAccessorData
		{
		private:
			const GET m_get;

		public:
			ReadWrap(GET get) noexcept
				: m_get(get)
			{
			}
			virtual void remove() noexcept override
			{
				delete this;
			}
			virtual JsAny get(const V8Object &_this) noexcept override
			{
				return m_get(_this);
			}
			virtual void set(const V8Object &_this, const JsAny &v) noexcept override
			{
			}
		};

	public:
		using Keep::Keep;
		using Keep::operator ->;

		template <typename GET, typename SET> inline JsAccessor(GET get, SET set) noexcept
			:Keep(_new Wrap<GET, SET>(get, set))
		{
		}
		template <typename GET> inline JsAccessor(GET get) noexcept
			:Keep(_new ReadWrap<GET>(get))
		{
		}
	};

	class JsIndexAccessor
	{
		friend V8Class;
	private:
		class Data : public JsExternalData
		{
		public:
			virtual void set(const V8Object &_this, uint32_t idx, const JsAny &v) noexcept = 0;
			virtual JsAny get(const V8Object &_this, uint32_t idx) noexcept = 0;
		};

		template <typename GET, typename SET> class Wrap :public Data
		{
		private:
			const GET m_get;
			const SET m_set;

		public:
			Wrap(GET get, SET set) noexcept
				:m_get(get), m_set(set)
			{
			}
			virtual void remove() noexcept override
			{
				delete this;
			}
			virtual JsAny get(const V8Object &_this, uint32_t idx) noexcept override
			{
				return m_get(_this, idx);
			}
			virtual void set(const V8Object &_this, uint32_t idx, const JsAny &v) noexcept override
			{
				return m_set(_this, idx, v);
			}

		};

		template <typename GET> class ReadWrap :public Data
		{
		private:
			const GET m_get;

		public:
			ReadWrap(GET get) noexcept
				:m_get(get)
			{
			}
			virtual void remove() noexcept override
			{
				delete this;
			}
			virtual JsAny get(const V8Object &_this, uint32_t idx) noexcept override
			{
				return m_get(_this, idx);
			}
			virtual void set(const V8Object &_this, uint32_t idx, const JsAny &v) noexcept override
			{
			}
		};
		Data * m_data;

	public:

		template <typename GET, typename SET> inline JsIndexAccessor(GET get, SET set) noexcept
		{
			m_data = _new Wrap<GET, SET>(get, set);
			m_data->AddRef();
		}
		template <typename GET> inline JsIndexAccessor(GET get) noexcept
		{
			m_data = _new ReadWrap<GET>(get);
			m_data->AddRef();
		}
		inline JsAny get(const V8Object &_this, uint32_t idx) const noexcept
		{
			return m_data->get(_this, idx);
		}
		inline void set(const V8Object &_this, uint32_t idx, const JsAny &v) const noexcept
		{
			m_data->set(_this, idx, v);
		}
		inline JsIndexAccessor(const JsIndexAccessor & a) noexcept
		{
			m_data = a.m_data;
			m_data->AddRef();
		}
		inline JsIndexAccessor(JsIndexAccessor && a) noexcept
		{
			m_data = a.m_data;
			a.m_data = nullptr;
		}
		inline JsIndexAccessor & operator =(const JsIndexAccessor & a) noexcept
		{
			m_data = a.m_data;
			m_data->AddRef();
			return *this;
		}
		inline JsIndexAccessor & operator =(JsIndexAccessor && a) noexcept
		{
			m_data = a.m_data;
			a.m_data = nullptr;
			return *this;
		}
		inline ~JsIndexAccessor() noexcept
		{
			if (m_data == nullptr) return;
			m_data->Release();
		}
	};

}