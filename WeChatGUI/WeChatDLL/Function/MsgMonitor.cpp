#include "MsgMonitor.h"
#include <json.hpp>
#include <fstream>
#include <vector>
#include <httplib.h>
#include <AnyCall/AnyCall.h>
#include "../InlineHook/InlineHook.h"
#include "../WeChatDLL.h"
#include "../TinyXml/tinyxml2.h"
#include "../public/Strings.h"
#include "../public/Public.h"
#include "../public/WinApi.h"
#include "../WeChat/ChatMsg.h"
#include "../WeChatDLL.h"
#include "../΢��ƫ��.h"
#include "ContactFunction.h"
#include "AccountFunction.h"
#include "��Ϣ�ϴ�.h"

InlineHook gHook_AddChatMsg;
InlineHook gHook_ImageDownload;

std::mutex gMutexMsg;


void WeChatNormalImageHandler(MyChatMsg& chatMsg)
{

}

void WeChatGroupImageHandler(MyChatMsg& chatMsg)
{
	GroupMsgInfo tmpMsgInfo;
	tmpMsgInfo.msgType = chatMsg.msgType;
	tmpMsgInfo.robotID = AccountFunction::currentUserWxid;
	tmpMsgInfo.senderWxid = chatMsg.sendWxid;
	tmpMsgInfo.senderName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.sendWxid).nickName.c_str());
	tmpMsgInfo.groupID = chatMsg.FromUserName;
	tmpMsgInfo.groupName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.FromUserName).nickName.c_str());
	tmpMsgInfo.postTime = 1000 * uint64_t(chatMsg.CreateTime);
	if (chatMsg.imagePath == "") {
		return;
	}
	tmpMsgInfo.msgContent = ReadFileToString(chatMsg.imagePath.c_str());
	if (!tmpMsgInfo.msgContent.size()) {
		return;
	}
	
}

void __stdcall MyDownloadImageSuccessed(HookContext* hookContext)
{
	char* pNetSceneGetMsgImgCDN = (char*)hookContext->ECX;
	ChatMsg* pChatMsg = (ChatMsg*)(pNetSceneGetMsgImgCDN + 0x5F4);

	MyChatMsg myChatMsg = CopyChatMsg(pChatMsg);
	mmString* pImageDownloadPath = (mmString*)(pNetSceneGetMsgImgCDN + 0x98C);
	myChatMsg.imagePath = UnicodeToAnsi(copyMMString(pImageDownloadPath).c_str());
	if (myChatMsg.FromUserName.find("@chatroom") != std::string::npos) {
		WeChatGroupImageHandler(myChatMsg);
	}
	else {
		WeChatNormalImageHandler(myChatMsg);
	}
}

void Handle_TicketInfoMsg(MyChatMsg& chatMsg)
{
	GroupMsgInfo tmpMsg;
	tmpMsg.msgType = chatMsg.msgType;
	tmpMsg.robotID = AccountFunction::currentUserWxid;
	tmpMsg.postTime = 1000 * uint64_t(chatMsg.CreateTime);
	tmpMsg.groupID = chatMsg.FromUserName;
	tmpMsg.groupName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.FromUserName).nickName.c_str());
	tmpMsg.senderWxid = chatMsg.sendWxid;
	tmpMsg.senderName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.sendWxid).nickName.c_str());
	tmpMsg.msgContent = LocalCpToUtf8(chatMsg.msgContent.c_str());
	
}

void Handle_EmojiChatMsg(MyChatMsg& chatMsg)
{
	GroupMsgInfo tmpMsg;
	tmpMsg.msgType = chatMsg.msgType;
	tmpMsg.robotID = AccountFunction::currentUserWxid;
	tmpMsg.postTime = 1000 * uint64_t(chatMsg.CreateTime);
	tmpMsg.groupID = chatMsg.FromUserName;
	tmpMsg.groupName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.FromUserName).nickName.c_str());
	tmpMsg.senderWxid = chatMsg.sendWxid;
	tmpMsg.senderName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.sendWxid).nickName.c_str());

	tinyxml2::XMLDocument xmlDocument;
	if (xmlDocument.Parse(chatMsg.msgContent.c_str()) != tinyxml2::XMLError::XML_SUCCESS) {
		WeChatDLL::Instance().MsgRecvLogger()->error("[Handle_EmojiMsg]������Ϣʧ��:" + chatMsg.msgContent);
		return;
	}
	auto elementMsg = xmlDocument.FirstChildElement("msg");
	if (!elementMsg) {
		WeChatDLL::Instance().MsgRecvLogger()->error("[Handle_EmojiMsg]δ�ҵ�msg��Ϣ�ڵ�:" + chatMsg.msgContent);
		return;
	}
	auto elementEmoji = elementMsg->FirstChildElement("emoji");
	if (!elementEmoji) {
		WeChatDLL::Instance().MsgRecvLogger()->error("[Handle_EmojiMsg]δ�ҵ�emoji��Ϣ�ڵ�:" + chatMsg.msgContent);
		return;
	}
	auto elementCDN = elementEmoji->Attribute("cdnurl");
	if (!elementCDN) {
		WeChatDLL::Instance().MsgRecvLogger()->error("[Handle_EmojiMsg]δ�ҵ�cdnurl��Ϣ�ڵ�:" + chatMsg.msgContent);
		return;
	}
	tmpMsg.msgContent = LocalCpToUtf8(elementCDN);

}

