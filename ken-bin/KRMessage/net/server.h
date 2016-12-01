#pragma once

#include "wsevent.h"
#include <KR3/mt/thread.h>

namespace kr
{
	class Server
	{
	public:
		struct ClientInfo
		{
			Server * server;
			size_t index;

			Socket * getSocket() noexcept;
			SocketEventHandle * getEvent() noexcept;
			void * getAttachment() noexcept;
			FNetworkEvent getState() noexcept;
		};
		class Iterator
		{
		public:
			ClientInfo* operator ->() noexcept;
			ClientInfo& operator *() noexcept;
			Iterator& operator ++() noexcept;
			bool operator ==(const Iterator & info) noexcept;
			bool operator !=(const Iterator & info) noexcept;

		private:
			ClientInfo m_ci;
		};

		Server() noexcept;
		~Server() noexcept;

		size_t clientLeft() noexcept;
		size_t clientCount() noexcept;
		void closeServer() noexcept;
		Socket* openServer(word port); // SocketException
		dword wait() noexcept;
		dword wait(duration time) noexcept;
		Iterator begin() noexcept;
		Iterator end() noexcept;
		void pickClient(size_t idx) noexcept;
		void putClient(Socket * socket, void * attachment = nullptr) noexcept;
		void putClient(Socket * socket, void * attachment, SocketEventHandle * ev) noexcept;
		Socket * getSocket(size_t idx) noexcept;
		SocketEventHandle * getEvent(size_t idx)  noexcept;
		void * getAttachment(size_t idx) noexcept;
		void setAttachment(size_t idx, void * attachment) noexcept;
		FNetworkEvent getState(size_t idx) noexcept;
		void ** getAttachmentBegin() noexcept;
		void ** getAttachmentEnd() noexcept;
		
	private:
		Socket * m_sockets[EventHandle::MAXIMUM_WAIT];
		SocketEventHandle * m_events[EventHandle::MAXIMUM_WAIT];
		void * m_attachments[EventHandle::MAXIMUM_WAIT];
		size_t m_size;
	};
	class ServerProcessor :public Server
	{
	public:
		ServerProcessor() noexcept;
		~ServerProcessor() noexcept;

		dword wait() noexcept;
		void putClient(Socket * socket, void * attachment = nullptr) noexcept;
		void putClient(Socket * socket, void * attachment, SocketEventHandle * ev) noexcept;
		void quit(int exitCode) noexcept;
		template <typename LAMBDA>
		bool post(LAMBDA lambda) noexcept
		{
			return m_threadId.post(move(lambda));
		}

	private:
		ThreadHandle * m_thread;
		ThreadId m_threadId;
	};
}