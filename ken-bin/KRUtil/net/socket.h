#pragma once

#include <KR3/main.h>
NEED_SOCKET

#include "ipaddr.h"

namespace kr
{
	class SocketException
	{
	public:
		SocketException() noexcept;
		SocketException(int err) noexcept;
		operator int() const noexcept;

	private:
		const int m_error;
	};
	class SocketWriteFailException:public SocketException
	{
	public:
		SocketWriteFailException(Buffer data) noexcept;

		ABuffer unsended;
	};

	class Socket:public io::Streamable<Socket>
	{
	public:
		struct Init
		{
			Init() noexcept;
			~Init() noexcept;
		};
		Socket() noexcept = delete;
		static Socket* create() noexcept;
		static void operator delete(ptr socket) noexcept;

		void connect(Ipv4Address add,word port); // SocketException
		void connect(pcstr url,word port); // SocketException
		void connect(pcwstr url,word port); // SocketException
		void connectAsync(Ipv4Address add,word port); // SocketException
		void setTimeout(long sec,long usec) noexcept;
		void open(word port); // SocketException
		Ipv4Address getIpAddress() noexcept;
		Socket* accept(); // SocketException
		void writeImpl(cptr binary, size_t len); // SocketException
		size_t readImpl(ptr binary, size_t len); // SocketException, EofException
		size_t readFully(ptr binary, size_t len); // SocketException, EofException

		template <typename CHR>
		static Ipv4Address findIp(const CHR * url); // SocketException
		static Ipv4Address getCurrentIpAddress(); // SocketException
	};

	namespace io
	{
		template <typename C>
		using SocketStream = RetypeStream<Socket, C>;
	}

}
