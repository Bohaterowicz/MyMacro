#pragma once
#include "RecordedKey.h"


class RecordedKeyboardKey : public RecordedKey
{
	DWORD VKCode;
	DWORD ScanCode;

public:
	RecordedKeyboardKey(DWORD VKCode, DWORD ScanCode, KeyType Type, DWORD Flags, DWORD TimeStamp, ULONG_PTR Info);
	void CreateKeyboardInput(KEYBDINPUT* KeyboardInput);
};

