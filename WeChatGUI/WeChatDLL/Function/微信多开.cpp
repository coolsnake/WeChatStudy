//����
#include "΢�Ŷ࿪.h"
#include "../΢��ƫ��.h"
#include "../public/WinApi.h"
#include "../WeChatDLL.h"

bool Patch_΢�Ŷ࿪(WeChatVersion ver)
{
	if (ver == WeChat_3_7_6_44) {
		return д�ڴ�_HEX(-1, WeChatDLL::Instance().getWinMoudule() + 0xE7FCBE, "EB");
	}
	return false;
}
