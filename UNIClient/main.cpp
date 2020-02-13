#include "driver.h"
#include <iostream>
#include <stdio.h>
SOCKET connection;
int pid;
int moduleSize;
extern "C"
{
#pragma region ReadFunc
	__declspec(dllexport) int ReadInt(UINT64 Offset)
	{
		return driver::read<int>(connection, pid, Offset);
	}

	__declspec(dllexport) float ReadFloat(UINT64 Offset)
	{
		return driver::read<float>(connection, pid, Offset);
	}
	__declspec(dllexport) UINT64 ReadPointer(UINT64 Offset)
	{
		return driver::read<UINT64>(connection, pid, Offset);
	}

	__declspec(dllexport) USHORT ReadUshort(UINT64 Offset)
	{
		return driver::read<USHORT>(connection, pid, Offset);
	}

	__declspec(dllexport) BYTE ReadByte(UINT64 Offset)
	{
		return driver::read<BYTE>(connection, pid, Offset);
	}

	__declspec(dllexport) bool ReadBool(UINT64 Offset)
	{
		return driver::read<bool>(connection, pid, Offset);
	}
#pragma endregion

#pragma region WriteFunc
	__declspec(dllexport) void WriteInt(UINT64 Offset, int Value)
	{
		driver::write<int>(connection, pid, Offset, Value);
	}

	__declspec(dllexport) void WriteFloat(UINT64 Offset, float Value)
	{
		driver::write<float>(connection, pid, Offset, Value);
	}

	__declspec(dllexport) void WriteByte(UINT64 Offset, BYTE Value)
	{
		driver::write<BYTE>(connection, pid, Offset, Value);
	}

	__declspec(dllexport) void WriteBool(UINT64 Offset, bool Value)
	{
		driver::write<bool>(connection, pid, Offset, Value);
	}

#pragma endregion

#pragma region InitDLL
	__declspec(dllexport) UINT64 GetBaseAddress(int ProcessID, char* nameModule)
	{
		int returnValue[256];
		if (strlen(nameModule) <= 256)
		{
			for (int i = 0; i < strlen(nameModule); i++) {
				returnValue[i] = (int)nameModule[i];
				moduleSize = i;
			}
		}
	    uint64_t base_address = driver::get_process_base_address(connection, ProcessID, returnValue, moduleSize);
		pid = ProcessID;
		if (!base_address) {
			return 0x0;
		}
		return base_address;
	}

	__declspec(dllexport) bool Init()
	{
		driver::initialize();
		connection = driver::connect();
		if (connection == INVALID_SOCKET) {
			return false;
		}

		if (connection == SOCKET_ERROR) {
			return false;
		}
		return true;
	}

	__declspec(dllexport) void DeInit() {
		driver::disconnect(connection);
		driver::deinitialize();
	}
#pragma endregion
}
