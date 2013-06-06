#include "BuildOptions.h"
#ifdef WINDOWS_BUILD
#ifndef WINDOW_H
#define WINDOW_H
#include <Windows.h>
#include <string>
using namespace std;


class Window
{
public:
	Window(string title, bool manualResize, int width, int height, int xOffset = 0, int yOffset = 0);
	~Window();
	void Resize(int width, int height, int xOffset = 0, int yOffset = 0);
	void GetSize(int* width, int* height);
	virtual bool ProcessWindowMessages();

	static bool closeAllWindows;
protected:
	Window(Window* prev);
	virtual void Initialize();
	friend class WindowFactory;
	virtual void step(float dt);
	HWND hWnd;
	HINSTANCE hInstance;
	int width, height;
	bool deleteMe;
};

#endif
#endif