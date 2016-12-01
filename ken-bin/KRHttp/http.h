#pragma once

#include <KRMessage/net/wsevent.h>
#include <KR3/io/protocol.h>
#include <KR3/io/pipe.h>
#include <KR3/io/selfbufferedstream.h>
#include <KR3/data/map.h>
#include <KR3/data/crypt.h>
#include <KRUtil/uts.h>
#include <KRUtil/net/socket.h>
#include "httpstatus.h"

namespace kr
{

	struct CHAHECONTROL
	{
		dword maxage;
	};

	struct KEEPALIVE
	{
		dword timeout;
	};

	struct HttpHeader
	{
		UnixTimeStamp date;
		AText server;
		CHAHECONTROL cacheControl;
		dword expires;
		UnixTimeStamp lastModified;
		AText eTag;
		dword contentLength;
		KEEPALIVE keepAlive;
		AText connection;
		AText contentType;
		AText location;
	
		HttpStatus statusCode;

		HttpHeader();
		~HttpHeader();
	};

	class HttpHeaderBuilder
	{
	public:
		HttpHeaderBuilder() noexcept;
		HttpHeaderBuilder(HttpHeaderBuilder && data) noexcept;

		void clear() noexcept;
		void appendETag(Text etag) noexcept;
		void appendIfModifiedSince(UnixTimeStamp time) noexcept;
		template <class T>
		void append(Text name, const T &value)
		{
			m_strHeaders << name << ": " << value << "\r\n";
		}
		Text data() const noexcept;

	private:
		AText m_strHeaders;
	};

	class HttpDataBuilder
	{
	public:
		HttpDataBuilder() noexcept;
		HttpDataBuilder(HttpDataBuilder && data) noexcept;

		void clear() noexcept;
		void append(Text line) noexcept;
		void appendUriEncoded(Text line) noexcept;
		io::UriOStream<AText> * getDataOutputStream(Text name) noexcept;
		Text data() const noexcept;

		template <class T> void append(Text name, const T &value)
		{
			value.writeTo(&getDataOutputStream(name));
		}
	private:
		AText m_strData;
	};
	class HttpConnection :public InStream<HttpConnection, char>
	{
	public:
		using PROC = void (*)(HttpConnection * conn, Text value);

		HttpConnection() noexcept;
		~HttpConnection() noexcept;
	
		void registeProc(Text key, PROC proc) noexcept;
		HttpStatus open(Text url, Text reqheader, Text data); // SocketException
		void close() noexcept;
		Text get() noexcept;
		bool next(); // SocketException
		void download(pcwstr filename); // FileException

		size_t readImpl(char * dest, size_t size);
	
		HttpHeader header;

	private:
		Map<Text, PROC> m_procs;
		
		io::SelfBufferedIStream<io::RetypeStream<Socket, char>> m_socket;
	
	};

	//class HTTPProtocol:public Protocol
	//{
	//public:
	//	HTTPProtocol();
	//	~HTTPProtocol();
	//	virtual size_t readFirst(ptr data, dword nSize) override;
	//	virtual size_t readNext(ptr data, dword nSize) override;
	//
	//private:
	//	HTTPConnection m_webfile;
	//};


}
