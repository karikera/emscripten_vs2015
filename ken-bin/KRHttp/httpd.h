#pragma once

#include <KR3/io/selfbufferedstream.h>
#include <KRUtil/net/socket.h>
#include <KRMessage/net/wsevent.h>
#include <KR3/mt/thread.h>
#include <KR3/data/map.h>

#include "httpstatus.h"

namespace kr
{
	class Page;
	class HttpClient;

	class Page
	{
	public:
		Page() noexcept;
		virtual ~Page() noexcept;
		virtual void sendTo(HttpClient * client,Text query, Text postdata) = 0; // SocketException

	private:
	};

	class TemplatePage:public Page
	{
	public:
		TemplatePage(pcwstr filename) noexcept;
		~TemplatePage() noexcept override;
		void parseQuery(Array<Text> &arr, Text prefix, Text query) noexcept;
		void sendTo(HttpClient * client, Text query, Text postdata) override; // SocketException

	private:
		AText m_contents;
		Array<Text> m_splitContents;
		std::unordered_multimap<std::string, size_t> m_keys;
	};

	class StaticPage :public Page
	{
	public:
		StaticPage(Text text) noexcept;
		void sendTo(HttpClient * client, Text query, Text postdata) override; // SocketException

	private:
		Text m_contents;
	};
	class MemoryPage :public Page
	{
	public:
		MemoryPage() noexcept;
		MemoryPage(pcwstr path); // FileException, TooBigException
		MemoryPage(Text header, pcwstr path); // FileException
		MemoryPage(const MemoryPage& _copy) noexcept;
		MemoryPage(MemoryPage&& _move) noexcept;
		~MemoryPage() noexcept;

		MemoryPage & operator =(const MemoryPage& _copy) noexcept;
		MemoryPage & operator =(MemoryPage&& _move) noexcept;

		void sendTo(HttpClient * client, Text query, Text postdata); // SocketException

	private:
		AText m_contents;

	};

	class HttpServer
	{
	public:
		HttpServer() noexcept;
		~HttpServer() noexcept;

		void open(word port = 80); // SocketException
		void close() noexcept;
		void setDefaultHeader(pcwstr filename) noexcept;
		void setErrorPage(HttpStatus exception, pcwstr filename) noexcept;
		void setTemplatePage(Text url, pcwstr filename) noexcept;
		void setMIMEType(Text type, Text mime) noexcept;
		void attachPage(Text url, Page * page) noexcept;
		int process() noexcept;
		void onAccept(HttpClient * client) noexcept;

	private:
		Socket* m_socket;
		ThreadHandle * m_thread;
		AText m_headers;
		SocketEventList<2> m_events;
		Event m_evStop;
		Event m_evRunning;
		std::unordered_map<HttpStatus, Page*> m_error;
		Map<Text, Page*> m_map;
		Map<Text, AText> m_mime;
		bool m_quit;
	};

	class HttpClient
	{
	public:
		AText m_url;

		HttpClient(HttpServer * server, Socket * socket) noexcept;

		void processHeader(); // HttpStatus
		AText getPostData(); // HttpStatus
		void write(Buffer buffer); // SocketException
		int onThread() noexcept;

	private:
		HttpServer * m_server;
		Socket * m_socket;
		io::SelfBufferedIStream<io::SocketStream<char>> m_stream;

	};

}