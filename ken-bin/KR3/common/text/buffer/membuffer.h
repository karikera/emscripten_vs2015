#pragma once

namespace kr
{
	template <typename C>
	class ReverseIterable
	{
	private:
		C * m_beg;
		C * m_end;

	public:
		class Iterator :public TIterator<Iterator, C>
		{
		private:
			using TIterator<Iterator, C>::m_pt;

		public:
			INHERIT_ITERATOR(TIterator<Iterator, C>);

			intp operator -(const Iterator & o) const noexcept
			{
				return o.m_pt - m_pt;
			}
			Iterator& operator += (intp v) noexcept
			{
				m_pt -= v;
				return *this;
			}
			Iterator& operator -= (intp v) noexcept
			{
				m_pt += v;
				return *this;
			}
			Iterator& operator ++() noexcept
			{
				m_pt--;
				return *this;
			}
			Iterator& operator --() noexcept
			{
				m_pt++;
				return *this;
			}
			C& operator [](size_t idx) const noexcept
			{
				return m_pt[-(intp)idx];
			}
		};

		ReverseIterable(C* beg, C* end) noexcept
		{
			m_beg = end - 1;
			m_end = beg - 1;
		}
		Iterator begin() const noexcept
		{
			return m_beg;
		}
		Iterator end() const noexcept
		{
			return m_end;
		}
	};

	namespace buffer
	{
		template <typename Derived, typename Info>
		class MemBuffer :public Info
		{
			CLASS_HEADER(MemBuffer, Info);
		public:
			INHERIT_COMPONENT();

			using Super::Super;
			using Info::accessable;
			using Info::szable;

			ComponentRef* data() noexcept
			{
				return static_cast<Derived*>(this)->begin();
			}
			ComponentRef* begin() noexcept
			{
				return static_cast<Derived*>(this)->begin();
			}
			ComponentRef* end() noexcept
			{
				return static_cast<Derived*>(this)->end();
			}
			const ComponentRef* data() const noexcept
			{
				return static_cast<const Derived*>(this)->begin();
			}
			const Component * begin() const noexcept
			{
				return static_cast<const Derived*>(this)->begin();
			}
			const Component * end() const noexcept
			{
				return static_cast<const Derived*>(this)->end();
			}
			size_t size() const noexcept
			{
				return static_cast<const Derived*>(this)->size();
			}
			bool empty() const noexcept
			{
				static_assert(&MemBuffer::empty != &Derived::empty, "This function need override");
				return static_cast<const Derived*>(this)->empty();
			}

			template <typename T> size_t sizeAs() const noexcept
			{
				static_assert(is_same<T, Component>::value, "Need same type");
				return size();
			}

			bool operator ==(const MemBuffer& other) const
			{
				size_t tsize = size();
				size_t osize = other.size();
				if (tsize != osize) return false;
				return mem::equals(begin(), other.begin(), tsize);
			}
			bool operator !=(const MemBuffer& other) const
			{
				return !(*this == other);
			}
			bool equals(Ref _v) const noexcept
			{
				size_t _len = size();
				size_t _len2 = _v.size();
				if (_len != _len2) return false;
				else return memm::equals(begin(), _v.begin(), _len);
			}
			size_t hash() const noexcept
			{
				return mem::hash(begin(), size() * sizeof(ComponentRef));
			}
			Ref beginIndex() const noexcept
			{
				return Ref(begin(), begin());
			}
			Ref endIndex() const noexcept
			{
				return Ref(end(), end());
			}
			size_t copyTo(Component * dest) const
			{
				mema::subs_copy((InternalComponent*)dest, (InternalComponent*)begin(), size());
				return size();
			}
			size_t sizeBytes() const noexcept
			{
				return size() * sizeof(InternalComponent);
			}
			bool contains_ptr(const ComponentRef * _v) const noexcept
			{
				return begin() <= _v && _v <= end();
			}
			template <class _Derived, bool a, bool b, class _Parent>
			bool contains_ptr(const Bufferable<_Derived, BufferInfo<Component, true, a, b, _Parent>>& _v) const noexcept
			{
				return contains_ptr(_v.begin());
			}
			Ref cut(const ComponentRef* end) const noexcept
			{
				_assert(contains_ptr(end));
				return Ref(begin(), end);
			}
			Ref cut(Ref _v) const noexcept
			{
				return cut(_v.begin());
			}
			Ref cut(size_t _len) const noexcept
			{
				return cut(math::min(begin() + _len, end()));
			}

