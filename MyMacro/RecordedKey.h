#pragma once
#include "KeyType.h"
#include <Windows.h>

class RecordedKey
{
protected:
	KeyType Type;
	DWORD Flags;
	DWORD TimeStamp;
	ULONG_PTR ExtraInfo;

public:
	RecordedKey(KeyType Type, DWORD Flags, DWORD TimeStamp, ULONG_PTR Info);
	KeyType GetType();
	DWORD GetTimeStamp();
};