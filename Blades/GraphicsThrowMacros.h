#pragma once

#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = hrcall)) throw Graphics::Exception(__LINE__, __FILE__, hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, hr)