			Ref subarr(size_t _left, size_t _count) const noexcept
			{
				return Ref(begin() + _left, end()).cut(_count);
			}
			Ref subarr(size_t _left) const noexcept
			{
				return Ref(begin() + _left, end());
			}
			Ref subarray(size_t _left) const noexcept
			{
				return Ref(begin() + _left, end());
			}
			Ref subarray(size_t _left, size_t _right) const noexcept
			{
				_assert(_left <= _right);
				return Ref(begin() + _left, begin() + _right);
			}

			bool startsWith(Ref _v) const noexcept
			{
				if (_v.size() > size())
					return false;
				return cut(_v.size()) == _v;
			}
			bool endsWith(Ref _v) const noexcept
			{
				if (_v.size() > size())
					return false;
				return subarr(size() - _v.size()) == _v;
			}
			bool startsWith_i(Ref _v) const noexcept
			{
				if (_v.size() > size())
					return false;
				return cut(_v.size()).equals_i(_v);
			}
			bool endsWith_i(Ref _v) const noexcept
			{
				if (_v.size() > size())
					return false;
				return subarr(size() - _v.size()).equals_i(_v);
			}
			bool startsWith(const InternalComponent &_v) const noexcept
			{
				if (empty())
					return false;
				return front() == _v;
			}
			bool endsWith(const InternalComponent &_v) const noexcept
			{
				if (empty())
					return false;
				return back() == _v;
			}
			bool startsWith_i(const InternalComponent &_v) const noexcept
			{
				if (empty())
					return false;
				return memm::equals_i(front(), _v);
			}
			bool endsWith_i(const InternalComponent &_v) const noexcept
			{
				if (empty())
					return false;
				return memm::equals_i(back(), _v);
			}

			InternalComponentRef& operator [](size_t i) noexcept
			{
				_assert(i < size());
				return begin()[i];
			}
			const InternalComponentRef& operator [](size_t i) const noexcept
			{
				_assert(i < size());
				return begin()[i];
			}
			InternalComponentRef& operator *()
			{
				return *begin();
			}
			const InternalComponentRef& operator *() const
			{
				return *begin();
			}
			const InternalComponentRef& get(size_t index) const noexcept
			{
				return begin()[index];
			}
			InternalComponentRef& get(size_t index) noexcept
			{
				return begin()[index];
			}
			void set(size_t index, const InternalComponent& src) noexcept
			{
				_assert(index < size());
				begin()[index] = src;
			}

			bool contains(const InternalComponent &_v) const noexcept
			{
				return memm::contains(begin(), _v, size());
			}
			bool contains(Ref _v) const noexcept
			{
				size_t _len = size();
				size_t _len2 = _v.size();
				if (_len < _len2) return false;
				return memm::find(begin(), _v.begin(), _len, _len2) != nullptr;
			}
			Ref find_n(const InternalComponent &_v) const noexcept
			{
				return Ref(memm::find_n(begin(), _v, size()), end());
			}
			Ref find_ne(const InternalComponent &_v) const noexcept
			{
				return Ref(memm::find_ne(begin(), _v, size()), end());
			}
			Ref find_ny(Ref _v) const noexcept
			{
				return Ref(memm::find_ny(begin(), _v.begin(), size(), _v.size()), end());
			}
			Ref find_nry(Ref _v) const noexcept
			{
				return Ref(memm::find_nry(begin(), _v.begin(), size(), _v.size()), end());
			}
			Ref find_nye(Ref _v) const noexcept
			{
				return Ref(memm::find_nye(begin(), _v.begin(), size(), _v.size()), end());
			}
			Ref find(const InternalComponent &needle) const noexcept
			{
				return Ref(memm::find(begin(), needle, size()), end());
			}
			Ref find(Ref _v) const noexcept
			{
				size_t _len = size();
				size_t _len2 = _v.size();
				if (_len < _len2)
					return nullptr;
				return Ref(memm::find(begin(), _v.begin(), _len, _len2), end());
			}
			Ref find_e(const InternalComponent &_v) const noexcept
			{
				return Ref(memm::find_e(begin(), _v, size()), end());
			}
			Ref find_e(Ref _v) const noexcept
			{
				Ref finded = find(_v);
				if (finded == nullptr)
					return endIndex();
				return finded;
			}
			Ref find_y(Ref _v) const noexcept
			{
				return Ref(memm::find_y(begin(), _v.begin(), size(), _v.size()), end());
			}
			Ref find_ye(Ref _v) const noexcept
			{
				return Ref(memm::find_ye(begin(), _v.begin(), size(), _v.size()), end());
			}
			Ref find_r(const InternalComponent &_v) const noexcept
			{
				return Ref(memm::find_r(begin(), _v, size()), end());
			}
			Ref find_r(Ref _v) const noexcept
			{
				size_t _len = size();
				size_t _len2 = _v.size();
				if (_len < _len2)
					return nullptr;
				return Ref(memm::find_r(begin(), _v.begin(), _len, _len2), end());
			}
			Ref find_ry(Ref _v) const noexcept
			{
				return Ref(memm::find_ry(begin(), _v.begin(), size(), _v.size()), end());
			}
			Ref find_re(const InternalComponent &_v) const noexcept
			{
				return Ref(memm::find_re(begin(), _v, size()), end());
			}
			Ref find_re(Ref _v) const noexcept
			{
				Ref finded = find_r(_v);
				if (finded == nullptr)
					return Ref(begin()-1, end());
				return finded;
			}
			template <typename LAMBDA>
			Ref find_L(const LAMBDA & lambda) const noexcept
			{
				InternalComponentRef * e = end();
				InternalComponentRef * p = begin();
				for (; p != e; p++)
				{
					if (lambda(*p)) return Ref(p, end());
				}
				return Ref(nullptr, end());
			}

