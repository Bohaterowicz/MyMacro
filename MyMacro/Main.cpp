#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <queue>


#include "RecordedKeyboardKey.h"
#include "RecordedMouseKey.h"
#include "KeyType.h"

struct window_handle_data
{
	DWORD ProcessID;
	HWND WindowHandle;
};

LRESULT CALLBACK Win32KeyboardProc(int Code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Win32MouseProc(int Code, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EnumWIndowsCallback(HWND Handle, LPARAM LParam);
BOOL Win32DisplayMonitorsProc(HMONITOR Monitor, HDC DeviceContext, LPRECT Dimensions, LPARAM Data);
BOOL IsMainWindow(HWND Window);
BOOL IsPrimaryMonitor(HMONITOR Monitor);
void SendInput(std::unique_ptr<RecordedKey> Key);
void ProcessWait(DWORD NewTime);




std::queue<std::unique_ptr<RecordedKey>> RecordQueue;


int main() {
	DWORD Error;

	//EnumDisplayMonitors(0, 0, Win32DisplayMonitorsProc, 0);

	HHOOK KeyboardProcHook = SetWindowsHookExA(WH_KEYBOARD_LL, Win32KeyboardProc, 0, 0);
	HHOOK MouseProcHook = SetWindowsHookExA(WH_MOUSE_LL, Win32MouseProc, 0, 0);
	if (KeyboardProcHook && MouseProcHook) {
		for (int i = 0; i < 499999999; ++i) {
			MSG Message;
			while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE)) {
			}
		}
		UnhookWindowsHookEx(KeyboardProcHook);
		UnhookWindowsHookEx(MouseProcHook);
		while (!RecordQueue.empty()) {
			SendInput(std::move(RecordQueue.front()));
			RecordQueue.pop();
		}
		/*
		window_handle_data WindowData = {};
		WindowData.ProcessID = 8216;
		EnumWindows(EnumWIndowsCallback, (LPARAM)&WindowData);
		if (WindowData.WindowHandle != 0) {
			SetForegroundWindow(WindowData.WindowHandle);
			while (!RecordQueue.empty()) {
				recorded_key Key = RecordQueue.front();
				RecordQueue.pop();
				SendInput(&Key);
			}
		}
		*/
	}
	else {
		Error = GetLastError();
	}
	return 0;
}

void SendInput(std::unique_ptr<RecordedKey> Key) {
	INPUT Input = {};
	switch (Key->GetType()) {
		case KeyType::Keyboard: {
			KEYBDINPUT KeyInput = {};
			static_cast<RecordedKeyboardKey*>(Key.get())->CreateKeyboardInput(&KeyInput);
			Input.type = INPUT_KEYBOARD;
			Input.ki = KeyInput;
		}break;
		case KeyType::Mouse: {
			MOUSEINPUT MouseInput = {};
			static_cast<RecordedMouseKey*>(Key.get())->CreateMouseInput(&MouseInput);
			Input.type = INPUT_MOUSE;
			Input.mi = MouseInput;
		}break;
		case KeyType::Wait: {
			DWORD WaitTime = static_cast<RecordedMouseKey*>(Key.get())->GetTimeStamp();
			Sleep(WaitTime);
		}break;
	}
	SendInput(1, &Input, sizeof(Input));
}

BOOL CALLBACK EnumWIndowsCallback(HWND Handle, LPARAM LParam) {
	auto WindowData = (window_handle_data*)LParam;
	DWORD ProcessID = 0;
	GetWindowThreadProcessId(Handle, &ProcessID);
	if (ProcessID != WindowData->ProcessID || !IsMainWindow(Handle)) {
		return TRUE;
	}
	WindowData->WindowHandle = Handle;
	return FALSE;
}

LRESULT CALLBACK Win32MouseProc(int Code, WPARAM wParam, LPARAM lParam) {
	LRESULT Result;
	if (Code >= 0) {
		auto LowLevelMouseInfo = (MSLLHOOKSTRUCT*)lParam;
		std::unique_ptr<RecordedMouseKey> Key;
		switch (wParam) {
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				DWORD MouseEventType = 0;
				if (wParam == WM_LBUTTONDOWN) {
					MouseEventType = MOUSEEVENTF_LEFTDOWN;
				}
				else if (wParam == WM_LBUTTONUP) {
					MouseEventType = MOUSEEVENTF_LEFTUP;
				}
				Key = std::make_unique<RecordedMouseKey>(RecordedMouseKey(LowLevelMouseInfo->pt,
																			0,
																			KeyType::Mouse,
																			MouseEventType | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE,
																			LowLevelMouseInfo->time,
																			LowLevelMouseInfo->dwExtraInfo));
				ProcessWait(LowLevelMouseInfo->time);
				RecordQueue.push(std::move(Key));
			}break;
		}
	}
	Result = CallNextHookEx(0, Code, wParam, lParam);
	return Result;
}

LRESULT CALLBACK Win32KeyboardProc(int Code, WPARAM wParam, LPARAM lParam) {
	LRESULT Result;
	if (Code >= 0) {
		auto LowLevelKeyboardInfo = (KBDLLHOOKSTRUCT*)lParam;
		switch (wParam) {
			case WM_KEYDOWN: {
				std::unique_ptr<RecordedKeyboardKey> Key = std::make_unique<RecordedKeyboardKey>(RecordedKeyboardKey(LowLevelKeyboardInfo->vkCode,
																														LowLevelKeyboardInfo->scanCode,
																														KeyType::Keyboard,
																														LowLevelKeyboardInfo->flags,
																														LowLevelKeyboardInfo->time,
																														LowLevelKeyboardInfo->dwExtraInfo));
				ProcessWait(LowLevelKeyboardInfo->time);
				RecordQueue.push(std::move(Key));
			}break;
		}
	}
	Result = CallNextHookEx(0, Code, wParam, lParam);
	return Result;
}

BOOL Win32DisplayMonitorsProc(HMONITOR Monitor, HDC DeviceContext, LPRECT Dimensions, LPARAM Data) {
	if (IsPrimaryMonitor(Monitor)) {
		RECT dim = *Dimensions;
		return FALSE;
	}
	return TRUE;
}

void ProcessWait(DWORD NewTime) {
	if (!RecordQueue.empty()) {
		DWORD TimeElapsed = NewTime - RecordQueue.back().get()->GetTimeStamp();
		if (TimeElapsed > 10) {
			std::unique_ptr<RecordedKey> WaitKey = std::make_unique<RecordedKey>(RecordedKey(KeyType::Wait, 0, TimeElapsed, 0));
			RecordQueue.push(std::move(WaitKey));
		}
	}
}

BOOL IsMainWindow(HWND Window) {
	return GetWindow(Window, GW_OWNER) == (HWND)0 && IsWindowVisible(Window);
}

BOOL IsPrimaryMonitor(HMONITOR Monitor)
{
	MONITORINFO Info = {};
	Info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfoA(Monitor, &Info);
	if (Info.dwFlags == MONITORINFOF_PRIMARY) {
		return TRUE;
	}
	return FALSE;
}
