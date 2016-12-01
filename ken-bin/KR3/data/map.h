#pragma once

#include <string>
#include <unordered_map>

namespace kr
{
	class KeyBuffer
	{
	public:
		KeyBuffer() noexcept;
		~KeyBuffer() noexcept;
		KeyBuffer(const void * data, size_t sz) noexcept;
		KeyBuffer(const KeyBuffer& _copy) noexcept;
		KeyBuffer(KeyBuffer&& _move) noexcept;
		KeyBuffer& operator =(const KeyBuffer& _copy) noexcept;
		KeyBuffer& operator =(KeyBuffer&& _move) noexcept;

		const void * data() const noexcept;
		size_t size() const noexcept;

		class Reference
		{
		public:
			Reference(Buffer buffer) noexcept;
			KeyBuffer& get() const noexcept;

		private:
			const void * m_data;
			size_t m_size;
		};

	private:
		void * m_data;
		size_t m_size;
	};
	
	template <typename TI, typename TK, typename TV>
	class MapWrapper:public std::unordered_map<TK, TV>
	{
	private:
		using ComponentTK = typename TK::Component;
	public:
		using super = std::unordered_map<TK, TV>;
		using typename super::iterator;
		using typename super::const_iterator;
		using typename super::size_type;
		using rawpair = std::pair<TK, TV>;

		std::pair<iterator, bool> insert(const TI &key, const TV & value)
		{
			return super::insert(rawpair(key.template cast<ComponentTK>(), value));
		}
		std::pair<iterator, bool> emplace(const TI &key, TV && value)
		{
			return super::insert(rawpair(key.template cast<ComponentTK>(), move(value)));
		}
		TV& operator [](const TI &key)
		{
			return (*(super*)this)[key.template cast<ComponentTK>()];
		}
		const TV& operator [](const TI &key) const
		{
			return (*(super*)this)[key.template cast<ComponentTK>()];
		}
		size_type erase(const TI &key)
		{
			return super::erase(key.template cast<ComponentTK>());
		}
		iterator erase(iterator iter)
		{
			return super::erase(iter);
		}
		iterator find(const TI& key)
		{
			return super::find(key.template cast<ComponentTK>());
		}
		const_iterator find(const TI& key) const
		{
			return super::find(key.template cast<ComponentTK>());
		}

	};

	template <typename TI, typename TV> 
	class MapWrapper<TI, KeyBuffer, TV>:public std::unordered_map<KeyBuffer, TV>
	{
	public:
		using super = std::unordered_map<KeyBuffer, TV>;
		using typename super::iterator;
		using typename super::const_iterator;
		using typename super::size_type;

		TV& operator [](const TI &key)
		{
			return (*(super*)this)[KeyBuffer::Reference(key).get()];
		}
		const TV& operator [](const TI &key) const
		{
			return (*(super*)this)[KeyBuffer::Reference(key).get()];
		}
		size_type erase(const TI &key)
		{
			return super::erase(KeyBuffer::Reference(key).get());
		}
		iterator erase(iterator iter)
		{
			return super::erase(iter);
		}
		iterator find(const TI& key)
		{
			return super::find(KeyBuffer::Reference(key).get());
		}
		const_iterator find(const TI& key) const
		{
			return super::find(KeyBuffer::Reference(key).get());
		}
	};

	template <typename TI, typename TV> using Map = MapWrapper<TI, ABuffer, TV>;
	template <typename TI, typename TV> using ReferenceMap = MapWrapper<TI, Buffer, TV>;

}

template <> struct std::hash<kr::KeyBuffer>
{
	size_t operator ()(const kr::KeyBuffer & buff) const noexcept;
};