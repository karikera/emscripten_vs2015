#pragma once

#include "../progressor.h"
#include <KRUtil/net/socket.h>
#include "wsevent.h"

namespace kr
{
	class Connection :
		public io::Streamable<Connection, void, StreamInfo<false, Progressor>>
	{
	public:
		struct Init:Socket::Init
		{
		};
		Connection(Text host, int port);
		~Connection();

		void connect(); // ThrowAbort
		void close() noexcept;
		virtual void onError(int se) = 0; // ThrowAbort

		void write(void * dest, size_t sz); // SocketException
		size_t read(void * dest, size_t size); // SocketException, DisconnectException
		template <typename T> void sendPacket(T & value) // ErrorCode
		{
			constexpr size_t opcode = SPackets::index_of<T>::value;
			static_assert(opcode <= 0xff, "packet opcode overflow");
			byte bopcode = (byte)opcode;
			serializeWrite(bopcode);
			serializeWrite(value);
		}
		template <typename T> void serializeWrite(T &value) //ErrorCode
		{
			size_t sz = SerializedSizer::getSize(value);
			TmpArray<void> buffer((size_t)0, sz);
			Serializer<TmpArray<void>> szer = &buffer;
			szer << const_cast<T&>(value);
			write(buffer.begin(), sz);
		}

	protected:
		void _download(const wchar * filename, size_t size); // NotEnoughSpaceException, SocketException, FileException

		AText m_host;
		int m_port;
		Socket* m_socket;
		SocketEvent m_event;
		HANDLE m_hFile;
	};

}


