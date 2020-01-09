#include "App.h"

const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];

	size_t outSize;
	mbstowcs_s(&outSize, &wc[0], cSize, c, cSize - 1);
	return wc;
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow )
{
	try 
	{
		return App().Go();
	}
	catch (const BladesException &e)
	{
		MessageBox(
			nullptr,
			GetWC(e.what()),
			GetWC(e.GetType()),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (const std::exception& e)
	{
		MessageBox(
			nullptr,
			GetWC(e.what()),
			L"Standard Exception",
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	catch (...)
	{
		MessageBox(
			nullptr,
			L"No details available",
			L"Unknown Exception",
			MB_OK | MB_ICONEXCLAMATION
		);
	}

	return -1;
}