void Handle_NormalChatMsg(MyChatMsg& chatMsg)
{
	MsgUploadInfo tmpMsg;
	tmpMsg.msgType = chatMsg.msgType;
	tmpMsg.robotID = AccountFunction::currentUserWxid;
	tmpMsg.postTime = chatMsg.CreateTime;
	tmpMsg.msgID = chatMsg.msgType;
	tmpMsg.fromWxid = chatMsg.FromUserName;
	if (!tmpMsg.fromWxid.empty()) {
		tmpMsg.fromName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.FromUserName).nickName.c_str());
	}
	tmpMsg.senderWxid = chatMsg.sendWxid;
	tmpMsg.senderName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.sendWxid).nickName.c_str());
	tmpMsg.msgContent = LocalCpToUtf8(chatMsg.msgContent.c_str());
	MsgMonitor::Instance().AddMsg(tmpMsg);
}

bool CopyXmlElementText(tinyxml2::XMLElement* dst, tinyxml2::XMLElement* src, const char* nodeName)
{
	auto nodeElement = src->FirstChildElement(nodeName);
	if (!nodeElement) {
		return false;
	}
	auto newNodeElement = dst->FirstChildElement(nodeName);
	if (!newNodeElement) {
		return false;
	}
	const char* pText = nodeElement->GetText();
	if (pText) {
		newNodeElement->SetText(pText);
	}
	else {
		dst->DeleteChild(newNodeElement);
	}
	return true;
}

std::string ParseAppMsg(std::string& appMsg)
{
	tinyxml2::XMLDocument originDocument;
	if (originDocument.Parse(appMsg.c_str()) != tinyxml2::XMLError::XML_SUCCESS) {
		WeChatDLL::Instance().MsgRecvLogger()->error("[ParseAppMsg]" + appMsg);
		return "";
	}

	auto originMsgElement = originDocument.FirstChildElement("msg");
	if (!originMsgElement) {
		return "";
	}

	//��������Ҫ�洢�Ľ��
	tinyxml2::XMLDocument newDocument;
	newDocument.InsertFirstChild(newDocument.NewDeclaration());
	tinyxml2::XMLElement* newMsgElement = newDocument.NewElement("msg");
	newDocument.InsertEndChild(newMsgElement);
	newMsgElement->InsertNewChildElement("appmsg");
	newMsgElement->InsertNewChildElement("appinfo");

	tinyxml2::XMLElement* newAppMsgElement = newMsgElement->FirstChildElement("appmsg");
	newAppMsgElement->InsertNewChildElement("title");
	newAppMsgElement->InsertNewChildElement("url");
	newAppMsgElement->InsertNewChildElement("des");
	newAppMsgElement->InsertNewChildElement("weappinfo");

	tinyxml2::XMLElement* newWeAppInfoElement = newAppMsgElement->FirstChildElement("weappinfo");
	newWeAppInfoElement->InsertNewChildElement("pagepath");
	newWeAppInfoElement->InsertNewChildElement("username");
	newWeAppInfoElement->InsertNewChildElement("weappiconurl");
	newWeAppInfoElement->InsertNewChildElement("shareId");

	tinyxml2::XMLElement* newAppInfoElement = newAppMsgElement->InsertNewChildElement("appinfo");
	newAppInfoElement->InsertNewChildElement("version");
	newAppInfoElement->InsertNewChildElement("appname");

	//��ʼת������
	auto appmsgElement = originMsgElement->FirstChildElement("appmsg");
	if (appmsgElement) {
		if (appmsgElement->Attribute("appid")) {
			newAppMsgElement->SetAttribute("appid", appmsgElement->Attribute("appid"));
		}
		CopyXmlElementText(newAppMsgElement, appmsgElement, "title");
		CopyXmlElementText(newAppMsgElement, appmsgElement, "url");
		CopyXmlElementText(newAppMsgElement, appmsgElement, "des");

		auto newWeAppInfoElement = newAppMsgElement->FirstChildElement("weappinfo");
		auto weappinfoElement = appmsgElement->FirstChildElement("weappinfo");
		if (weappinfoElement) {
			CopyXmlElementText(newWeAppInfoElement, weappinfoElement, "pagepath");
			CopyXmlElementText(newWeAppInfoElement, weappinfoElement, "username");
			CopyXmlElementText(newWeAppInfoElement, weappinfoElement, "weappiconurl");
			CopyXmlElementText(newWeAppInfoElement, weappinfoElement, "shareId");
		}
	}

	auto appinfoElement = originMsgElement->FirstChildElement("appinfo");
	if (appinfoElement) {
		CopyXmlElementText(newAppInfoElement, appinfoElement, "version");
		CopyXmlElementText(newAppInfoElement, appinfoElement, "appname");
	}

	tinyxml2::XMLPrinter printer;
	newDocument.Print(&printer);
	return printer.CStr();
}

