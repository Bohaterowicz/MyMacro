#pragma once
#include "RecordedKey.h"

class RecordedMouseKey : public RecordedKey
{
	POINT Position;
	DWORD MouseData;

public:
	RecordedMouseKey(POINT Position, DWORD MouseData, KeyType Type, DWORD Flags, DWORD TimeStamp, ULONG_PTR Info);
	void CreateMouseInput(MOUSEINPUT* MouseInput);
};
