#include <wrl.h>
#include "WebView2.h"
#include <Urlmon.h>
#include <Shlwapi.h>

#include <KR3/wl/clean.h>
#include <type_traits>
#include <KR3/initializer.h>
#include <KR3/main.h>
#include <KRWin/winx.h>
#include <KRMessage/pump.h>
#include <KR3/meta/function.h>
#include <KR3/util/wide.h>
#include <KR3/util/envvar.h>
#include <KR3/data/map.h>
#include <KRUtil/fs/path.h>

#pragma comment(lib, "KR3.lib")
#pragma comment(lib, "KRUtil.lib")
#pragma comment(lib, "KRWin.lib")
#pragma comment(lib, "KRMessage.lib")
#pragma comment(lib, "Shlwapi.lib")

using namespace kr;
using namespace win;
using namespace Microsoft::WRL;

class ComErrorThrower
{
public:
    ComErrorThrower(HRESULT err) throws(HRESULT)
    {
        if (FAILED(err)) throw err;
    }
};

#define COMERR ComErrorThrower CONCAT(__err, __COUNTER__) =

class Main : public WindowProgram
{
private:
    ComPtr<IWebView2WebView5> m_webview;
    ComPtr<IWebView2Environment> m_env;
    AText16 m_url;
    EventRegistrationToken m_onTitleChanged;
    EventRegistrationToken m_onWebResourceRequested;

public:
    Main(int show) noexcept
    {
        m_url = nullptr;
        m_onTitleChanged.value = 0;

        createPrimary(u"EMViewer", WS_OVERLAPPEDWINDOW, { CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT });
        Window* wnd = getWindow();
        wnd->show(show);
    }

    template <typename LAMBDA>
    void exec(LPCWSTR script, LAMBDA lambda) noexcept
    {
        m_webview->ExecuteScript(script, Callback<IWebView2ExecuteScriptCompletedHandler>([this, lambda=move(lambda)](
            HRESULT result,
            LPCWSTR resultObjectAsJson) {
            if (FAILED(result)) return result;
            lambda();
            return S_OK;
            }).Get());
    }

