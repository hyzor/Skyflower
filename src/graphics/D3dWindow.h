#ifndef D3DWINDOW_H
#define D3DWINDOW_H

#include "shared/GameTimer.h"
#include <string>
#include <sstream>
#include "Direct3D.h"
#include <iostream>

class D3dWindow
{
public:
	D3dWindow(HINSTANCE hInstance);
	~D3dWindow(void);

	bool Init();
	int Run();

	HINSTANCE AppInst() const;
	HWND MainWnd() { return mhMainWnd; }
	float AspectRatio() const;

	UINT GetWindowWidth() const;
	UINT GetWindowHeight() const;

	void SetResolution(UINT Width, UINT Height);
	void SetFullscreen(bool toFullscreen);
	void SwitchFullscreen();

	bool IsFullscreen();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void OnResize();

	void ShowWindow();

	Direct3D* GetDirect3D() { return mDirect3D; }
	bool AppPaused() { return mAppPaused; }

	void CalculateFrameStats();
	GameTimer* GetTimer() { return mTimer; }

protected:
	bool InitMainWindow();

	BOOL mIsFullscreen;

	HINSTANCE mhAppInst;
	HWND mhMainWnd;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;

	// Customized values
	int mClientWidth;
	int mClientHeight;
	std::wstring mMainWndCaption;

	GameTimer* mTimer;

	Direct3D* mDirect3D;
};

#endif
