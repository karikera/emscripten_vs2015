#pragma once

namespace kr
{
	namespace ary
	{
		namespace method
		{
			template <class Parent> class BufferIOMethod
				:public Parent
			{
				CLASS_HEADER(BufferIOMethod, Parent);
			public:
				INHERIT_ARRAY();

				using Super::_resize;
				using Super::_extend;
				using Super::_setSize;
				using Super::_realloc;
				using Super::_alloc;
				using Super::_clear;
				using Super::_shiftRight;
				using Super::Super;
				using Super::begin;
				using Super::end;
				using Super::size;
				using Super::empty;
				using Super::reserve;
				using Super::szable;
				using Super::fill;
				using Super::operator =;

				void clear() noexcept
				{
					mema::dtor(begin(), size());
					_clear();
				}
				void truncate() noexcept
				{
					this->~BufferIOMethod();
					new(this) BufferIOMethod(nullptr);
				}
				void pick(size_t i) noexcept
				{
					_assert(i < size());
					_assert(!empty());
					size_t sz = size() - 1;
					_resize(sz);

					Component* data = begin();
					Component* pos = data + i;
					Component* last = data + sz;

					if (pos == last) return;
					*pos = move(*last);
					callDestructor(last);
				}
				InternalComponent pickGet(size_t i) noexcept
				{
					_assert(i < size());
					_assert(!empty());
					size_t sz = size() - 1;
					_resize(sz);

					InternalComponent* data = (InternalComponent*)begin();
					InternalComponent* pos = data + i;
					InternalComponent* last = data + sz;

					if (pos == last)
						return move(*pos);

					InternalComponent out = move(*pos);
					*pos = move(*last);
					callDestructor(last);
					return move(out);
				}
				InternalComponent pickRandom() noexcept
				{
					return pickGet(g_random.getDword() % size());
				}
				void fill(const InternalComponent &chr, size_t sz) // NotEnoughSpaceException
				{
					if (std::is_class<InternalComponent>::value)
					{
						size_t osz = size();
						_resize(sz);
						if (sz < osz)
						{
							mema::subs_fill(begin(), chr, sz);
						}
						else
						{
							mema::subs_fill(begin(), chr, osz);
							mema::ctor_fill(begin() + osz, chr, sz - osz);
						}
					}
					else
					{
						_resize(sz);
						memm::set(begin(), chr, sz);
					}
				}
				template <typename T> void copy(const T& _copy)
				{
					bufferize_t<T, Component> buffer = _copy;
					size_t sz = buffer.size();
					resize(sz, sz + buffer.szable);
					buffer.copyTo(begin());
				}
				void copy(const Component* arr, size_t sz)
				{
					if (std::is_class<Component>::value)
					{
						size_t osz = size();
						if (sz < osz)
						{
							_resize(sz);
							mema::subs_copy(begin(), arr, sz);
						}
						else
						{
							_resize(sz);
							mema::subs_copy(begin(), arr, osz);
							mema::ctor_copy(begin() + osz, arr + osz, sz - osz);
						}
					}
					else
					{
						_resize(sz);
						memm::copy(begin(), arr, sz);
					}
				}
			
				void pop() // EofException
				{
					size_t osize = size();
					if(osize == 0)
						throw EofException();
					_resize(osize-1);
				}
				InternalComponent popGet() // EofException
				{
					size_t osize = size();
					if (osize == 0)
						throw EofException();
					osize--;
					InternalComponent out = move(((InternalComponent*)begin())[osize]);
					_resize(osize);
					return move(out);
				}

				void cut_self(const ComponentRef* newend) noexcept
				{
					_assert(begin() <= newend && newend <= end());
					_resize(newend - (ComponentRef*)begin());
				}
				void cut_self(Ref _v) noexcept
				{
					return cut_self(_v.begin());
				}
				void cut_self(size_t _len) noexcept
				{
					_len = math::min(_len, size());
					_resize(_len);
				}

				template <typename ... ARGS> 
				Component* emplace(ARGS && ... args) noexcept
				{
					size_t osize = size();
					_resize(osize+1);
					InternalComponent * comp = begin() + osize;
					new(comp) InternalComponent(move(args) ...);
					return (Component*)comp;
				}
				void insert(size_t i, const InternalComponent & value) // NotEnoughSpaceException
				{
					_assert(i <= size());
					_shiftRight(i);
					new(begin() + i) InternalComponent(value);
				}
				void insert(size_t i, Ref arr) // NotEnoughSpaceException
				{
					_assert(i <= size());
					size_t cnt = arr.size();
					mema::ctor_copy(prepareAt(i, cnt), arr.begin(), cnt);
				}
				Component* prepareAt(size_t i, size_t cnt) // NotEnoughSpaceException
				{
					_assert(i <= size());
					_shiftRight(i, cnt);
					return begin() + i;
				}
				void remove_p(Component * axis)
				{
					size_t sz = size();
					_assert((size_t)(axis - begin()) < sz);
					sz--;
					kr::mema::ctor_move_d(axis, axis + 1, begin() + sz - axis);
					_setSize(sz);
				}
				void remove(size_t i)
				{
					size_t sz = size();
					_assert(i < sz);
					Component * axis = begin() + i;
					sz--;
					kr::mema::ctor_move_d(axis, axis + 1, sz - i);
					_setSize(sz);
				}
				void remove(size_t i, size_t cnt)
				{
					size_t sz = size();
					_assert(i+cnt <= sz);
					Component * axis = begin() + i;
					kr::mema::ctor_move_d(axis, axis + cnt, sz - i - cnt);
					_setSize(sz - cnt);
				}
				InternalComponent removeGet(size_t i)
				{
					size_t sz = size();
					_assert(i < sz);
					InternalComponent * axis = (InternalComponent*)begin() + i;
					InternalComponent out = move(*axis);
					kr::mema::ctor_move_d(axis, axis + 1, sz - i - 1);
					_setSize(sz - 1);
					return move(out);
				}
				void resize(size_t nsize) // NotEnoughSpaceException
				{
					size_t sz = size();
					_resize(nsize);
					if (nsize > sz)
						mema::ctor((InternalComponent*)begin() + sz, nsize - sz);
				}
				template <typename ... ARGS>
				void initResize(size_t nsize, const ARGS & ... args) // NotEnoughSpaceException
				{
					size_t sz = size();
					_resize(nsize);
					if (nsize > sz)
					{
						Component * beg = begin();
						Component * end = beg + nsize;
						beg += sz;
						do
						{
							new(beg) Component(args ...);
							beg++;
						}
						while (beg != end);
					}
				}
				void alloc(size_t nsize) // NotEnoughSpaceException
				{
					clear();
					_resize(nsize);
					mema::ctor(begin(), nsize);
				}
				void resize(size_t nsize, size_t ncapacity) // NotEnoughSpaceException
				{
					reserve(ncapacity);
					resize(nsize);
				}
				template <typename LAMBDA>
				void removeMatch(const LAMBDA &lambda)
				{
					Component* i = begin();
					Component* to = end();

					for (;i != to; i++)
					{
						if (!lambda(*i)) continue;
						return remove_p(i);
					}
				}
				void removeMatch(const InternalComponent &value)
				{
					removeMatch([&](const InternalComponent& v)->bool { return v == value; });
				}
				template <typename LAMBDA> 
				void removeMatchAll(const LAMBDA &lambda)
				{
					Component* i = begin();
					Component* to = end();

					for (;;)
					{
						if (i == to) return;
						if (lambda(*i)) break;
						i++;
					}

					Component* moveto = i;

					callDestructor(i++);

					while (i != to)
					{
						if (lambda(*i))
						{
							callDestructor(i++);
						}
						else
						{
							new(moveto++) Component(move(*i));
							callDestructor(i++);
						}
					}

					_setSize(moveto - begin());
				}
				void removeAll(const InternalComponent &value)
				{
					removeMatchAll([&](const InternalComponent& v)->bool{ return v == value; });
				}

				template <typename S> void serialize(S &s)
				{
					s.serializeSize(this);
					for (Component & c : *this)
					{
						s & c;
					}
				}

				constexpr BufferIOMethod() noexcept = default;
				constexpr BufferIOMethod(const BufferIOMethod& _copy) = default;
				constexpr BufferIOMethod(BufferIOMethod && _mv) = default;
				BufferIOMethod(const Ref & data) noexcept // XXX: for intelisense bug
				{
					size_t sz = data.size();
					_alloc(sz, sz + szable);
					Component * beg = begin();
					mema::ctor(beg, sz);
					resize(data.copyTo(beg));
				}
				template <typename _Derived, class _Parent>
				BufferIOMethod(const Printable<_Derived, Component, _Parent> & _data) noexcept
				{
					_data.writeTo(this);
				}
				template <typename _Derived, bool szable, bool b, class _Parent>
				BufferIOMethod(const Bufferable<_Derived, BufferInfo<Component, false, szable, b, _Parent>> & _data) noexcept
				{
					size_t sz = _data.size();
					_alloc(sz, sz + szable);
					Component * beg = begin();
					mema::ctor(beg, sz);
					resize(_data.copyTo(beg));
				}
				template <typename _Derived, bool szable, bool b, class _Parent>
				BufferIOMethod(const Bufferable<_Derived, BufferInfo<AutoComponent, false, szable, b, _Parent>> & _data) noexcept
				{
					size_t sz = _data.template sizeAs<Component>();
					_alloc(sz, sz + szable);
					Component * beg = begin();
					mema::ctor(beg, sz);
					resize(_data.template copyTo<Component>(beg));
				}
				template <typename _Derived, bool a, bool b, class _Parent>
				explicit BufferIOMethod(Bufferable<_Derived, BufferInfo<Component, true, a, b, _Parent>> && _mv) noexcept
				{
					_move(_mv.begin(), _mv.size());
				}

				Component * c_str() noexcept
				{
					*this << nullterm;
					return begin();
				}

				template <typename ... ARGS>
				static Self concat(const ARGS & ... args) noexcept
				{
					Self text;
					text.prints(args ...);
					return text;
				}
			protected:
				friend typename Parent::Data;
				template <typename, class>
				friend class data::WritableForm;
			};
		}
	}
}