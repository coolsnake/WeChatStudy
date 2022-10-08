#pragma once
#include <vector>
#include <string>
#include <list>
#include <mutex>


enum msgCategory
{
	WX_Undefine = 0x0,
	//˽����Ϣ
	WX_PrivateMsg,
	//Ⱥ��Ϣ
	WX_GroupMsg,
	//���ں�
	WX_PublicMsg,
};

struct MsgUploadInfo
{
	//��Ϣ����
	int msgType;
	//��Ϣ����ʱ��
	unsigned int postTime;
	//��Ϣid
	unsigned long long msgID;
	//��Ϣ��Դ
	std::string wxid;
	//��Ϣ��Դ����
	std::string name;
	//�����ߵ�����
	std::string senderName;
	//�����ߵ�WXID
	std::string senderWxid;
	//��Ϣ����
	std::string msgContent;
	//�����˵�wxid
	std::string robotID;
};

class MsgBuffer
{
public:
	MsgBuffer();
	void pushMsg(MsgUploadInfo& msg);
	void popMsg(std::vector<MsgUploadInfo>& outMsgList);
private:
	std::list<MsgUploadInfo> buffer;
	unsigned int index;
	bool bOverflow;
	std::mutex mute;
};

enum WeChatVersion;
class MsgMonitor
{
public:
	static MsgMonitor& Instance();
	bool InitMsgMonitor(WeChatVersion ver);
	std::vector<MsgUploadInfo> SyncMsg();
	void AddMsg(MsgUploadInfo& msg);
private:
	WeChatVersion WeChatVer;
	MsgBuffer msgBuffer;
};