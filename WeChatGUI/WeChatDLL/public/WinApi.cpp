#include "WinApi.h"
#include "Strings.h"

bool д�ڴ�_HEX(DWORD hProcess, size_t addr, std::string hex)
{
	std::vector<unsigned char> bin = HexStrToBin(hex);
	return WriteProcessMemory((HANDLE)hProcess, (LPVOID)addr, bin.data(), bin.size(), 0);
}