#pragma once

#include <KR3/main.h>
#include <KR3/data/linkedlist.h>

#include "socket.h"

namespace kr
{
	namespace irc
	{
		namespace __pri__
		{
			template <typename T> 
			struct __TYPE_CONST
			{
			};

			template <> struct __TYPE_CONST<byte>
			{
				byte limitof = 253;
				byte maxof = 126;
			};
			template <> struct __TYPE_CONST<word>
			{
				word limitof = 253U * 253;
				word maxof = 32004;
			};
			template <> struct __TYPE_CONST<dword>
			{
				dword limitof = 253U * 253 * 253 * 253;
				dword maxof = 2048576040;
			};
		}

#define irc_limitof(type)	(::kr::irc::__pri__::__TYPE_CONST<type>::limitof)
#define irc_maxof(type)		(::kr::irc::__pri__::__TYPE_CONST<type>::maxof)
#define irc_minof(type)		(::kr::irc::__pri__::__TYPE_CONST<type>::maxof+1)

		constexpr uint IRC_IDMAX = 256;
		constexpr uint IRCM_REGIST = 1;
		constexpr uint IRCM_SAMENICK = 433;
		constexpr uint IRCM_NICKLIST = 353;
		constexpr uint IRCM_NLEND = 366;
		constexpr uint IRCM_ERRNICK = 432;
		constexpr uint IRCM_ERRNAME = 468;

		constexpr uint IRCM_PRIVMSG = 10000;
		constexpr uint IRCM_JOIN = 10001;
		constexpr uint IRCM_QUIT = 10002;
		constexpr uint IRCM_NICK = 10003;
		constexpr uint IRCM_PART = 10004;
		constexpr uint IRCM_JOINOPER = 10005;
		constexpr uint IRCM_KICK = 10006;

		constexpr uint IRCM_INCUSER = 20000;
		constexpr uint IRCM_DECUSER = 20001;

		constexpr uint IRCM_LOW = 30000;

		class User;
		class InUser;
		class Channel;
		class IRC;
		template <typename C> class ParameterT;

		using ChannelList = Chain<Channel>;
		using UserList = Chain<User>;
		using InUserList = Chain<InUser>;
		using ParameterA = ParameterT<char>;
		using ParameterW = ParameterT<wchar>;
		using Parameter16 = ParameterT<char16>;
		using Parameter32 = ParameterT<char32>;

		template <typename C>
		class ParameterT: public BArray<Text, 32>
		{
			using Text = RefArray<C>;
		public:

			//NotEnoughSpaceException
			ParameterT(Text str);

			using BArray<Text, 32>::operator [];
		};

		class User
		{
			friend class InUser;
			friend class IRC;
		public:

			User(Text nick);
			~User();

			void user(Text name, Text real);
			void nick(Text nick);
			void real(Text real);
			void setIP(Ipv4Address ip);

			Text nick();
			bool operator ==(User &other);
			bool operator !=(User &other);

		protected:
			BText<IRC_IDMAX> m_strNick, m_strReal, m_strName;
			Ipv4Address m_IPAddress;
			dword m_InCount;

		};

		class InUser
		{
			friend class Channel;
			friend class IRC;
		public:

			InUser(User& user, char mode);
			~InUser();

			void mode(char mode, char sign);
			void kick(User& user);
			char mode();

		protected:
			User& m_user;
			char m_charMode;


		};

		class Channel
		{
			friend class IRC;
		public:
			Channel(Text channel);
			~Channel() noexcept;
			InUser* join(User& user);
			void part(User& user);
			void mode(User& user, char mode, char sign);
			InUser& findUser(User& user);

			Text name();

		protected:
			BText<IRC_IDMAX> m_name;
			InUserList m_InUser;
		};

		class IRC
		{
		public:

			IRC();
			IRC(pcstr url, Text name, Text host, Text nick);
			void ircConnect(pcstr url, Text name, Text host, Text nick);
			void ircRecv();
			void ircSend(Text str);
			//void asyncEvent(Window* pWnd, dword wMsg);

			void ircNick(Text nick);
			void ircReal(Text real);
			void ircPrivMsg(Channel& channel, Text str);
			void ircJoin(Text channel);
			void ircPart(Channel& channel);
			void ircUser(Text nick, Text real);
			bool ircMode(Channel& channel, User& user, char mode, char sign);
			void ircKick(Channel& channel, User& user);
			void ircQuit(Text str = nullptr);
			User& ircFindUser(Text nick);
			Channel& ircFindChannel(Text chname);

			virtual void onIRCMessage(dword msg, User& user, Channel *channel, cptr param) = 0;

			Text getNick();

			ChannelList& getChannelList();

			UserList& getUserList();

		protected:
			Socket* m_socket;
			BText<1024> m_data;
			UserList m_users;
			ChannelList m_channels;

			void _quit(User& user);
			void _sysMsg(Text str, Channel *channel = nullptr);
		};


		template <typename C>
		ParameterT<C>::ParameterT(Text str)
		{
			for (;;)
			{
				str.skip(' ');
				if (str.empty())
					return;
				if (*str == ':')
				{
					str++;
					push(str);
				}
				else
				{
					Text nstr = str.find(' ');
					if (nstr != nullptr)
						push(str.cut(nstr));
					else
						push(str);
					str = nstr;
				}
			}
		}

		namespace _pri_
		{
			template <typename T>
			T rawToIrc(T value)
			{
				if (T(-1)<0)
				{
					if (value<0)
					{
						if (value < -(T)IRCVALUE_MAX(T))
							throw NotEnoughSpaceException();
						return value + IRCVALUE_LIMIT(T);
					}
					else
					{
						if (value >(T)IRCVALUE_MAX(T))
							throw NotEnoughSpaceException();
						return value;
					}
				}
				else
				{
					if (value >= (T)IRCVALUE_LIMIT(T))
						throw NotEnoughSpaceException();
					return value;
				}
			}
			template <typename T>
			T ircToRaw(T value)
			{
				if (T(-1)<0 && (dword)value > IRCVALUE_MAX(T))
					return value - IRCVALUE_LIMIT(T);
				return value;
			}
			template <>
			inline float rawToIrc<float>(float value)
			{
				if (value<0)
					throw NotEnoughSpaceException();
				return value;
			}
			template <>
			inline float ircToRaw<float>(float value)
			{
				return value;
			}
		}
		template <typename T>
		T rawToIrc(T value)
		{
			using uint_t = uint_sz<sizeof(T)>;

			union
			{
				T realValueTemp;
				uint_t temp;
			};
			realValueTemp = _pri_::rawToIrc(value);
			uint_t result = 0;
			for (size_t i = 0; i < sizeof(T); i++)
			{
				uint_t left = temp % 253;
				temp /= 253;
				left++;
				if (left >= 0x0a)
					left++;
				if (left >= 0x0d)
					left++;

				result <<= 8;
				result |= left;
			}
			return (T&)result;
		}
		template <typename T>
		T ircToRaw(T value)
		{
			using uint_t = uint_sz<sizeof(T)>;

			uint_t result = 0;
			byte(*values)[sizeof(T)] = (byte(*)[sizeof(T)]) &value;

			for (byte v : values)
			{
				result *= 253;

				if (v >= 0x0d)
					v--;
				if (v >= 0x0a)
					v--;
				v--;
				result += v;
			}
			return _pri_::ircToRaw((T&)result);
		}

	}
}