    bool open(LPWSTR commandLine) noexcept
    {
        //{
        //    TText out;
        //    for (char** s = environ; *s != nullptr; s++)
        //    {
        //        out << *s << "\r\n";
        //    }
        //    MessageBoxA(nullptr, out.c_str(), nullptr, MB_OK);
        //}

        //TText16 test = currentDirectory;
        //errorBox(test.c_str());

        TText16 EMVIEWER_APP = EnviromentVariable16(u"EMVIEWER_APP");
        if (!EMVIEWER_APP.empty())
        {
            currentDirectory.set(EMVIEWER_APP.data());
        }
        else
        {
            EMVIEWER_APP = u".";
            EMVIEWER_APP.c_str();
        }
        m_url = u"https://app/";
        m_url << EnviromentVariable16(u"EMVIEWER_URL");
        m_url.c_str();

        CreateWebView2EnvironmentWithDetails(nullptr, wide(EMVIEWER_APP.data()), commandLine, Callback<IWebView2CreateWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, IWebView2Environment* env) {
                if (FAILED(result)) return result;
                m_env = env;

                env->CreateWebView(getWindow(), Callback<IWebView2CreateWebViewCompletedHandler>(
                    [this](HRESULT result, IWebView2WebView* webview) -> HRESULT {
                        try
                        {
                            COMERR result;
                            COMERR webview->QueryInterface<IWebView2WebView5>(&m_webview);

                            // Add a few settings for the webview
                            // this is a redundant demo step as they are the default settings values
                            IWebView2Settings* Settings;
                            webview->get_Settings(&Settings);
                            Settings->put_IsScriptEnabled(TRUE);
                            Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                            Settings->put_IsWebMessageEnabled(TRUE);

                            COMERR m_webview->add_DocumentTitleChanged(Callback<IWebView2DocumentTitleChangedEventHandler>([this](
                                IWebView2WebView3* sender,
                                IUnknown* args) {
                                    HRESULT res;
                                    LPWSTR title;
                                    if (FAILED(res = sender->get_DocumentTitle(&title))) return res;
                                    getWindow()->setText(unwide(title));
                                    return S_OK;
                                }).Get(), &m_onTitleChanged);

                            COMERR m_webview->AddWebResourceRequestedFilter(
                                L"https://app/*", WEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
                            COMERR m_webview->add_WebResourceRequested(
                                Callback<IWebView2WebResourceRequestedEventHandler>([this](
                                    IWebView2WebView* webview,
                                    IWebView2WebResourceRequestedEventArgs* args) {
                                        try
                                        {
                                            ComPtr<IWebView2WebResourceRequest> req;
                                            COMERR args->get_Request(&req);

                                            LPWSTR uri;
                                            COMERR req->get_Uri(&uri);

                                            TSZ16 path;
                                            Text16 uritx = Text16(unwide(uri));
                                            path << uritx.subarr(12);
                                            path.change(u'/', u'\\');

                                            Text16 ext = uritx.find_r(u'.');

                                            ComPtr<IStream> is;
                                            COMERR SHCreateStreamOnFileW(wide(path.c_str()), 0, &is);

                                            ULARGE_INTEGER size;
                                            COMERR is->Seek(LARGE_INTEGER{ 0, 0 }, STREAM_SEEK_END, &size);

                                            COMERR is->Seek(LARGE_INTEGER{ 0, 0 }, STREAM_SEEK_SET, nullptr);

                                            ComPtr<IWebView2WebResourceResponse> response;
                                            COMERR m_env->CreateWebResourceResponse(
                                                is.Get(), 200, L"OK", L"", &response);

                                            ComPtr<IWebView2HttpResponseHeaders> resp;
                                            COMERR response->get_Headers(&resp);

                                            static const Map<Text16, LPCWSTR> types = {
                                                {u"html", L"text/html; charset=utf-8"},
                                                {u"htm", L"text/html; charset=utf-8"},
                                                {u"js", L"text/javascript"},
                                                {u"wasm", L"application/wasm"},
                                            };

                                            if (ext != nullptr)
                                            {
                                                ext++;
                                                auto iter = types.find(ext);
                                                if (iter != types.end())
                                                {
                                                    COMERR resp->AppendHeader(L"Content-Type", iter->second);
                                                }
                                            }

                                            COMERR resp->AppendHeader(L"Content-Length", wide((TSZ16() << size.QuadPart).c_str()));

                                            COMERR args->put_Response(response.Get());
                                            

                                            return S_OK;
                                        }
                                        catch (HRESULT err)
                                        {
                                            return err;
                                        }
                                }).Get(), &m_onWebResourceRequested);
                        
                            irect bounds = getWindow()->getClientRect();
                            COMERR webview->put_Bounds((RECT&)bounds);
                            COMERR webview->Navigate(wide(m_url.data()));
                            return S_OK;
                        }
                        catch (HRESULT err)
                        {
                            return err;
                        }
                    }).Get());
                return S_OK;
            }).Get());
        return true;
    }

    void wndProc(win::Window* wnd, uint Msg, WPARAM wParam, LPARAM lParam) override
    {
        switch (Msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_SIZE:
            if (m_webview != nullptr)
            {
                irect bounds = getWindow()->getClientRect();
                m_webview->put_Bounds((RECT&)bounds);
            }
            break;
        }
    }
};

int wWinMain(HINSTANCE, HINSTANCE, LPWSTR commandLine, int show)
{
    Initializer<COM> __init;
    WindowProgram::registerClass(0);

    Main mainwindow(show);
    if (!mainwindow.open(commandLine)) return EINVAL;

    return EventPump::getInstance()->messageLoop();
}