			Ref end_find_r(const InternalComponent &needle) noexcept
			{
				const Component * _end = memm::find_r(begin(), needle, size());
				if (_end == nullptr)
				{
					Ref out;
					out.setEnd(nullptr);
					return out;
				}
				return Ref(begin(), _end + 1);
			}
			Ref end_find_re(const InternalComponent &needle) noexcept
			{
				return Ref(begin(), memm::find_re(begin(), needle, size()) + 1);
			}
			Ref end_find_r(Ref needle) noexcept
			{
				size_t _len = size();
				size_t _len2 = needle.size();
				if (_len < _len2)
				{
					Ref out;
					out.setEnd(nullptr);
					return out;
				}
				const Component * _end = memm::find_r(begin(), needle.begin(), _len, _len2);
				if (_end == nullptr)
				{
					Ref out;
					out.setEnd(nullptr);
					return out;
				}
				return Ref(begin(), _end + needle.size());
			}
			Ref end_find_re(Ref needle) noexcept
			{
				Ref finded = end_find_r(needle);
				if (finded.end() == nullptr)
					return Ref(begin(), begin());
				return finded;
			}

			size_t pos(const InternalComponent &_v) const noexcept
			{
				return memm::pos(begin(), _v, size());
			}
			size_t pos(Ref _v) const noexcept
			{
				size_t len2 = _v.size();
				if (size() < len2) return -1;
				return memm::pos(begin(), _v.begin(), size(), len2);
			}
			size_t pos_y(Ref _v) const noexcept
			{
				return memm::pos_y(begin(), _v.begin(), size(), _v.size());
			}
			size_t pos_r(const InternalComponent &_v) const noexcept
			{
				return memm::pos_r(begin(), _v, size());
			}
			size_t pos_ry(Ref _v) const noexcept
			{
				return memm::pos_ry(begin(), _v.begin(), size(), _v.size());
			}
			size_t pos_re(const InternalComponent &_v) const noexcept
			{
				return memm::pos_re(begin(), _v, size());
			}
			size_t pos_nry(Ref _v) const noexcept
			{
				return memm::pos_nry(begin(), _v.begin(), size(), _v.size());
			}
			size_t count(const InternalComponent &_v) const noexcept
			{
				return memm::count(begin(), _v, size());
			}

			const InternalComponentRef& front() const noexcept
			{
				return begin()[0];
			}
			InternalComponentRef& front() noexcept
			{
				return begin()[0];
			}
			const InternalComponentRef& back() const noexcept
			{
				return end()[-1];
			}
			InternalComponentRef& back() noexcept
			{
				return end()[-1];
			}

			template <class ODerived, typename _Info>
			size_t writeTo(OutStream<ODerived, Component, _Info> * os) const
			{
				size_t sz = size();
				os->write(data(), sz);
				return sz;
			}

