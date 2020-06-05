#include "RecordedKeyboardKey.h"

RecordedKeyboardKey::RecordedKeyboardKey(DWORD VKCode, DWORD ScanCode, KeyType Type, DWORD Flags, DWORD TimeStamp, ULONG_PTR Info) :
	RecordedKey(Type, Flags, TimeStamp, Info)
{
	RecordedKeyboardKey::VKCode = VKCode;
	RecordedKeyboardKey::ScanCode = ScanCode;
}

void RecordedKeyboardKey::CreateKeyboardInput(KEYBDINPUT* KeyboardInput)
{
	KeyboardInput->wVk = VKCode;
	KeyboardInput->wScan = ScanCode;
	KeyboardInput->dwFlags = Flags;
	KeyboardInput->time = TimeStamp;
	KeyboardInput->dwExtraInfo = ExtraInfo;
}
