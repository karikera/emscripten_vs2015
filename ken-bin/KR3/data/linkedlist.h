#pragma once

#include "../main.h"

#include "iterator.h"
#include "bin_search.h"

namespace kr
{
	namespace _pri_
	{
		template <typename C>
		class ChainData;
		template <typename C, bool isAbstract>
		class ChainAbstractTest;
		template <typename C, bool isAbstract>
		class LinkedListAbstractTest;

		template <typename C, bool isclass>
		class NodeImpl;
	}

	template <typename C> class NodeLink
	{
		friend Node<C>;
		friend Chain<C>;
		friend _pri_::ChainData<C>;
		friend LinkedList<C>;

	private:
		Node<C>* m_next;
		Node<C>* m_previous;

		void _detach() noexcept
		{
			m_previous->m_next = m_next;
			m_next->m_previous = m_previous;
			m_previous = nullptr;
			m_next = nullptr;
		}

	public:
		NodeLink() noexcept
		{
			m_next = nullptr;
			m_previous = nullptr;
		}
		Node<C> * getNext() noexcept
		{
			return static_cast<Node<C>*>(m_next);
		}
		Node<C> * getPrevious() noexcept
		{
			return static_cast<Node<C>*>(m_previous);
		}

	};

	template <typename C>
	class _pri_::NodeImpl<C, true>:
		public C, public NodeLink<C>
	{
	public:
		NodeImpl() = default;
		NodeImpl(const C & _copy)
			:C(_copy)
		{
		}
		NodeImpl(C && _move)
			:C((C&&)_move)
		{
		}
		using C::C;

	};
	template <typename C>
	class _pri_::NodeImpl<C, false>
	{
	public:
		NodeImpl() = default;
		NodeImpl(const C & _copy)
			:m_data(_copy)
		{
		}
		NodeImpl(C && _move)
			:m_data(move(_move))
		{
		}
		operator C&() noexcept
		{
			return m_data;
		}
	private:
		C m_data;
	};

	template <typename C> class Node :
		public _pri_::NodeImpl<C, std::is_class<C>::value>
	{
		friend NodeLink<C>;
		friend Chain<C>;
		friend _pri_::ChainData<C>;
		friend LinkedList<C>;
		using Super = _pri_::NodeImpl<C, std::is_class<C>::value>;

	protected:
		using Super::m_next;
		using Super::m_previous;

	public:
		Node() = default;
		using Super::Super;
		Node(const C & _copy)
			:Super(_copy)
		{
		}
		Node(C && _move)
			:Super((C&&)_move)
		{
		}

	private:
		void _insertAfter(Node* node) noexcept
		{
			if(m_next != nullptr)
			{
				m_next->m_previous = node;
				node->m_next = m_next;
			}
			m_next = node;
			node->m_previous = this;
		}
		void _insertBefore(Node* node) noexcept
		{
			if(m_previous != nullptr)
			{
				m_previous->m_next = node;
				node->m_previous = m_previous;
			}
			m_previous = node;
			node->m_next = this;
		}
		size_t _cut() noexcept
		{
			Node *t = this;
			int len = 0;
			do
			{
				Node *s = t;
				t = s->m_next;
				delete s;
				len++;
			}
			while(t != nullptr);
			return len;
		}
	};;

	namespace _pri_
	{
		template <typename C>
		class ChainData :public TIterable<Chain<C>>
		{
		public:
			using Node = kr::Node<C>;
			ChainData() noexcept
			{
				m_axis.m_previous = m_axis.m_next = static_cast<Node*>(&m_axis);
			}
			~ChainData() noexcept
			{
				Node* p = m_axis.m_next;
				while (p != &m_axis)
				{
					Node* t = p;
					p = p->m_next;
					delete t;
				}
			}

		protected:
			NodeLink<C> m_axis;
		};

		template <typename C, bool isAbstract>
		class ChainAbstractTest:public ChainData<C>
		{
		public:
			C popGet() noexcept
			{
				Chain<C>* pthis = static_cast<Chain<C>*>(this);
				kr::Node<C> * last = pthis->detachLast();
				C out = move(*static_cast<C*>(last));
				delete last;
				return move(out);
			}
		};