void Handle_AppChatMsg(MyChatMsg& chatMsg)
{
	GroupMsgInfo tmpMsg;
	tmpMsg.msgType = chatMsg.msgType;
	tmpMsg.robotID = AccountFunction::currentUserWxid;
	tmpMsg.postTime = 1000 * uint64_t(chatMsg.CreateTime);
	tmpMsg.groupID = chatMsg.FromUserName;
	tmpMsg.groupName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.FromUserName).nickName.c_str());
	tmpMsg.senderWxid = chatMsg.sendWxid;
	tmpMsg.senderName = LocalCpToUtf8(ContactModule::Instance().getContactInfoDynamic(chatMsg.sendWxid).nickName.c_str());

	std::string appMsg = ParseAppMsg(chatMsg.msgContent);
	tmpMsg.msgContent = LocalCpToUtf8(appMsg.c_str());
}


void __stdcall MyAddChatMsg(HookContext* hookContext)
{
	ChatMsg* pChatMsg = (ChatMsg*)hookContext->ECX;
	if (pChatMsg->msgType == 51 || pChatMsg->msgType == 10002) {
		return;
	}

	MyChatMsg tmpMsg = CopyChatMsg(pChatMsg);
	if (tmpMsg.IsOwner) {
		tmpMsg.sendWxid = AccountFunction::currentUserWxid;
	}
	switch (tmpMsg.msgType)
	{
	case 1:
		Handle_NormalChatMsg(tmpMsg);	//��ͨ��Ϣ
		break;
	case 3:
		//�������ﴦ��ͼƬ��Ϣ
		break;
	case 42:
		//Handle_TicketInfoMsg(tmpMsg);  //��Ƭ��Ϣ
		break;
	case 43:
		//Handle_VideoMsg(addMsg);    //��Ƶ��Ϣ
		break;
	case 47:
		//Handle_EmojiChatMsg(tmpMsg);	//������Ϣ
		break;
	case 49:
		//Handle_AppChatMsg(tmpMsg);		//Ӧ����Ϣ
		break;
	case 10000:
		//ϵͳ֪ͨ
		break;
	default:
		break;
	}
}


MsgMonitor& MsgMonitor::Instance()
{
	static MsgMonitor gMsgMonitor;
	return gMsgMonitor;

}

bool MsgMonitor::InitMsgMonitor(WeChatVersion v)
{
	WeChatVer = v;

	DWORD hWeChatWinDLL = WeChatDLL::Instance().getWinMoudule();
	if (WeChatVer == WeChat_3_7_6_44) {
		gHook_AddChatMsg.AddHook((LPVOID)(hWeChatWinDLL + 0x5F7423), MyAddChatMsg);
		return true;
	}

	//gHook_ImageDownload.AddHook((LPVOID)(hWeChatWinDLL + 0x58AD50), MyDownloadImageSuccessed);
	

	//����ͼƬ�Զ�����
	//д�ڴ�_HEX(-1, WeChatDLL::Instance()->getWinMoudule() + ΢��ƫ��_ͼƬ�Զ�����, "B001C3");
	return false;
}

std::vector<MsgUploadInfo> MsgMonitor::SyncMsg()
{
	std::vector<MsgUploadInfo> retMsgList;
	msgBuffer.popMsg(retMsgList);
	return retMsgList;
}

void MsgMonitor::AddMsg(MsgUploadInfo& msg)
{
	msgBuffer.pushMsg(msg);
}

MsgBuffer::MsgBuffer()
{
	
}

inline void MsgBuffer::pushMsg(MsgUploadInfo& msg)
{
	mute.lock();
	if (buffer.size() >= 1000) {
		buffer.pop_front();
	}
	buffer.push_back(msg);
	mute.unlock();
}

void MsgBuffer::popMsg(std::vector<MsgUploadInfo>& outMsgList)
{
	mute.lock();
	unsigned int msgCount = 0;
	for (std::list<MsgUploadInfo>::iterator it = buffer.begin(); it != buffer.end(); ++it) {
		outMsgList.push_back(*it);
		if (++msgCount >= 10) {
			break;
		}
	}
	for (unsigned int n = 0; n < msgCount; ++n) {
		buffer.pop_front();
	}
	mute.unlock();
	return;
}