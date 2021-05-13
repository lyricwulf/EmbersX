// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <wchar.h>
#include <wincodec.h>

#include <cinttypes>

template <class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
  if (*ppInterfaceToRelease != NULL) {
    (*ppInterfaceToRelease)->Release();

    (*ppInterfaceToRelease) = NULL;
  }
}

#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG)
#define Assert(b)                             \
  do {                                        \
    if (!(b)) {                               \
      OutputDebugStringA("Assert: " #b "\n"); \
    }                                         \
  } while (0)
#else
#define Assert(b)
#endif  // DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp {
 public:
  DemoApp();
  ~DemoApp();

  // Register the window class and call methods for instantiating drawing
  // resources
  HRESULT Initialize();

  // Process and dispatch messages
  void RunMessageLoop();

  void LoadBitmap(void* pixel_buffer, uint32_t height);
  float Render();
  uint8_t m_pOffset = 0;

 private:
  // Initialize device-independent resources.
  HRESULT CreateDeviceIndependentResources();

  // Initialize device-dependent resources.
  HRESULT CreateDeviceResources();

  // Release device-dependent resource.
  void DiscardDeviceResources();

  // Draw content.
  HRESULT OnRender();

  // Resize the render target.
  void OnResize(UINT width, UINT height);

  // The windows procedure.
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                                  LPARAM lParam);

 private:
  HWND m_hwnd;
  ID2D1Factory* m_pDirect2dFactory;
  ID2D1HwndRenderTarget* m_pRenderTarget;
  ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
  ID2D1SolidColorBrush* m_pCornflowerBlueBrush;

  HBITMAP m_pBitmap;
};