		template <typename C>
		class ChainAbstractTest<C, true> :public ChainData<C>
		{
		public:
		};

		template <typename C, bool isAbstract>
		class LinkedListAbstractTest :public Chain<C>
		{
		public:
			C popGet() noexcept
			{
				m_size--;
				return Chain<C>::popGet();
			}

		protected:
			size_t m_size;
		};

		template <typename C>
		class LinkedListAbstractTest<C, true> :public Chain<C>
		{
		public:

		protected:
			size_t m_size;
		};
	}

	// 길이를 가지지 않은 링크드 리스트이다.
	template <typename C> class Chain:public _pri_::ChainAbstractTest<C, std::is_abstract<C>::value>
	{
		using Super = _pri_::ChainAbstractTest<C, std::is_abstract<C>::value>;
	protected:
		using Super::m_axis;

	public:
		using Node = kr::Node<C>;
		using NodeLink = kr::NodeLink<C>;

		class Iterator:public TIterator<Iterator, Node>
		{
		public:
			INHERIT_ITERATOR(TIterator<Iterator, Node>);

		protected:
			using Super::m_pt;

		public:
			Iterator& operator ++() noexcept
			{
				m_pt = static_cast<Node*>(m_pt)->m_next;
				return *this;
			}
			Iterator& operator --() noexcept
			{
				m_pt = static_cast<Node*>(m_pt)->m_previous;
				return *this;
			}
		};

		template <typename... ARGS> Node* create(ARGS && ... args) noexcept
		{
			Node * node = _new Node(args ...);
			return node;
		}
		void reaxis(Node* node) noexcept
		{
			m_axis._detach();

			Node * pnode = node->m_previous;
			m_axis.m_previous = pnode;
			pnode->m_next = static_cast<Node*>(&m_axis);
			m_axis.m_next = node;
			node->m_previous = static_cast<Node*>(&m_axis);
		}
		void attach(Node* node) noexcept
		{
			m_axis.m_previous->m_next = node;
			node->m_next = static_cast<Node*>(&m_axis);
			node->m_previous = static_cast<Node*>(m_axis.m_previous);
			m_axis.m_previous = node;
		}
		void attachFirst(Node* node) noexcept
		{
			if(m_axis.m_previous == &m_axis)
			{
				attach(node);
			}
			else
			{
				m_axis.m_next->m_previous = node;
				node->m_next = m_axis.m_next;
				m_axis.m_next = node;
				node->m_previous = static_cast<Node*>(&m_axis);
			}
		}
		void attachAfter(Node* node, Node* pAxis) noexcept
		{
			pAxis->_insertAfter(node);
		}
		void attachBefore(Node* node, Node* pAxis) noexcept
		{
			pAxis->_insertBefore(node);
		}
		void sortAttach(Node* _node) noexcept
		{
			using KeyType = decltype(Key<C>::getKey(*(C*)0));

			C & insertValue = *static_cast<C*>(_node);
			KeyType insertKey = Key<C>::getKey(insertValue);

			Node * p = m_axis.m_next;
			for (; p != &m_axis; p = p->m_next)
			{
				C & value = *static_cast<C*>(p);
				KeyType compareKey = Key<C>::getKey(value);
				if (Comparator<KeyType>::compare(insertKey, compareKey) > 0)
					continue;
				p->_insertBefore(_node);
				return;
			}

			attach(_node);
		}
		Node* push(const C& data)
		{
			Node* node = _new Node(data);
			attach(node);
			return node;
		}
		Node* push(C&& data)
		{
			Node* node = _new Node(move(data));
			attach(node);
			return node;
		}
		Node* insertFirst(const C& data)
		{
			Node* node = _new Node(data);
			attachFirst(node);
			return node;
		}
		Node* insertFirst(C&& data)
		{
			Node* node = _new Node(move(data));
			attachFirst(node);
			return node;
		}
		Node* insertAfter(const C& data, Node* pAxis)
		{
			Node* node = _new Node(data);
			attachAfter(node);
			return node;
		}
		Node* insertAfter(C&& data, Node* pAxis)
		{
			Node* node = _new Node(move(data));
			attachAfter(node, pAxis);
			return node;
		}
		Node* insertBefore(const C& data, Node* pAxis)
		{
			Node* node = _new Node(data);
			attachAfter(node);
			return node;
		}
		Node* insertBefore(C&& data, Node* pAxis)
		{
			Node* node = _new Node(move(data));
			attachAfter(node, pAxis);
			return node;
		}
		Node* sortInsert(const C & _data) noexcept
		{
			Node* node = _new Node(_data);
			sortAttach(node);
			return node;
		}
		Node* sortInsert(C && _data) noexcept
		{
			Node* node = _new Node(move(_data));
			sortAttach(node);
			return node;
		}
		void detach(Node* node) noexcept
		{
			node->_detach();
		}
		Node* detachFirst() noexcept
		{
			_assert(!empty());
			Node * node = m_axis.m_next;
			node->_detach();
			return node;
		}
		Node* detachLast() noexcept
		{
			_assert(!empty());
			Node * node = m_axis.m_previous;
			node->_detach();
			return node;
		}
		void remove(Node* node) noexcept
		{
			detach(node);
			delete node;
		}
		Iterator remove(Iterator _node) noexcept
		{
			Iterator ret = _node;
			++ret;
			remove((Node*)_node);
			return ret;
		}
		typename Iterator::Reverse remove(typename Iterator::Reverse _node) noexcept
		{
			typename Iterator::Reverse ret = _node;
			++ret;
			remove((Node*)_node);
			return ret;
		}
		void removeFirst() noexcept
		{
			delete detachFirst();
		}
		void pop() noexcept
		{
			delete detachLast();
		}
		size_t cut(Node * pAxis) noexcept
		{
			m_axis.m_previous->m_next = nullptr;
			m_axis.m_previous = pAxis->m_previous;
			if(!empty()) m_axis.m_previous->m_next = &m_axis;
			return m_axis.m_previous->m_next->_cut();
		}
		size_t cutR(Node * pAxis) noexcept
		{
			m_axis.m_next = pAxis->m_next;
			if(m_axis.m_next) 
				m_axis.m_next->m_previous = static_cast<Node*>(this);

			C *t = pAxis->m_previous;

			int len = 0;
			while(t)
			{
				C* s = t;
				t = s->m_previous;
				delete s;
				len++;
			}
			return len;
		}
		void clear() noexcept
		{
			Node* p = m_axis.m_next;
			while(p != &m_axis)
			{
				Node* t =p;
				p = p->m_next;
				delete t;
			}
			m_axis.m_previous = m_axis.m_next = static_cast<Node*>(&m_axis);
		}

		Node * indexOfNode(size_t n) noexcept
		{
			Node* p = m_axis.m_next;
			if (p == &m_axis)
				return nullptr;
			while (n--)
			{
				p = p->m_next;
				if (p == &m_axis)
					return nullptr;
			}
			return p;
		}
		const Node * indexOfNode(size_t n) const noexcept
		{
			const Node* p = m_axis.m_next;
			if (p == &m_axis)
				return nullptr;
			while (n--)
			{
				p = p->m_next;
				if (p == &m_axis)
					return nullptr;
			}
			return p;
		}

		C& front() noexcept
		{
			return *static_cast<C*>(m_axis.m_next);
		}
		C& back() noexcept
		{
			return *static_cast<C*>(m_axis.m_previous);
		}
		const C& front() const noexcept
		{
			return *static_cast<C*>(m_axis.m_next);
		}
		const C& back() const noexcept
		{
			return *static_cast<C*>(m_axis.m_previous);
		}
		C& indexOf(size_t n) noexcept
		{
			Node * node = indexOfNode(n);
			_assert(node != nullptr);
			return *static_cast<C*>(node);
		}
		const C& indexOf(size_t n) const noexcept
		{
			const Node * node = indexOfNode(n);
			_assert(node != nullptr);
			return *static_cast<const C*>(node);
		}

		bool operator ==(const Chain& node) const noexcept
		{
			return this == &node;
		}
		bool operator !=(const Chain& node) const noexcept
		{
			return this != &node;
		}

		bool empty() const noexcept
		{
			return m_axis.m_next == &m_axis;
		}
		Iterator begin() noexcept
		{
			return m_axis.m_next;
		}
		Iterator end() noexcept
		{
			return static_cast<Node*>(&m_axis);
		}
		typename Iterator::Reverse rbegin() noexcept
		{
			return static_cast<Node*>(m_axis.m_previous);
		}
		typename Iterator::Reverse rend() noexcept
		{
			return static_cast<Node*>(&m_axis);
		}

	};

