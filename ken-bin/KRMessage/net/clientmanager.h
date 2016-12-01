#pragma once

#include <KR3/main.h>
#include "server.h"
#include "client.h"
#include <KRUtil/net/socket.h>
#include <KR3/mt/thread.h>
#include <KR3/data/linkedlist.h>

namespace kr
{
	template <class Client>
	class ClientManager
	{
		MUST_BASE_OF(Client, ::kr::Client);
	public:
		inline ClientManager(word port);  // SocketException
		inline void processAccept() noexcept;
		
	private:
		class MgrThread:public ServerProcessor
		{
		public:
			inline int _thread() noexcept;
			inline MgrThread() noexcept;
		};
		inline MgrThread* _getEmptyServer() noexcept;
		Chain<MgrThread> m_list;
		Socket * const m_server;
	};


	template <class Client>
	inline kr::ClientManager<Client>::ClientManager(word port) // SocketException
		:m_server(Socket::create())
	{
		m_server->open(port);
	}
	template<class Client>
	inline void ClientManager<Client>::processAccept() noexcept
	{
		for (;;)
		{
			kr::Socket* socket = m_server->accept();
			MgrThread * mgr = _getEmptyServer();
			Client * newClient = new Client(socket);
			mgr->putClient(socket, newClient, newClient->getSocketEvent());
		}
	}
	template <class Client>
	inline ClientManager<Client>::MgrThread::MgrThread() noexcept
	{
		ThreadHandle::create<MgrThread, &MgrThread::_thread>(this);
	}
	template <class Client>
	inline int ClientManager<Client>::MgrThread::_thread() noexcept
	{
		for (;;)
		{
			dword idx = wait();
			if (idx != -1)
			{
				Client* client = (Client*)getAttachment(idx);
				FNetworkEvent ev = getState(idx);
				try
				{
					if (ev.read)
					{
						client->processRead();
						client->onRead();
					}
					client->processEvent(ev);
				}
				catch (SocketException &)
				{
					pickClient(idx);
					delete client;
				}
				catch (DisconnectException&)
				{
					pickClient(idx);
					delete client;
				}
			}

			for (ClientInfo& i : *this)
			{
				Client * c = (Client*)i.getAttachment();
				try
				{
					c->flush();
				}
				catch (SocketException&)
				{
					pickClient(i.index);
					delete c;
				}
			}
		}
		return 0;
	}
	template <class Client>
	inline typename kr::ClientManager<Client>::MgrThread * kr::ClientManager<Client>::_getEmptyServer() noexcept
	{
		auto iter = m_list.begin();
		auto end = m_list.end();
		while (iter != end)
		{
			if (iter->clientLeft() == 0)
			{
				m_list.reaxis(iter);
				return iter;
			}
			iter++;
		}
		return m_list.create();
	}

}
