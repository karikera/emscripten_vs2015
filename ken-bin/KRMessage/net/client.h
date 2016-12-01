#pragma once

#include <KRUtil/bufferqueue.h>
#include <KRUtil/serializer.h>
#include "wsevent.h"

namespace kr
{
	class Client
	{
	public:
		Client() noexcept;
		Client(Socket* socket) noexcept;
		~Client() noexcept;
		Client(const Client&) = delete;
		Client& operator =(const Client&) = delete;
		Ipv4Address getIpAddress() noexcept;
		SocketEventHandle * getSocketEvent() noexcept;
		void setSocket(Socket * socket) noexcept;
		void moveNetwork(Client * watcher) noexcept;
		void write(Buffer buff) noexcept;
		void writeRef(Buffer buff) noexcept;

		// return: if not exists remaining then true
		bool flush(); // SocketException
		void close() noexcept;
		bool isClosed() noexcept;
		FNetworkEvent enumNetwork(); // DisconnectException
		void processEvent(FNetworkEvent ev); // SocketException, DisconnectException
		void processRead(); // SocketException

		SocketEventHandle* getEventHandle() noexcept;
		void clearReadRemaining() noexcept;
		size_t getReadRemaining() noexcept;
		BufferQueuePointer getReadPointer() noexcept;
		void moveReadPointer(BufferQueuePointer pointer) noexcept;
		void moveReadPointer(size_t size) noexcept;

		template <typename T>
		T serializeRead()  // EofException
		{
			BufferQueuePointer p = getReadPointer();
			Deserializer<BufferQueuePointer> szer = &p;			

			T dest;
			szer >> dest;
			moveReadPointer(p);
			return dest;
		}

		template <typename T>
		void serializeWrite(const T& data) // kr::EofException
		{
			size_t sz = SerializedSizer::getSize<T>(data);
			TBuffer buffer((size_t)0, sz);
			Serializer<TBuffer> szer = &buffer;
			szer << const_cast<T&>(data);
			write((Buffer)buffer);
		}


	private:
		SocketEvent m_event;
		Socket* m_socket;
		BufferQueueWithSize m_readbuf;
		BufferQueueWithRef m_writebuf;
		bool m_closed : 1;
	};


}