	template <typename C> class LinkedList:public _pri_::LinkedListAbstractTest<C, std::is_abstract<C>::value>
	{
		using Super = _pri_::LinkedListAbstractTest<C, std::is_abstract<C>::value>;
	protected:
		using Super::m_size;
		using Super::m_axis;

	public:
		using typename Super::Node;
		using typename Super::NodeLink;
		using typename Super::Iterator;

		LinkedList() noexcept
		{
			m_size = 0;
		}
		template <typename ... ARGS> Node* create(ARGS ... args) noexcept
		{
			Node * node = _new Node(args ...);
			attach(node);
			return node;
		}
		void attach(Node* node) noexcept
		{
			m_size++;
			Super::attach(node);
		}
		void attachFirst(Node * node) noexcept
		{
			m_size++;
			Super::attachFirst(node);
		}
		void attachAfter(Node* node, Node* pAxis) noexcept
		{
			m_size++;
			Super::attachBefore(node, pAxis);
		}
		void attachBefore(Node* node, Node* pAxis) noexcept
		{
			m_size++;
			Super::attach(node, pAxis);
		}
		void sortAttach(Node* _node) noexcept
		{
			Super::sortAttach(_node);
			m_size++;
		}
		Node* push(const C& data) noexcept
		{
			Node * p = Super::push(data);
			m_size++;
			return p;
		}
		Node* push(C&& data)
		{
			Node * p = Super::push(move(data));
			m_size++;
			return p;
		}
		Node* insertFirst(const C& data)
		{
			Node * p = Super::insertFirst(data);
			m_size++;
			return p;
		}
		Node* insertFirst(C&& data)
		{
			Node * p = Super::insertFirst(move(data));
			m_size++;
			return p;
		}
		Node* insertAfter(const C& data, Node* pAxis)
		{
			Node * p = Super::push(data, pAxis);
			m_size++;
			return p;
		}
		Node* insertAfter(C&& data, Node* pAxis)
		{
			Node * p = Super::insertAfter(move(data), pAxis);
			m_size++;
			return p;
		}
		Node* insertBefore(const C& data, Node* pAxis)
		{
			Node * p = Super::insertBefore(data, pAxis);
			m_size++;
			return p;
		}
		Node* insertBefore(C&& data, Node* pAxis)
		{
			Node * p = Super::insertBefore(move(data), pAxis);
			m_size++;
			return p;
		}
		Node* sortInsert(const C & _data) noexcept
		{
			Node * p = Super::sortInsert(_data);
			m_size++;
			return p;
		}
		Node* sortInsert(C && _data) noexcept
		{
			Node * p = Super::sortInsert(move(_data));
			m_size++;
			return p;
		}
		Node * detachFirst() noexcept
		{
			m_size--;
			return Super::detachFirst();
		}
		Node * detachLast() noexcept
		{
			m_size--;
			return Super::detachLast();
		}
		
		void detach(Node * node) noexcept
		{
			m_size--;
			Super::detach(node);
		}
		void remove(Node* node) noexcept
		{
			m_size--;
			Super::remove(node);
		}
		Iterator remove(Iterator node) noexcept
		{
			m_size--;
			return Super::remove(node);
		}
		typename Iterator::Reverse remove(typename Iterator::Reverse node) noexcept
		{
			m_size--;
			return Super::remove(node);
		}
		void removeFirst() noexcept
		{
			m_size--;
			Super::removeFirst();
		}
		void pop() noexcept
		{
			m_size--;
			Super::pop();
		}
		void cut(Node * pAxis) noexcept
		{
			m_size -= Super::cut(pAxis);
		}
		void cutR(Node * pAxis) noexcept
		{
			m_size -= Super::cutR(pAxis);
		}
		void resize(size_t length) noexcept
		{
			if(m_size <= length) return;
			(*this)[m_size - length].cutR();
		}
		void resizeR(size_t length) noexcept
		{
			if(m_size <= length) return;
			(*this)[m_size - length].cutR();
		}
		void clear() noexcept
		{
			m_size = 0;
			Super::clear();
		}
		void check()
		{
		#ifndef NDEBUG
			size_t count = 0;
			NodeLink * p = &m_axis;
			do
			{
				_assert(p->m_next->m_previous == p);
				p = p->m_next;
				count++;
			}
			while(p != &m_axis);
			_assert(count-1 == size());
		#endif
		}

		size_t size() const noexcept
		{
			return m_size;
		}

		using Super::front;
		using Super::back;
		using Super::indexOf;
		using Super::indexOfNode;
		using Super::operator ==;
		using Super::operator !=;
		using Super::empty;
		using Super::begin;
		using Super::end;
		using Super::rbegin;
		using Super::rend;
		using Super::reverse;
		using Super::getReverseIterable;

		template <typename F> void sort(F cmpfunc) noexcept
		{
			Sorter<F> sort(cmpfunc, &m_axis);
			Node * first = sort.sort();
			Node * last = first->m_previous;
			m_axis.m_next = first;
			m_axis.m_previous = last;
			first->m_previous = last->m_next = static_cast<Node*>(&m_axis);			
		}
		template <typename F, typename V> Node* search(F cmpfunc, V value) noexcept
		{
			Node* p = m_axis.m_next;
			while(p != &m_axis)
			{
				if(cmpfunc(p, value)) return p;
				p = p->m_next;
			}
			return nullptr;
		}

	protected:

		template <typename F> class Sorter
		{
		public:
			Sorter(F func, NodeLink * axis) noexcept
				:m_cmpfunc(move(func)), m_list(axis->m_next)
			{
				Node * last = axis->m_previous;
				last->m_next = nullptr;
				m_list->m_previous = last;
			}
			Node* sort() noexcept
			{
				Node* p;
				try
				{
					p = _pop();
				}
				catch(EofException&)
				{
					return nullptr;
				}
				try
				{
					int level = 0;
					for(;;)
					{
						p = _merge(p, _pickLevel(level++));
					}
				}
				catch(EofException&)
				{
					return p;
				}
			}

		private:
			F m_cmpfunc;
			Node* m_list;

			Node* _pop() // EofException
			{
				Node*  p = m_list;
				if(p == nullptr) throw EofException();
				m_list = p->m_next;
				if(m_list != nullptr)
				{
					m_list->m_previous = p->m_previous;
				}
				p->m_next = nullptr;
				p->m_previous = p;
				return p;
			}
			Node* _pickLevel(uint level) // EofException
			{
				if(level == 0)
				{
					return _pop();
				}
				else
				{
					level--;
					Node *a, *b;
					a = _pickLevel(level);
					try { b = _pickLevel(level); }
					catch(EofException&){ return a; }
					return _merge(a, b);
				}
			}
			// 첫번째 노드의 pPrevious에 마지막 노드를 넣는다.
			// 마지막 노드의 pNext에는 nullptr을 넣는다.
			Node* _merge(Node *a, Node *b) noexcept
			{
				Node * pOut;
				Node * pLast;
				Node * alast = a->m_previous;
				Node * blast = b->m_previous;

				if(m_cmpfunc(a, b) > 0)
				{
					pOut = pLast = b;
					b = b->m_next;
					if(b == nullptr)
					{
						goto _connect;
					}
				}
				else
				{
					pOut = pLast = a;
					a = a->m_next;
					if(a == nullptr)
					{
						a = b;
						alast = blast;
						goto _connect;
					}
				}

				for(;;)
				{
					if(m_cmpfunc(a, b) > 0)
					{
						pLast->m_next = b;
						b->m_previous = pLast;
						pLast = b;
						b = b->m_next;
						if(b == nullptr) break;
					}
					else
					{
						pLast->m_next = a;
						a->m_previous = pLast;
						pLast = a;
						a = a->m_next;
						if(a == nullptr)
						{
							a = b;
							alast = blast;
							break;
						}
					}
				}

			_connect:
				pLast->m_next = a;
				pOut->m_previous = alast;
				a->m_previous = pLast;
				return pOut;
			}
		};
	};

}