			template <typename NewComponent>
			RefArray<NewComponent> cast() const noexcept
			{
				return RefArray<NewComponent>((NewComponent*)begin(), (NewComponent*)end());
			}

			template <class Converter, class _Derived, typename _Info>
			void replace(OutStream<_Derived, typename Converter::Component, _Info> * _out, Ref _tar, RefArray<typename Converter::Component> _to) const // NotEnoughSpaceException
			{
				Ref reader = *static_cast<const Bufferable<Derived, Info>*>(this);

				for (;;)
				{
					Ref finded = reader.readto(_tar);
					if (finded == nullptr)
					{
						_out->print(Converter(reader));
						return;
					}
					_out->print(Converter(finded));
					_out->write(_to);
				}
			}
			template <class Converter, class _Derived, typename _Info>
			void replace(OutStream<_Derived, typename Converter::Component, _Info> * _out, const InternalComponent &_tar, const internal_component_t<typename Converter::Component> &_to) const // NotEnoughSpaceException
			{
				Ref reader = *static_cast<const Bufferable<Derived, Info>*>(this);

				for (;;)
				{
					Ref finded = reader.readto(_tar);
					if (finded == nullptr)
					{
						_out->print(Converter(reader));
						return;
					}
					_out->print(Converter(finded));
					_out->write(_to);
				}
			}
			template <class _Derived, typename _Info>
			void replace(OutStream<_Derived, Component, _Info> * _out, Ref _tar, Ref _to) const // NotEnoughSpaceException
			{
				replace<Ref>(_out, _tar, _to);
			}
			template <class _Derived, typename _Info>
			void replace(OutStream<_Derived, Component, _Info> * _out, const InternalComponent &_tar, const InternalComponent &_to) const // NotEnoughSpaceException
			{
				replace<Ref>(_out, _tar, _to);
			}


			struct SplitIteratorEnd
			{
			};

			template <typename NIDDLE, typename Iterator>
			class SplitIterableCommon
			{
			private:
				const This * const m_ptr;
				const NIDDLE m_chr;

			public:
				SplitIterableCommon(const This * ptr, const NIDDLE &chr) noexcept
					: m_ptr(ptr), m_chr(chr)
				{
				}
				Iterator begin() const noexcept
				{
					return Iterator(Ref(m_ptr->begin(), m_ptr->end()), m_chr);
				}
				Iterator end() const noexcept
				{
					return Iterator();
				}
			};
			
			class SplitIterator
			{
			private:
				Ref m_ref;
				Ref m_next;
				Component m_chr;

			public:
				SplitIterator() = default;
				SplitIterator(Ref ref, Component chr) noexcept : m_chr(chr)
				{
					m_ref = ref;
					m_next = ref.find_e(m_chr);
				}
				bool operator !=(const SplitIterator & o) const noexcept
				{
					return !m_ref.empty();
				}
				bool operator ==(const SplitIterator & o) const noexcept
				{
					return m_ref.empty();
				}
				SplitIterator& operator ++() noexcept
				{
					m_ref = m_next;
					if (m_ref.empty())
						return *this;
					m_ref++;
					m_next = m_ref.find_e(m_chr);
					return *this;
				}
				SplitIterator operator ++(int) noexcept
				{
					SplitIterator old = *this;
					++*this;
					return old;
				}
				Ref operator *() const noexcept
				{
					return m_ref.cut(m_next);
				}
			};
			
			class TextSplitIterator
			{
			private:
				Ref m_ref;
				Ref m_next;
				Ref m_chr;

			public:
				TextSplitIterator() = default;
				TextSplitIterator(Ref ref, Ref chr) noexcept : m_chr(chr)
				{
					m_ref = ref;
					m_next = ref.find_e(m_chr);
				}
				bool operator !=(const TextSplitIterator & o) const noexcept
				{
					return !m_ref.empty();
				}
				bool operator ==(const TextSplitIterator & o) const noexcept
				{
					return m_ref.empty();
				}
				TextSplitIterator& operator ++() noexcept
				{
					m_ref = m_next;
					if (m_ref.empty())
						return *this;
					m_ref += m_chr.size();
					m_next = m_ref.find_e(m_chr);
					return *this;
				}
				TextSplitIterator operator ++(int) noexcept
				{
					TextSplitIterator old = *this;
					++*this;
					return old;
				}
				Ref operator *() const noexcept
				{
					return m_ref.cut(m_next);
				}
			};
			
