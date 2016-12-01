#pragma once

#include <KR3/wl/handle.h>
#include <KRMessage/message.h>
#include <KR3/data/linkedlist.h>
#include <KR3/math/coord.h>
#include "handle.h"

typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

namespace kr
{
	constexpr int IDN_KCONTROL = 3;

	class LResult
	{
	public:
		LResult(LRESULT lRes);
		explicit LResult(autovar<sizeof(LRESULT)> value);

		operator LRESULT() const;

	private:
		LRESULT lResult;
	};

	class WndProcedure
	{
	public:
		virtual void wndProc(win::Window* pWindow, uint Msg, WPARAM wParam, LPARAM lParam) =0;

	};

	class WndProcWrapper:public Node<WndProcedure>
	{
	public:
		WndProcWrapper();
		WndProcWrapper(WndProc pProc);

		virtual void wndProc(win::Window* pWindow, uint Msg, WPARAM wParam, LPARAM lParam) override;

	private:
		WndProc m_pNextProc;
	};

	class WindowProgram:private Node<WndProcedure>
	{
	public:
		Chain<WndProcedure> procedures;

		WindowProgram() noexcept;
		WindowProgram(win::Window* pWindow) noexcept;
		WindowProgram(const WindowProgram&) = delete;
		WindowProgram& operator =(const WindowProgram&) = delete;
		virtual ~WindowProgram() noexcept;
		win::Window* detachWindow() noexcept;
		win::Window* getWindow() noexcept;
		void createPrimary(pcwstr title, int style, const irectwh & rc) noexcept;
		void createPrimary(pcwstr title, int style, int width, int height) noexcept;
		void createPrimary(pcwstr title, int style) noexcept;
		void create(pcwstr title, int style, const irectwh & rc) noexcept;
		void destroy() noexcept;

		static ATOM registerClass(HICON icon, uint style = CS_HREDRAW | CS_VREDRAW) noexcept;
		static ATOM registerClass(int icon, uint style = CS_HREDRAW | CS_VREDRAW) noexcept;
		static ATOM registerClass(pcwstr className, HICON icon, uint style = CS_HREDRAW | CS_VREDRAW) noexcept;
		static ATOM registerClass(pcwstr className, int icon, uint style = CS_HREDRAW | CS_VREDRAW) noexcept;
		template <typename T, typename... ARGS> T* changeTo(ARGS... args) noexcept
		{
			MUST_BASE_OF(T, WindowProgram);
			win::Window* pWindow = detachWindow();
			delete this;
			return _new T(pWindow, args...);
		}

	protected:
		Keep<win::Window> m_window;

		static LRESULT CALLBACK _wndProc(win::Window* pWindow, uint Msg, WPARAM wParam, LPARAM lParam) noexcept;
	};

	class ExTranslator :public Node<Translator>
	{
		virtual bool translate(const MessageStruct* pMsg) override;
	};

	class SubClassingProgram:public WindowProgram
	{
	public:
		SubClassingProgram();
		SubClassingProgram(win::Window* pWindow);
		SubClassingProgram(const SubClassingProgram&) = delete;
		SubClassingProgram& operator =(const SubClassingProgram&) = delete;
		~SubClassingProgram();

	private:
		WndProcWrapper m_nextProc;
	};

	class WindowClass
	{
	public:
		WindowClass() noexcept;
		WindowClass(pcwstr pszClassName, WndProc pfnWndProc, HICON hIcon, uint style = CS_HREDRAW | CS_VREDRAW, int wndExt = 0, int clsExt = 0) noexcept;
		WindowClass(pcwstr pszClassName, WndProc pfnWndProc, int nIcon, uint style = CS_HREDRAW | CS_VREDRAW, int wndExt = 0, int clsExt = 0) noexcept;

		ATOM registerClass() const noexcept;

		static bool unregister(pcwstr name) noexcept;

	private:
		WNDCLASSEXW m_wc;
	};

	const wchar_t * makeIntResource(int res);

	irect getCursoredMonitorRect() noexcept;
	irectwh calculateWindowPos(dword style, dword w, dword h);
	irectwh calculateWindowPos(dword w, dword h);
	void visiblePrimaryWindow(dword style, dword w, dword h);
	ivec2 getCursorPos();
}