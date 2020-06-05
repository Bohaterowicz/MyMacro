#include "RecordedMouseKey.h"

#define MOUSE_VIRTUAL_NORMALIZED_COORD(position, screen_size) ((65536 * (position) / (screen_size)) + ((position) < 0 ? -1 : 1)) 

RecordedMouseKey::RecordedMouseKey(POINT Position, DWORD MouseData, KeyType Type, DWORD Flags, DWORD TimeStamp, ULONG_PTR Info) :
	RecordedKey(Type, Flags, TimeStamp, Info)
{
	RecordedMouseKey::Position = Position;
	RecordedMouseKey::MouseData = MouseData;
}

void RecordedMouseKey::CreateMouseInput(MOUSEINPUT* MouseInput)
{
	HMONITOR PrimaryMonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO MonitorInfo = {};
	MonitorInfo.cbSize = sizeof(MONITORINFO);
	if (GetMonitorInfoA(PrimaryMonitor, &MonitorInfo)) {

		MouseInput->dx = MOUSE_VIRTUAL_NORMALIZED_COORD(Position.x, MonitorInfo.rcMonitor.right);
		MouseInput->dy = MOUSE_VIRTUAL_NORMALIZED_COORD(Position.y, MonitorInfo.rcMonitor.bottom);
		MouseInput->mouseData = MouseData;
		MouseInput->time = TimeStamp;
		MouseInput->dwFlags = Flags;
		MouseInput->dwExtraInfo = ExtraInfo;
	}
}