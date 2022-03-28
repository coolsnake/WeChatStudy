#pragma once
#include <windows.h>

struct HookContext
{
	DWORD ELF;
	DWORD EDI;
	DWORD ESI;
	DWORD EBP;
	DWORD ESP;
	DWORD EBX;
	DWORD EDX;
	DWORD ECX;
	DWORD EAX;
};

typedef void(_stdcall * InlineFunc)(HookContext*);

//ֻ֧��x86
class InlineHook
{
public:
	InlineHook();
	~InlineHook();

	bool AddHook(LPVOID pTarget, InlineFunc func);
	bool RemoveHook();
private:
	//���ڱ����滻����5�ֽ�
	BYTE m_OldCode[5];
	//����Hook��ַ,����RemoveHook
	LPVOID m_dwHookAddr = 0;
	DWORD m_dwVirtualAddr = 0;
};

