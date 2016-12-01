#pragma once

#include "bin_search.h"

namespace kr
{
	namespace map
	{
		template <typename Parent, typename GetKey = Key<typename Parent::Component>, typename Cmp = GetKeyComparator<typename Parent::Component, GetKey>>
		class IdMap: public Parent
		{
		public:
			using Component = typename Parent::Component;
			using K = GetKeyType<Component, GetKey>;
			using Search = Searcher<Component, GetKey, Cmp>;

			IdMap() noexcept;
			IdMap(size_t capacity) noexcept;
			~IdMap() noexcept;

			bool insert(const Component &data) noexcept;
			bool replace(const Component &data) noexcept;
			void remove(K key) noexcept;
			Component removeGet(K key) noexcept;
			bool removeGet(K key, Component* dest) noexcept;
			void removeByIndex(size_t idx) noexcept;
			Component removeByIndexGet(size_t idx) noexcept;
			Component get(K key) noexcept;

			K getEmptyKey(K from, K to) noexcept;
			K getEmptyKey(K from) noexcept;

			const Component& getDataByIndex(size_t index) noexcept;
			size_t getIndexByData(Component* ptr) noexcept;
			void insertByIndex(size_t index, const Component &data) noexcept;
			void makeEmptyField(size_t size) noexcept;
			using Parent::removeMatchAll;
			using Parent::removeAll;

			template <typename LAMBDA> Component insertAlways(Component data, LAMBDA old = [](Component) {}) noexcept;
			template <typename LAMBDA> auto findAct(K key, LAMBDA lambda)->decltype(lambda(nullptr));
			template <typename LAMBDA, typename LAMBDA2> auto findAct(K key, LAMBDA found, LAMBDA2 notfound)->decltype(found(nullptr));

		protected:
			using Parent::resize;
			using Parent::push;
			using Parent::operator [];

		private:
			Component* _searchKeyLeft(K key) noexcept;
		};

		template <typename Parent, typename GetKey = Key<typename Parent::Component>, typename Cmp = GetKeyComparator<typename Parent::Component, GetKey>>
		class SortedArray : public Parent
		{
		public:
			using Component = typename Parent::Component;
			using K = GetKeyType<Component, GetKey>;
			using Search = Searcher<Component, GetKey, Cmp>;

			SortedArray() noexcept;
			SortedArray(size_t capacity) noexcept;
			~SortedArray() noexcept;

			void insert(const Component &data); // NotEnoughSpaceException
			void rankingInsert(const Component & data, size_t limit); // NotEnoughSpaceException

			void remove(K key) noexcept;
			Component removeGet(K key) noexcept;
			bool removeGet(K key, Component* dest) noexcept;
			void removeByIndex(size_t idx) noexcept;
			Component removeByIndexGet(size_t idx) noexcept;
			Component get(K key) noexcept;

			using Parent::resize;
			using Parent::operator [];
			void insertByIndex(size_t index, const Component &data) noexcept;
			void makeEmptyField(size_t size) noexcept;
			using Parent::removeMatchAll;
			using Parent::removeAll;

			template <typename LAMBDA> void rankingLoop(LAMBDA lambda) noexcept;
			template <typename LAMBDA> void rankingLoop_u(LAMBDA lambda) noexcept;
			template <typename LAMBDA> Component insertAlways(Component data, LAMBDA old = [](Component) {}) noexcept;
			template <typename LAMBDA> auto findAct(K key, LAMBDA lambda)->decltype(lambda(nullptr));
			template <typename LAMBDA, typename LAMBDA2> auto findAct(K key, LAMBDA found, LAMBDA2 notfound)->decltype(found(nullptr));

		protected:
			using Parent::push;

		private:
			Component* _searchKeyLeft(K key) noexcept;
		};
	}

	template <typename T, typename GetKey = Key<T>, typename Cmp = GetKeyComparator<T, GetKey>>
	using IdMap = map::IdMap<Array<T>, GetKey, Cmp>;
	template <typename T, size_t SZ, typename GetKey = Key<T>, typename Cmp = GetKeyComparator<T, GetKey>>
	using BIdMap = map::IdMap<BArray<T, SZ>, GetKey, Cmp>;
	template <typename T, typename GetKey = Key<T>, typename Cmp = GetKeyComparator<T, GetKey>>
	using SortedArray = map::SortedArray<Array<T>, GetKey, Cmp>;
	template <typename T, size_t SZ, typename GetKey = Key<T>, typename Cmp = GetKeyComparator<T, GetKey>>
	using BSortedArray = map::SortedArray<BArray<T, SZ>, GetKey, Cmp>;
}

#include "idmap.inl"