			class ReverseSplitIterator
			{
			private:
				Ref m_ref;
				Ref m_next;
				Component m_chr;

			public:
				ReverseSplitIterator() = default;
				ReverseSplitIterator(Ref ref, Component chr) noexcept : m_chr(chr)
				{
					m_ref = ref;
					m_next = ref.end_find_r(m_chr);
					if (m_next.end() == nullptr)
						m_next = m_ref.beginIndex();
				}
				bool operator !=(const ReverseSplitIterator & o) const noexcept
				{
					return !m_ref.empty();
				}
				bool operator ==(const ReverseSplitIterator & o) const noexcept
				{
					return m_ref.empty();
				}
				ReverseSplitIterator& operator ++() noexcept
				{
					m_ref = m_next;
					if (m_ref.empty())
						return *this;
					m_ref.setEnd(m_ref.end() - 1);
					m_next = m_ref.end_find_r(m_chr);
					if (m_next.end() == nullptr)
						m_next = m_ref.beginIndex();
					return *this;
				}
				ReverseSplitIterator operator ++(int) noexcept
				{
					SplitIterator old = *this;
					++*this;
					return old;
				}
				Ref operator *() const noexcept
				{
					return Ref(m_next.end(), m_ref.end());
				}
			};

			class ReverseTextSplitIterator
			{
			private:
				Ref m_ref;
				Ref m_next;
				Ref m_chr;

			public:
				ReverseTextSplitIterator() = default;
				ReverseTextSplitIterator(Ref ref, Ref chr) noexcept : m_chr(chr)
				{
					m_ref = ref;
					m_next = ref.end_find_r(m_chr);
					if (m_next.end() == nullptr)
						m_next = m_ref.beginIndex();
				}
				bool operator !=(const ReverseTextSplitIterator & o) const noexcept
				{
					return !m_ref.empty();
				}
				bool operator ==(const ReverseTextSplitIterator & o) const noexcept
				{
					return m_ref.empty();
				}
				ReverseTextSplitIterator& operator ++() noexcept
				{
					m_ref = m_next;
					if (m_ref.empty()) return *this;
					m_ref.setEnd(m_ref.end() - m_chr.size());
					m_next = m_ref.end_find_r(m_chr);
					if (m_next.end() == nullptr)
						m_next = m_ref.beginIndex();
					return *this;
				}
				ReverseTextSplitIterator operator ++(int) noexcept
				{
					TextSplitIterator old = *this;
					++*this;
					return old;
				}
				Ref operator *() const noexcept
				{
					return Ref(m_next.end(), m_ref.end());
				}
			};
			using SplitIterable = SplitIterableCommon<Component, SplitIterator>;
			using TextSplitIterable = SplitIterableCommon<Ref, TextSplitIterator>;
			using ReverseSplitIterable = SplitIterableCommon<Component, ReverseSplitIterator>;
			using ReverseTextSplitIterable = SplitIterableCommon<Ref, ReverseTextSplitIterator>;

			SplitIterable splitIterable(const InternalComponent &chr) const noexcept
			{
				return SplitIterable(this, chr);
			}
			TextSplitIterable splitIterable(Ref chr) const noexcept
			{
				return TextSplitIterable(this, chr);
			}
			ReverseSplitIterable reverseSplitIterable(const InternalComponent &chr) const noexcept
			{
				return ReverseSplitIterable(this, chr);
			}
			ReverseTextSplitIterable reverseSplitIterable(Ref chr) const noexcept
			{
				return ReverseTextSplitIterable(this, chr);
			}

