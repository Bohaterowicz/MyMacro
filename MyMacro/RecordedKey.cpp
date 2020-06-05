#include "RecordedKey.h"

RecordedKey::RecordedKey(KeyType Type, DWORD Flags, DWORD TimeStamp, ULONG_PTR Info)
{
	RecordedKey::Type = Type;
	RecordedKey::Flags = Flags;
	RecordedKey::TimeStamp = TimeStamp;
	RecordedKey::ExtraInfo = Info;
}

KeyType RecordedKey::GetType()
{
	return Type;
}

DWORD RecordedKey::GetTimeStamp()
{
	return TimeStamp;
}
