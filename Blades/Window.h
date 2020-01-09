#pragma once

#include "WinHeader.h"
#include "BladesException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

class Window
{
public:
	class Exception : public BladesException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT hr;
	};

private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();

		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		static constexpr const wchar_t* wndClassName = L"BladesEngine";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const wchar_t* name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void SetTitle(const std::wstring);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() noexcept;
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

// Embeded Modules 
public:
	Keyboard kbd;
	Mouse mouse;

private:
	bool cursorEnabled = true;
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
};