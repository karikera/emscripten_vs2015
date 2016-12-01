#pragma once

#include "../main.h"

namespace kr
{
	template <typename T> struct Key;


	namespace _pri_
	{
		template <typename T, bool iscls> struct _Key;
		template <typename T> struct _Key<T, true>
		{
			static auto getKey(const T &t) noexcept -> decltype(t.getKey())
			{
				return t.getKey();
			}
		};
		template <typename T> struct _Key<Keep<T>, true>
		{
			static auto getKey(const T *t) noexcept -> decltype(t->getKey())
			{
				return t->getKey();
			}
		};
		template <typename T> struct _Key<Must<T>, true>
		{
			static auto getKey(const T *t) noexcept -> decltype(t->getKey())
			{
				return t->getKey();
			}
		};
		template <typename T> struct _Key<T*, false>
		{
			static auto getKey(const T *t) noexcept -> decltype(t->getKey())
			{
				return t->getKey();
			}
		};
		template <typename T> struct _Key<T, false>
		{
			static T getKey(const T &t) noexcept
			{
				return t;
			}
		};

		template <typename T, bool iscls> struct _Comparator;
		template <typename T> struct _Comparator<T, true>
		{
			static intp compare(const T& a, const T&b)
			{
				return a.compare(b);
			}
		};
		template <typename T> struct _Comparator<T, false>
		{
			static intp compare(const T& a, const T&b) noexcept
			{
				using K = Key<T>;
				return (intp)K::getKey(a) - (intp)K::getKey(b);
			}
		};

	}

	template <typename T> struct Comparator: _pri_::_Comparator<T, std::is_class<T>::value>
	{
	};
	template <typename T> struct Key: _pri_::_Key<T, std::is_class<T>::value>
	{
	};

	template <typename T, typename GetKey = Key<T>>
	using GetKeyType = decltype(GetKey::getKey(*(T*)0));
	template <typename T, typename GetKey = Key<T>>
	using GetKeyComparator = Comparator<GetKeyType<T, GetKey>>;

	template <typename T, typename GetKey = Key<T>, typename Cmp = GetKeyComparator<T, GetKey>>
	class Searcher:public Cmp, public GetKey
	{
	public:
		using K = GetKeyType<T, GetKey>;
		static T* searchPointer(T* L, size_t size, K key)
		{
			L--;
			T* R = L + size;
			T* E = R;

			while (R > L + 1)
			{
				T* C = (R - L) / 2 + L;
				if (compare(getKey(*C), key) < 0) L = C;
				else R = C;
			}

			if (R == E) return nullptr;
			else if (compare(getKey(*R), key) == 0) return R;
			else return nullptr;
		}
		template <typename FOUND, typename NOTFOUND>
		static auto search(T* L, size_t size, K key, FOUND found, NOTFOUND notfound)->decltype(found((T*)0))
		{
			T* R = L + size;
			T* E = R;
			L--;

			while (R > L + 1)
			{
				T* C = (R - L) / 2 + L;
				if (compare(getKey(*C), key) < 0) L = C;
				else R = C;
			}

			if (R == E) return notfound(E);
			else if (compare(getKey(*R), key) == 0) return found(R);
			else return notfound(R);
		}
		static const T* searchLeft(const T* L, size_t size, K key)
		{
			L--;
			const T* R = L + size;
			const T* E = R;

			while (R > L + 1)
			{
				const T* C = (R - L) / 2 + L;
				if (compare(getKey(*C), key) < 0) L = C;
				else R = C;
			}

			return R;
		}

	};
}
