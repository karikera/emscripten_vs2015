#pragma once

#include <KR3/main.h>

namespace kr
{
	class BufferQueueWithSize;
	class BufferQueuePointer;
	class BufferQueueWithRef;

	class BufferQueue:public Bufferable<BufferQueue, BufferInfo<void, false, false, true, 
		OutStream<BufferQueue, void>
	> >
	{
		friend BufferQueueWithSize;
		friend BufferQueuePointer;
		friend BufferQueueWithRef;
	public:
		static constexpr size_t BUFFER_SIZE = 4096;
	
		struct Buffer;

		BufferQueue() noexcept;
		~BufferQueue() noexcept;
		BufferQueue(BufferQueue&& _move) noexcept;
		BufferQueue& operator = (BufferQueue&& _move) noexcept;
		void write(const void * data, size_t size) noexcept;
		void read(void * dest, size_t size) noexcept;
		bool empty() noexcept;
		void clear() noexcept;
		void skip(size_t size) noexcept;
		template <typename _Derived, typename _Info>
		void recv(InStream<_Derived, void, _Info>* src);
		template <typename _Derived, typename _Info>
		void send(OutStream<_Derived, void, _Info>* dest); // SocketException

		static void clearKeeped() noexcept;
	
	private:
		Buffer * _makeBuffer() noexcept;
		void _removeFrontNode() noexcept;
		Buffer * _axis() noexcept;

		Buffer *m_first;
		Buffer *m_last;
		size_t m_readed;
	};
	class BufferQueueWithSize :public BufferQueue
	{
		friend BufferQueuePointer;
	public:
		BufferQueueWithSize() noexcept;
		BufferQueueWithSize(BufferQueueWithSize&& _move) noexcept;
		BufferQueueWithSize& operator = (BufferQueueWithSize&& _move) noexcept;
		template <typename _Derived, typename _Info>
		void recv(InStream<_Derived, void, _Info>* src); //SocketException
		template <typename _Derived, typename _Info>
		void send(OutStream<_Derived, void, _Info>* dest); // SocketException
		void write(const void * data, size_t size) noexcept;
		void read(void * dest, size_t size) noexcept;
		void clear() noexcept;
		void skip(size_t size) noexcept;
		size_t size() noexcept;
		
	private:
		size_t m_totalSize;
	};
	class BufferQueuePointer:public InStream<BufferQueuePointer, void>
	{
	public:
		BufferQueuePointer(const BufferQueue & buf) noexcept;
		byte read(); // EofException
		void read(void * dest, size_t size); // EofException
		template <typename T> T read() // EofException
		{
			T v;
			read(&v, sizeof(v));
			return v;
		}
		void skip(size_t size); // EofException
		size_t getReadSize() noexcept;
		void clearSize() noexcept;

	private:
		BufferQueue::Buffer *m_buffer;
		size_t m_readed;
		size_t m_fullreaded;
	};

	class BufferQueueWithRef:public BufferQueue
	{
	public:
		struct Buffer;

		template <typename _Derived, typename _Info>
		void send(OutStream<_Derived, void, _Info>* dest); //SocketException

		void write(const void * data, size_t size) noexcept;
		void writeRef(const void * data, size_t size) noexcept;
		void read(void * dest, size_t size) noexcept;

	private:
		using ParentBuffer = BufferQueue::Buffer;
		using BufferQueue::recv;
	};

	struct BufferQueue::Buffer
	{
		Buffer * next;
		size_t size;
		byte buffer[BUFFER_SIZE];

		void * begin() noexcept;
		size_t capacity() const noexcept;
	};

	struct BufferQueueWithRef::Buffer
	{
		Buffer * next;
		size_t size;
		bool isReference;
		union
		{
			const byte * bufferPtr;
			byte buffer[1];
		};

		const byte * begin() const noexcept;
		size_t capacity() const noexcept;
	};


	template <typename _Derived, typename _Info>
	void BufferQueue::recv(InStream<_Derived, void, _Info>* src)
	{
		size_t readed;
		if (m_last != _axis())
		{
			Buffer* buff = m_last;
			size_t left = buff->capacity() - buff->size;
			readed = src->read(buff->buffer + buff->size, left);
			if (readed == 0) return;
			buff->size += readed;
			if (readed != left) return;
		}

		size_t wantRead;
		do
		{
			Buffer * buff = _makeBuffer();

			wantRead = buff->capacity();
			buff->size = readed = src->read(buff->buffer, wantRead);
			if (readed == 0) return;
		}
		while (readed == wantRead);
	}
	template <typename _Derived, typename _Info>
	void BufferQueue::send(OutStream<_Derived, void, _Info>* dest) // SocketException
	{
		while (!empty())
		{
			const char * data = (const char *)m_first->buffer + m_readed;
			size_t size = m_first->size - m_readed;
			dest->write(data, size);
			_removeFrontNode();
		}
	}
	template <typename _Derived, typename _Info>
	void BufferQueueWithSize::recv(InStream<_Derived, void, _Info>* src) //SocketException
	{
		size_t readed;
		if (m_last != _axis())
		{
			Buffer* buff = m_last;
			size_t left = buff->capacity() - buff->size;
			readed = src->read(buff->buffer + buff->size, left);
			if (readed == 0) return;
			buff->size += readed;
			m_totalSize += readed;
			if (readed != left) return;
		}

		size_t wantRead;
		do
		{
			Buffer * buff = _makeBuffer();
			wantRead = buff->capacity();
			buff->size = readed = src->read(buff->buffer, wantRead);
			if (readed == 0) return;
			m_totalSize += readed;
		}
		while (readed == wantRead);
	}
	template <typename _Derived, typename _Info>
	void BufferQueueWithSize::send(OutStream<_Derived, void, _Info>* dest) // SocketException
	{
		while (!empty())
		{
			Buffer * buff = m_first;
			const char * data = (const char *)buff->buffer + m_readed;
			size_t size = buff->size - m_readed;
			dest->write(data, size);
			_removeFrontNode();
		}
	}
	template <typename _Derived, typename _Info>
	void BufferQueueWithRef::send(OutStream<_Derived, void, _Info>* dest) //SocketException
	{
		while (!empty())
		{
			Buffer * buff = (Buffer*)m_first;
			const byte * data = buff->begin() + m_readed;
			size_t size = buff->size - m_readed;
			dest->write(data, size);
			_removeFrontNode();
		}
	}
}