#include "Window.h"
#include "WindowsThrowMacros.h"

#include "resource1.h"
#include <cstdlib>
#include <sstream>

#include "imgui/imgui_impl_win32.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 64, 64, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(
	int width, 
	int height, 
	const wchar_t* name
)
	:
	width(width),
	height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;


	if (AdjustWindowRect(
		&wr,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		FALSE) == 0)
	{
		throw BDWND_LAST_EXCEPT();
	}

	hWnd = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr, nullptr,
		WindowClass::GetInstance(),
		this
	);

	if (hWnd == nullptr)
	{
		throw BDWND_LAST_EXCEPT();
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	//Init ImGui Win32 Impl
	ImGui_ImplWin32_Init(hWnd);
	// create graphics object
	pGfx = std::make_unique<Graphics>(hWnd);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw  BDWND_LAST_EXCEPT();
	}
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::wstring title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw BDWND_LAST_EXCEPT();
	}
}

void Window::EnableCursor() noexcept
{
	cursorEnabled = true;
	
	// Show Cursor
	while (::ShowCursor(TRUE) < 0);
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

	// Free Cursor

	ClipCursor(nullptr);
}

void Window::DisableCursor() noexcept
{
	cursorEnabled = false;
	
	// Hide Cursor
	while (::ShowCursor(FALSE) >= 0);
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;

	// Confine Cursor

	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

bool Window::CursorEnabled() noexcept
{
	return cursorEnabled;
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

Graphics& Window::Gfx()
{
	//if (!pGfx)
		//throw BDWND_NOGFX_EXCEPT();

	return *pGfx;
}

LRESULT CALLBACK Window::HandleMsgSetup(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
) noexcept
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return pWnd->HandleMsg(
			hWnd,
			msg,
			wParam,
			lParam
		);
	}

	return DefWindowProc(
		hWnd,
		msg,
		wParam,
		lParam
	);
}

LRESULT CALLBACK Window::HandleMsgThunk(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return pWnd->HandleMsg(
		hWnd,
		msg,
		wParam,
		lParam
	);
}

LRESULT Window::HandleMsg(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	
	case WM_ACTIVATE:
		if (!cursorEnabled) 
		{
			if (wParam & WA_ACTIVE)
			{
				// Confine Cursor

				RECT rect;
				GetClientRect(hWnd, &rect);
				MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
				ClipCursor(&rect);

				while (::ShowCursor(FALSE) >= 0); // Hide Cursor
			}
			else
			{
				ClipCursor(nullptr); // Free Cursor

				while (::ShowCursor(TRUE) < 0); // Show Cursor
			}
		}
		break;


	// KEYBOARD MESSAGES
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}

		if(!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;

	case WM_CHAR:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;

	// MOUSE MESSAGES
	case WM_MOUSEMOVE:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			
			if (!mouse.IsInWindow()) 
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}

		break;
	}

	case WM_LBUTTONDOWN: 
	{
		if (!cursorEnabled)
		{
			// Confine Cursor

			RECT rect;
			GetClientRect(hWnd, &rect);
			MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
			ClipCursor(&rect);

			while (::ShowCursor(FALSE) >= 0); // Hide Cursor
		}

		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
		
	case WM_RBUTTONDOWN:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break; 
	}

	case WM_LBUTTONUP:
	{	
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}

	case WM_RBUTTONUP:
	{	
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}

	case WM_MOUSEWHEEL:
	{	
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnWheelDelta(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
		break; 
	}

	// RAW INPUT MESSAGES
	case WM_INPUT:
	{
		if (!mouse.RawEnabled())
		{
			break;
		}

		UINT size;

		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)
		) == -1)
		{
			break;
		}

		rawBuffer.resize(size);

		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			// bail msg processing if error
			break;
		}
		// process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


// Window Exception stuff

Window::Exception::Exception(
	int line,
	const char* file,
	HRESULT hr
) noexcept
	:
	BladesException(line, file),
	hr(hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Blades Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	wchar_t* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf),
		0,
		nullptr
	);

	if (nMsgLen == 0)
	{
		return "Undefined error code";
	}

	char buffer[200];
	size_t x;

	buffer[0] = '\0';
	wcstombs_s(&x, buffer, pMsgBuf, sizeof(buffer));

	std::string errorString = buffer;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}