#pragma once
#include <string>

//�ı����ͱ�����utf8
struct GroupMsgInfo
{
	//��Ϣ����
	int msgType;
	//�����ߵ�����
	std::string senderName;
	//�����ߵ�WXID
	std::string senderWxid;
	//��Ϣ����
	std::string msgContent;
	//Ⱥ��
	std::string groupName;
	//ȺID
	std::string groupID;
	//������WXID
	std::string robotID;
	//��Ϣ����ʱ��
	std::uint64_t postTime;
};

void UploadMsg(GroupMsgInfo& msgInfo);

void PLUGIN_��Ϣ�ϴ�();