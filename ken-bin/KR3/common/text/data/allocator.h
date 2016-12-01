#pragma once

#include <KR3/main.h>

namespace kr
{
	namespace ary
	{
		namespace data
		{
			template <typename C, size_t offset>
			class SAllocator
			{
			private:
				using InternalComponent = internal_component_t<C>;
				static constexpr size_t align = alignof(InternalComponent);

			public:
				static InternalComponent * _mem_alloc_bytes(size_t sz) noexcept
				{
					StackAllocator * allocator = StackAllocator::getThreadAllocator();
					if (align <= 1)
					{
						return (InternalComponent*)((pbyte)allocator->allocate(sz + offset) + offset);
					}
					else if (offset == 0)
					{
						return (InternalComponent*)((pbyte)allocator->allocate(sz, align) + offset);
					}
					else
					{
						return (InternalComponent*)((pbyte)allocator->allocate(sz, align, offset) + offset);
					}
				}
				static InternalComponent * _mem_alloc(size_t sz) noexcept
				{
					return _mem_alloc_bytes(sz * sizeof(InternalComponent));
				}
				static size_t _mem_msize_bytes(InternalComponent * p) noexcept
				{
					StackAllocator * allocator = StackAllocator::getThreadAllocator();
					return (allocator->msize((pbyte)p - offset) - offset);
				}
				static size_t _mem_msize(InternalComponent * p) noexcept
				{
					return _mem_msize_bytes(p) / sizeof(InternalComponent);
				}
				static bool _mem_expand(InternalComponent * p, size_t sz) // NotEnoughSpaceException
				{
					StackAllocator * allocator = StackAllocator::getThreadAllocator();
					if (allocator->isLastBlock((pbyte)p - offset))
					{
						return allocator->expand((pbyte)p - offset, sz * sizeof(InternalComponent) + offset);
					}
					else
					{
						if (_mem_msize_bytes(p) < sz * sizeof(InternalComponent))
							throw NotEnoughSpaceException();
						return true;
					}
				}
				static void _mem_reduce(InternalComponent * p, size_t sz) noexcept
				{
					_assert(sz <= _mem_msize(p));
					StackAllocator * allocator = StackAllocator::getThreadAllocator();
					if (!allocator->isLastBlock((pbyte)p - offset)) return;
					allocator->expand((pbyte)p - offset, sz * sizeof(InternalComponent) + offset);
				}
				static void _mem_free(InternalComponent * p) noexcept
				{
					StackAllocator * allocator = StackAllocator::getThreadAllocator();
					allocator->free((pbyte)p - offset);
				}

				template <typename LAMBDA>
				static InternalComponent* _obj_move(InternalComponent * from, size_t ncap, const LAMBDA & ctor_move_d) // NotEnoughSpaceException
				{
					StackAllocator * allocator = StackAllocator::getThreadAllocator();
					pbyte allocpoint = (pbyte)from - offset;
					if (!allocator->isLastBlock(allocpoint))
						throw NotEnoughSpaceException();

					ncap *= sizeof(InternalComponent);

					StackAllocator::Node * node = allocator->getLastNode();
					InternalComponent* narr = (InternalComponent*)((pbyte)allocator->allocateWithNewNode(ncap, align, offset) + offset);
					ctor_move_d(narr);
					node->free(allocpoint);
					return narr;
				}
			};
			template <typename C, size_t offset>
			class DAllocator
			{
			private:
				using InternalComponent = internal_component_t<C>;
				static constexpr size_t align = alignof(InternalComponent);

			public:
				static InternalComponent * _mem_alloc_bytes(size_t sz) noexcept
				{
					if (align <= 1)
					{
						return (InternalComponent*)((pbyte)kr_alloc(sz + offset) + offset);
					}
					else if (offset == 0)
					{
						return (InternalComponent*)((pbyte)kr_aligned_alloc(sz + offset, align) + offset);
					}
					else
					{
						return (InternalComponent*)((pbyte)kr_aligned_alloc(sz + offset, align, offset) + offset);
					}
				}
				static InternalComponent * _mem_alloc(size_t sz) noexcept
				{
					return _mem_alloc_bytes(sz * sizeof(InternalComponent));
				}
				static size_t _mem_msize_bytes(InternalComponent * p) noexcept
				{
					if (align <= 1)
					{
						return (kr_msize((pbyte)p - offset) - offset);
					}
					else if (offset == 0)
					{
						return (kr_aligned_msize((pbyte)p, align));
					}
					else
					{
						return (kr_aligned_msize((pbyte)p - offset, align, offset) - offset);
					}
				}
				static size_t _mem_msize(InternalComponent * p) noexcept
				{
					return _mem_msize_bytes(p) / sizeof(InternalComponent);
				}
				static bool _mem_expand(InternalComponent * p, size_t sz) noexcept
				{
					if (align <= 1)
					{
						return kr_expand((pbyte)p - offset, sz * sizeof(InternalComponent) + offset);
					}
					else
					{
						return false;
					}
				}
				static void _mem_reduce(InternalComponent * p, size_t sz) noexcept
				{
					_assert(sz <= _mem_msize(p));
				}
				static void _mem_free(InternalComponent * p) noexcept
				{
					if (align <= 1)
					{
						kr_free((pbyte)p - offset);
					}
					else
					{
						kr_aligned_free((pbyte)p - offset);
					}
				}

				template <typename LAMBDA>
				static InternalComponent* _obj_move(InternalComponent * from, size_t ncap, const LAMBDA & ctor_move_d) noexcept
				{
					InternalComponent* narr = _mem_alloc(ncap);
					ctor_move_d(narr);
					_mem_free(from);
					return narr;
				}
			};
		}
	}
}