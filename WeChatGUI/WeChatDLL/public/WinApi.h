#pragma once
#include <windows.h>
#include <string>

bool д�ڴ�_HEX(DWORD hProcess,size_t addr,std::string hex);

//ȡģ��·��
std::string getModulePath(HMODULE hModule);

//ȡ�ļ��汾
std::string getFileVersion(const char* filePath);