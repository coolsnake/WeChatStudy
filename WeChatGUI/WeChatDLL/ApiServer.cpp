#include "ApiServer.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "WeChat/ChatMsg.h"
#include "WeChatDLL.h"
#include <AnyCall/AnyCall.h>
#include "WeChat/common.h"

unsigned int gWechatInstance;

void* SendMessageMgr_Instance()
{
	return AnyCall::invokeStdcall<void*>((void*)(WeChatDLL::Instance().getWinMoudule() + 0x141890));
}

void Api_sendImageMsg(const httplib::Request& req, httplib::Response& res)
{
	nlohmann::json json = nlohmann::json::parse(req.body);
	nlohmann::json retJson;
	std::string toWxid = json["to_wxid"].get<std::string>();
	std::string msgContent = json["image_path"].get<std::string>();
	if (toWxid.empty() || msgContent.empty()) {
		retJson["code"] = 201;
		retJson["msg"] = "empty param";
		res.set_content(retJson.dump(), "application/json");
		return;
	}
	ChatMsg retChatMsg;
	memset(&retChatMsg, 0x0, sizeof(ChatMsg));
	MymmString sendWxid, imagePath, unknowFiled;
	sendWxid.coreStr.assignUTF8(toWxid.c_str());
	imagePath.coreStr.assignUTF8(msgContent.c_str());
	AnyCall::invokeThiscall<void>(SendMessageMgr_Instance(), (void*)(gWechatInstance + 0x5CCDD0), &retChatMsg,
		&sendWxid.coreStr, &imagePath.coreStr, unknowFiled.coreStr);
	retJson["code"] = 200;
	retJson["msg"] = "send ok";
	res.set_content(retJson.dump(), "application/json");
	return;
}

void Api_sendTextMsg(const httplib::Request& req, httplib::Response& res)
{
	nlohmann::json json = nlohmann::json::parse(req.body);
	nlohmann::json retJson;
	std::string toWxid = json["to_wxid"].get<std::string>();
	std::string msgContent = json["msg"].get<std::string>();
	if (toWxid.empty() || msgContent.empty()) {
		retJson["code"] = 201;
		retJson["msg"] = "empty param";
		res.set_content(retJson.dump(), "application/json");
		return;
	}
	ChatMsg objMsg;
	MymmString sendWxid, sendMsg;
	sendWxid.coreStr.assignUTF8(toWxid.c_str());
	sendMsg.coreStr.assignUTF8(msgContent.c_str());
	unsigned int unknowField = 0x0;
	AnyCall::invokeAnycall(&objMsg, &sendWxid.coreStr, (void*)(gWechatInstance + 0x5CD2E0), &sendMsg.coreStr, &unknowField, (void*)1, 0, 0x0);
	retJson["code"] = 200;
	retJson["msg"] = "send ok";
	res.set_content(retJson.dump(), "application/json");
	return;
}

//����Ⱥ��Ϣ
void Api_recvGroupMsg(const httplib::Request& req, httplib::Response& res)
{
	
}

//����˽����Ϣ
void Api_recvPrivateMsg(const httplib::Request& req, httplib::Response& res)
{

}

//��������Ȧ��Ϣ
void Api_recvSnsMsg(const httplib::Request& req, httplib::Response& res)
{

}

//��ȡ��ǰ��¼�û���Ϣ
void Api_getLoginUserInfo(const httplib::Request& req, httplib::Response& res)
{

}

//��ȡ���˱����б�

void Api_getCustomEmotionList(const httplib::Request& req, httplib::Response& res)
{

}

//���͸��˱���

void Api_sendCustomEmotion(const httplib::Request& req, httplib::Response& res)
{
	
}


void StartApiServer(unsigned short port)
{
	gWechatInstance = WeChatDLL::Instance().getWinMoudule();

	httplib::Server svr;

	//��Ϣ���
	svr.Get("/recvGroupMsg", Api_recvGroupMsg);
	svr.Get("/recvPrivateMsg", Api_recvPrivateMsg);
	svr.Post("/sendTextMsg", Api_sendTextMsg);
	svr.Post("/sendImageMsg", Api_sendImageMsg);

	svr.Get("/getCustomEmotionList", Api_getCustomEmotionList);
	svr.Post("/sendCustomEmotion", Api_sendCustomEmotion);

	svr.Get("/recvSnsMsg", Api_recvSnsMsg);
	

	svr.Get("/getLoginUserInfo", Api_getLoginUserInfo);




	svr.listen("0.0.0.0",port);
}