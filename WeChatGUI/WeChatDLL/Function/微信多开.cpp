//����
#include "΢�Ŷ࿪.h"
#include "../΢��ƫ��.h"
#include "../public/WinApi.h"
#include "../WeChatDLL.h"

bool Patch_΢�Ŷ࿪()
{
	return д�ڴ�_HEX(-1, WeChatDLL::Instance()->getWinMoudule() + ΢��ƫ��_�࿪, "EB");
}

