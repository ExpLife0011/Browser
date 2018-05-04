#include "stdafx.h"
#include "ClientAppBrowser.h"
#include "ClientSwitches.h"
#include "include/base/cef_logging.h"
#include "include/cef_cookie.h"

namespace Browser
{
	ClientAppBrowser::ClientAppBrowser()
	{
		CreateDelegates(m_delegates);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// CefApp methods.
	void ClientAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
	{
		if (process_type.empty())
		{
			// �����ӽ���·���ܹؼ�����������ã����ܻᴥ��cef��һ��bug
			// cef��LoadUrl������Ⱦ����ʱ��������ӽ��̵�·�������ܻ�����һ��bug����IDE��debug״̬ʱ����
			// ����ָ�����ӽ���·���Ϳ�����

			// ����ʹ��sandbox�Ļ���������ʹ��������ӽ���;��ʹ��sandbox�Ļ�����һ�μ���flash���ʱ�ᵯ����������ʾ�У�����cef��bug��flash���ӽ��̶��߲��ɼ��
			//command_line->AppendSwitchWithValue("browser-subprocess-path", "render.exe");

			//������ģʽ
			//command_line->AppendSwitch("single-process");

#ifndef USE_MINIBLINK
			//command_line->AppendSwitch("--disable-web-security");//�ر�ͬԴ����

			//ʹ��ϵͳFlash
			command_line->AppendSwitch("--enable-system-flash");

			//ָ��Flash
			//command_line->AppendSwitchWithValue("ppapi-flash-version", "20.0.0.228");
			//command_line->AppendSwitchWithValue("ppapi-flash-path", "plugins\\pepflashplayer.dll");
#endif
			//ͬһ�����µ�ʹ��ͬһ����Ⱦ����
			command_line->AppendSwitch("process-per-site");
			command_line->AppendSwitch("enable-caret-browsing");
			command_line->AppendSwitch("auto-positioned-ime-window");

			// Pass additional command-line flags when off-screen rendering is enabled.
			if (command_line->HasSwitch("off-screen-rendering-enabled")) {
				// If the PDF extension is enabled then cc Surfaces must be disabled for
				// PDFs to render correctly.
				// See https://bitbucket.org/chromiumembedded/cef/issues/1689 for details.
				if (!command_line->HasSwitch("disable-extensions") &&
					!command_line->HasSwitch("disable-pdf-extension")) {
						command_line->AppendSwitch("disable-surfaces");
				}

				// Use software rendering and compositing (disable GPU) for increased FPS
				// and decreased CPU usage. This will also disable WebGL so remove these
				// switches if you need that capability.
				// See https://bitbucket.org/chromiumembedded/cef/issues/1257 for details.
				if (!command_line->HasSwitch("enable-gpu")) {
					command_line->AppendSwitch("disable-gpu");
					command_line->AppendSwitch("disable-gpu-compositing");
				}

				// Synchronize the frame rate between all processes. This results in
				// decreased CPU usage by avoiding the generation of extra frames that
				// would otherwise be discarded. The frame rate can be set at browser
				// creation time via CefBrowserSettings.windowless_frame_rate or changed
				// dynamically using CefBrowserHost::SetWindowlessFrameRate. In cefclient
				// it can be set via the command-line using `--off-screen-frame-rate=XX`.
				// See https://bitbucket.org/chromiumembedded/cef/issues/1368 for details.
				command_line->AppendSwitch("enable-begin-frame-scheduling");
			}
		}
		DelegateSet::iterator it = m_delegates.begin();
		for (; it != m_delegates.end(); ++it)
			(*it)->OnBeforeCommandLineProcessing(this, command_line);
	}

	void ClientAppBrowser::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
	{
		// Default schemes that support cookies.
		cookie_schemes.push_back("http");
		cookie_schemes.push_back("https");
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// CefBrowserProcessHandler methods.
	void ClientAppBrowser::OnContextInitialized()
	{
		CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(NULL);
		DCHECK(manager.get());
		manager->SetSupportedSchemes(cookie_schemes, NULL);

		DelegateSet::iterator it = m_delegates.begin();
		for (; it != m_delegates.end(); ++it)
			(*it)->OnContextInitialized(this);
	}

	void ClientAppBrowser::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
	{
		DelegateSet::iterator it = m_delegates.begin();
		for (; it != m_delegates.end(); ++it)
			(*it)->OnBeforeChildProcessLaunch(this, command_line);
	}

	void ClientAppBrowser::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
	{
		DelegateSet::iterator it = m_delegates.begin();
		for (; it != m_delegates.end(); ++it)
			(*it)->OnRenderProcessThreadCreated(this, extra_info);
	}

	// static
	void ClientAppBrowser::CreateDelegates(DelegateSet& delegates) {
	}
}