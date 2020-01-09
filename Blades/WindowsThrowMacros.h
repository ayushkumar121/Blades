#pragma once

#define BDWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr);
#define BDWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError());