			size_t innerMaxSize() noexcept
			{
				const Component * iter = begin();
				const Component * iend = end();
				if (iter == iend) return 0;

				size_t sz = iter->size();
				iter++;
				while (iter != iend)
				{
					size_t nsz = iter->size();
					if (nsz > sz) sz = nsz;
					iter++;
				}
				return sz;
			}
			template <class Converter> Array<Array<typename Converter::Component>> split(const InternalComponent &chr)
			{
				using C2 = typename Converter::Component;
				if (size() == 0) return nullptr;
				size_t scolen = count(chr) + 1;
				Array<Array<C2>> arr(scolen);
				Array<C2>* scores = arr.begin();
				Ref read(begin(), end());
				Ref readi;
				while ((readi = read.readto(chr)) != nullptr)
				{
					(scores++)->copy(Converter(readi));
				}
				scores->copy(Converter(read));
				return arr;
			}
			template <class Converter> Array2D<Array<typename Converter::Component>> split(const InternalComponent &chr, const InternalComponent &chrin)
			{
				using C2 = typename Converter::Component;
				if (size() == 0) return nullptr;
				size_t ylen = count(chr);
				Array2D<Array<C2>> arr(ylen + 1);

				Array<Array<C2>>* inarray = arr.begin();
				Ref read(begin(), end());

				for (size_t y = 0; y<ylen; y++)
				{
					*inarray++ = read.readto(chr).template split<Converter>(chrin);
				}

				*inarray = read.template split<Converter>(chrin);
				return arr;
			}
			Array<Alc> split(const InternalComponent &chr)
			{
				return split<Ref>(chr);
			}
			Array2D<Alc> split(const InternalComponent &chr, const InternalComponent &chrin)
			{
				return split<Ref>(chr, chrin);
			}

			ReverseIterable<Component> reverseIterable() noexcept
			{
				return ReverseIterable<Component>(begin(), end());
			}
			ReverseIterable<const Component> reverseIterable() const noexcept
			{
				return ReverseIterable<const Component>(begin(), end());
			}


			Ref operator +(intp n) const noexcept
			{
				return Ref(begin() + n, end());
			}
			Ref operator -(intp n) const noexcept
			{
				return Ref(begin() - n, end());
			}
			intp operator -(const Component * ptr) const noexcept
			{
				return begin() - ptr;
			}
			friend intp operator -(const Component * ptr, const MemBuffer& ori) noexcept
			{
				return ptr - ori.begin();
			}
			template <typename _Derived, bool _szable, bool _readonly, typename _Parent> 
			intp operator -(const MemBuffer<_Derived, BufferInfo<Component, true, _szable, _readonly, _Parent>>& ptr) const noexcept
			{
				return begin() - ptr.begin();
			}
			intp operator -(const MemBuffer& ptr) const noexcept
			{
				return begin() - ptr.begin();
			}
		};

		template <typename Derived, typename Info>
		class WMemBuffer :public MemBuffer<Derived, Info>
		{
			CLASS_HEADER(WMemBuffer, MemBuffer<Derived, Info>);
		public:
			INHERIT_COMPONENT();

			using Super::Super;
			using Super::begin;
			using Super::end;
			using Super::size;
			using Super::cut;
			using Super::subarr;
			using Super::subarray;

			WRef cut(const ComponentRef* end) noexcept
			{
				_assert(contains_ptr(end));
				return WRef(begin(), const_cast<ComponentRef*>(end));
			}
			WRef cut(Ref _v) noexcept
			{
				return cut(_v.begin());
			}
			WRef cut(size_t _len) noexcept
			{
				return cut(math::min(begin() + _len, end()));
			}
			WRef subarr(size_t _left, size_t _count) noexcept
			{
				return WRef(begin() + _left, end()).cut(_count);
			}
			WRef subarr(size_t _left) noexcept
			{
				return WRef(begin() + _left, end());
			}
			WRef subarray(size_t _left) noexcept
			{
				return WRef(begin() + _left, end());
			}
			WRef subarray(size_t _left, size_t _right) noexcept
			{
				_assert(_left <= _right);
				return WRef(begin() + _left, begin() + _right);
			}

			void change(const InternalComponent &fchr, const InternalComponent &tchr) noexcept
			{
				_assert(!std::is_class<Component>::value);
				memm::change(begin(), fchr, tchr, size());
			}
			void fill(const InternalComponent &chr) // NotEnoughSpaceException
			{
				if (std::is_class<InternalComponent>::value)
				{
					mema::subs_fill(begin(), chr, size());
				}
				else
				{
					memm::set(begin(), chr, size());
				}
			}

			void subfill(const InternalComponent &chr, size_t sz, size_t offset) // NotEnoughSpaceException
			{
				_assert(offset + sz <= size());
				mema::subs_fill(begin() + offset, chr, sz);
			}
			void subcopy(const InternalComponent* arr, size_t sz, size_t offset) noexcept
			{
				_assert(offset + sz <= size());
				mema::subs_copy(begin() + offset, arr, sz);
			}
			void subcopy(std::initializer_list<InternalComponent> arr, size_t offset) noexcept
			{
				subcopy(arr.begin(), arr.size(), offset);
			}
		};
	}
}
