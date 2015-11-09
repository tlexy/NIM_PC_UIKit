﻿#include "msglog.h"
#include "gui/session/control/session_util.h"
#include "gui/session/session_form.h"

namespace nim_comp
{
// void JsonToMsg( const Json::Value &json, nim::IMMessage &msg )
// {
// 	msg.session_type_ = json[nim::kNIMMsgKeyToType].asString() == "0" ? nim::kNIMSessionTypeP2P : nim::kNIMSessionTypeTeam;
// 	msg.receiver_accid_ = json[nim::kNIMMsgKeyToAccount].asString();
// 	msg.sender_accid_ = json[nim::kNIMMsgKeyFromAccount].asString();
// 	msg.timetag_ = json[nim::kNIMMsgKeyTime].asInt64();
// 	msg.type_ = json[nim::kNIMMsgKeyType].asInt();
// 	msg.content_ = json[nim::kNIMMsgKeyBody].asString();
// 	msg.attach_ = json[nim::kNIMMsgKeyAttach].asString();
// 	msg.client_msg_id_ = json[nim::kNIMMsgKeyClientMsgid].asString();
// 	msg.resend_flag_ = json[nim::kNIMMsgKeyResendFlag].asInt() == 0 ? false : true;
// 	msg.local_res_path_ = json[nim::kNIMMsgKeyLocalFilePath].asString();
// 	msg.status_ = (nim::NIMMsgLogStatus)json[nim::kNIMMsgKeyLocalLogStatus].asInt();
// 	msg.sub_status_ = (nim::NIMMsgLogSubStatus)json[nim::kNIMMsgKeyLocalLogSubStatus].asInt();
// 
// 	//客户端发送消息是不需要填写
// // 	msg.readonly_sender_client_type_ = json[nim::kNIMMsgKeyFromClientType].asInt();
// // 	msg.readonly_sender_device_id_ = json[nim::kNIMMsgKeyFromDeviceId].asString();
// // 	msg.readonly_sender_nickname_ = json[nim::kNIMMsgKeyFromNick].asString();
// // 	msg.readonly_server_id_ = json[nim::kNIMMsgKeyServerMsgid].asUInt64();
// // 	msg.rescode_ = (NIMResCode)json[nim::kNIMMsgKeyLocalRescode].asInt();
// // 	msg.feature_ = json[nim::kNIMMsgKeyLocalMsgFeature].asInt();
// }

// void MsgToJson(const nim::IMMessage &msg, Json::Value &json)
// {
// 	json[nim::kNIMMsgKeyToType] = msg.session_type_;
// 	json[nim::kNIMMsgKeyToAccount] = msg.receiver_accid_;
// 	json[nim::kNIMMsgKeyFromAccount] = msg.sender_accid_;
// 	json[nim::kNIMMsgKeyTime] = msg.timetag_;
// 	json[nim::kNIMMsgKeyType] = msg.type_;
// 	json[nim::kNIMMsgKeyBody] = msg.content_;
// 	json[nim::kNIMMsgKeyAttach] = msg.attach_;
// 	json[nim::kNIMMsgKeyClientMsgid] = msg.client_msg_id_;
// 	json[nim::kNIMMsgKeyResendFlag] = msg.resend_flag_ ? 1 : 0;
// 
// 	json[nim::kNIMMsgKeyLocalFilePath] = msg.local_res_path_;
// 	json[nim::kNIMMsgKeyLocalLogStatus] = msg.status_;
// 	json[nim::kNIMMsgKeyLocalLogSubStatus] = msg.sub_status_;
// 
// 	//客户端接收消息不需要解析
// // 	json[nim::kNIMMsgKeyHistorySave] = msg.support_cloud_history_;
// // 	json[nim::kNIMMsgKeyMsgRoaming] = msg.support_roam_msg_;
// // 	json[nim::kNIMMsgKeyMsgSync] = msg.support_sync_msg_;
// }

void CustomSysMessageToIMMessage(const nim::SysMessage &sys_msg, nim::IMMessage &msg)
{
	//无法对等
	//msg.attach_ = sys_msg.apns_text_;

	//这个方法只能再要显示在ui前调用
	msg.receiver_accid_ = sys_msg.receiver_accid_;
	msg.sender_accid_ = sys_msg.sender_accid_;
	msg.timetag_ = sys_msg.timetag_;
	msg.content_ = sys_msg.content_;
	msg.attach_ = sys_msg.attach_;
	msg.readonly_server_id_ = sys_msg.id_;
	msg.support_cloud_history_ = sys_msg.support_offline_;
	msg.rescode_ = sys_msg.rescode_;
	msg.feature_ = sys_msg.feature_;
	msg.session_type_ = sys_msg.type_ == nim::kNIMSysMsgTypeCustomP2PMsg ? nim::kNIMSessionTypeP2P : nim::kNIMSessionTypeTeam;
}

// void CustomSysMsgToJson(const MsgData &msg, Json::Value &json)
// {
// 	json[nim::kNIMSysMsgKeyType] = msg.msg_type;
// 	json[nim::kNIMSysMsgKeyToAccount] = msg.to_account;
// 	json[nim::kNIMSysMsgKeyFromAccount] = msg.from_account;
// 	json[nim::kNIMSysMsgKeyTime] = msg.msg_time;
// 	json[nim::kNIMSysMsgKeyMsg] = msg.msg_body;
// 	json[nim::kNIMSysMsgKeyAttach] = msg.msg_attach; 
// 	json[nim::kNIMSysMsgKeyMsgId] = msg.server_msg_id;
// 	json[nim::kNIMSysMsgKeyCustomSaveFlag] = msg.custom_save_flag;
// 	json[nim::kNIMSysMsgKeyCustomApnsText] = msg.custom_apns_text;
// }

bool StringToJson( const std::string &str, Json::Value &json )
{
	Json::Reader reader;
	return reader.parse(str, json);
}

void GetNotifyMsg(const std::string& msg_attach, const std::string& from_account, const std::string& to_account, std::wstring &show_text)
{
	Json::Value json;
	if (StringToJson(msg_attach, json))
	{
		nim::NIMNotificationId id = (nim::NIMNotificationId)json[nim::kNIMNotificationKeyId].asInt();
		std::wstring who = LoginManager::GetInstance()->IsEqual(from_account) ? L"你" : UserService::GetInstance()->GetUserName(from_account);

		std::vector<std::string> ids;
		Json::Value array;

		if (id == nim::kNIMNotificationIdTeamInvite)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataIds];
		}
		else if (id == nim::kNIMNotificationIdTeamApplyPass)
		{
			std::string uid = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataId].asString();
			std::wstring name;
			if (LoginManager::GetInstance()->IsEqual(uid))
			{
				name = L"你";
			}
			else
			{
				std::string team_nick;
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				if(session_wnd)
					team_nick = session_wnd->GetTeamMemberInfo(uid).GetNick();
				name = team_nick.empty() ? UserService::GetInstance()->GetUserName(uid) : name = nbase::UTF8ToUTF16(team_nick);
			}
			show_text = nbase::StringPrintf(L"%s 通过了 %s 的入群申请", who.c_str(), name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamInviteAccept)
		{
			std::string uid = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataId].asString();
			std::wstring name;
			if (LoginManager::GetInstance()->IsEqual(uid))
			{
				name = L"你";
			}
			else
			{
				std::string team_nick;
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				if(session_wnd)
					team_nick = session_wnd->GetTeamMemberInfo(uid).GetNick();
				name = team_nick.empty() ? UserService::GetInstance()->GetUserName(uid) : name = nbase::UTF8ToUTF16(team_nick);				
			}
			show_text = nbase::StringPrintf(L"%s 接受了 %s 的入群邀请", who.c_str(), name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamKick)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataIds];
		}
		else if (id == nim::kNIMNotificationIdTeamLeave)
		{
			std::wstring name;
			if (LoginManager::GetInstance()->IsEqual(from_account))
			{
				name = L"你";
			}
			else
			{
				std::string team_nick;
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				if(session_wnd)
					team_nick = session_wnd->GetTeamMemberInfo(from_account).GetNick();
				name = team_nick.empty() ? UserService::GetInstance()->GetUserName(from_account) : name = nbase::UTF8ToUTF16(team_nick);
			}
			show_text = nbase::StringPrintf(L"%s 离开了群", name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamUpdate)
		{
			std::string team_nick;
			SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
			if (session_wnd)
				team_nick = session_wnd->GetTeamMemberInfo(from_account).GetNick();
			std::wstring name = team_nick.empty() ? UserService::GetInstance()->GetUserName(from_account) : nbase::UTF8ToUTF16(team_nick);

			Json::Value tinfo_json = json[nim::kNIMNotificationKeyData]["tinfo"];
			if(tinfo_json.isMember(nim::kNIMTeamInfoKeyName))
			{
				std::wstring team_name = nbase::UTF8ToUTF16(tinfo_json[nim::kNIMTeamInfoKeyName].asString());
				show_text = nbase::StringPrintf(L"群名称被更新为 %s", team_name.c_str());
			}	
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyAnnouncement))
			{
				show_text = nbase::StringPrintf(L"%s 更新了群公告", name.c_str());
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyJoinMode))
			{
				std::string mode = tinfo_json[nim::kNIMTeamInfoKeyJoinMode].asString();
				show_text = L"群身份验证模式更新为";

				if(mode == "0")
					show_text += L"允许任何人加入";
				else if (mode == "1")
					show_text += L"需要验证消息";
				else if (mode == "2")
					show_text += L"不允许任何人申请加入";
				else
					show_text = L"群身份验证模式已更新";
			}
			else if (tinfo_json.isMember(nim::kNIMTeamInfoKeyIntro))
			{
				show_text = nbase::StringPrintf(L"%s 更新了群介绍", name.c_str());
			}
			else
			{
				show_text = nbase::StringPrintf(L"%s 更新了群信息", name.c_str());
			}
		}
		else if (id == nim::kNIMNotificationIdTeamDismiss)
		{
			show_text = L"已被解散";
		}
		else if (id == nim::kNIMNotificationIdTeamOwnerTransfer)
		{
			std::string to = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataId].asString();
			std::wstring name;
			if (LoginManager::GetInstance()->IsEqual(to)) {
				name = L"你";
			}
			else {
				std::string team_nick;
				SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
				if(session_wnd)
					team_nick = session_wnd->GetTeamMemberInfo(to).GetNick();
				name = team_nick.empty() ? UserService::GetInstance()->GetUserName(to) : nbase::UTF8ToUTF16(team_nick);
			}
			show_text = nbase::StringPrintf(L"%s 将群转让给 %s", who.c_str(), name.c_str());
		}
		else if (id == nim::kNIMNotificationIdTeamAddManager)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataIds];
		}
		else if (id == nim::kNIMNotificationIdTeamRemoveManager)
		{
			array = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationKeyDataIds];
		}
		else if (id == nim::kNIMNotificationIdNetcallBill || \
			id == nim::kNIMNotificationIdNetcallMiss || \
			id == nim::kNIMNotificationIdLocalNetcallReject || \
			id == nim::kNIMNotificationIdLocalNetcallNoResponse)
		{
			//未接电话,{"time":139323423424,"calltype":1,"from":"account_temp","channel":6144978055925334000}
			//话单,{"time":139323423424,"calltype":1,"duration":5,"channel":6144978055925334000}
			int type = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationIdNetCallTypeKey].asUInt();
			std::string creater = from_account;
			bool b_create = LoginManager::GetInstance()->IsEqual(creater);
			std::wstring tm;// = GetMessageTime(time, false);
			if (type == nim::kNIMRtsVideoChatModeAudio)
			{
				show_text = tm + L"[语音通话]";
			}
			else
			{
				show_text = tm + L"[视频通话]";
			}
			if (id == nim::kNIMNotificationIdNetcallMiss)
			{
				show_text += L" 未接";
			} 
			else if (id == nim::kNIMNotificationIdNetcallBill)
			{
				__int64 duration = json[nim::kNIMNotificationKeyData][nim::kNIMNotificationIdNetCallDurationKey].asInt64();
				int h = (int)(duration / 3600), m = (int)((duration % 3600) / 60), s = (int)(duration % 60);
				std::wstring time_tip;
				if (h > 0)
					nbase::StringPrintf(time_tip, L"%d小时%d分%d秒", h, m, s);
				else if (m > 0)
				{
					nbase::StringPrintf(time_tip, L"%d分%d秒", m, s);
				}
				else
					nbase::StringPrintf(time_tip, L"%d秒", s);
				if (type == nim::kNIMRtsVideoChatModeAudio)
				{
					show_text = nbase::StringPrintf(L"%s 时长%s", show_text.c_str(), time_tip.c_str());
				}
				else
				{
					show_text = nbase::StringPrintf(L"%s 时长%s", show_text.c_str(), time_tip.c_str());
				}
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallReject)
			{
				if (b_create)
				{
					show_text += L" 对方正忙";
				} 
				else
				{
					show_text += L" 拒绝接听";
				}
			}
			else if (id == nim::kNIMNotificationIdLocalNetcallNoResponse)
			{
				show_text += L" 未接通，已取消";
			}

		}

		if( !array.empty() && array.isArray() )
		{
			int len = array.size();
			for(int i = 0; i < len; i++)
			{
				ids.push_back( array[i].asString() );
			}
			if( !ids.empty() )
			{
				std::wstring obj;

				int n = ids.size();
				if (n == 1 && LoginManager::GetInstance()->IsEqual(ids[0]))
				{
					obj = L"你";
				}
				else
				{
					int i = 0;
					for(; i < n; i++)
					{
						if( !obj.empty() )
							obj.append(L"，");

						std::string team_nick;
						SessionForm* session_wnd = dynamic_cast<SessionForm*>(WindowsManager::GetInstance()->GetWindow(SessionForm::kClassName, nbase::UTF8ToUTF16(to_account)));
						if(session_wnd)
							team_nick = session_wnd->GetTeamMemberInfo(ids[i]).GetNick();
						std::wstring name = team_nick.empty() ? UserService::GetInstance()->GetUserName(ids[i]) : nbase::UTF8ToUTF16(team_nick);;
						obj.append(name);

						if(i >= 2)
							break;
					}
					if(i < n-1)
					{
						obj.append( nbase::StringPrintf(L"等%d人", n) );
					}
				}

				if (id == nim::kNIMNotificationIdTeamInvite)
				{
					show_text = nbase::StringPrintf(L"%s 邀请 %s 加入了群聊", who.c_str(), obj.c_str());
				}
				else if (id == nim::kNIMNotificationIdTeamKick)
					show_text = nbase::StringPrintf(L"%s 已被移出", obj.c_str());
				else if (id == nim::kNIMNotificationIdTeamAddManager)
					show_text = nbase::StringPrintf(L"%s 被任命为管理员", obj.c_str());
				else if (id == nim::kNIMNotificationIdTeamRemoveManager)
					show_text = nbase::StringPrintf(L"%s 被取消管理员资格", obj.c_str());
			}
		}

	}
	if( show_text.empty() )
		show_text = L"[通知消息]";
}

bool IsNetCallMsg(nim::NIMMessageType msg_type, const std::string& msg_attach)
{
	if (msg_type != nim::kNIMMessageTypeNotification)
	{
		return false;
	}
	Json::Value valus;
	Json::Reader reader;
	if (reader.parse(msg_attach, valus) && valus.isObject() && valus[nim::kNIMNotificationKeyData].isObject())
	{
		nim::NIMNotificationId id = (nim::NIMNotificationId)valus[nim::kNIMNotificationKeyId].asInt();
		if (id == nim::kNIMNotificationIdNetcallBill || \
			id == nim::kNIMNotificationIdNetcallMiss || \
			id == nim::kNIMNotificationIdLocalNetcallReject || \
			id == nim::kNIMNotificationIdLocalNetcallNoResponse)
		{
			return true;
		}
	}

	return false;
}
std::wstring GetCustomMsg(const std::string &msg_attach)
{
	std::wstring show_text = L"[自定义消息]";
	Json::Value json;
	if (StringToJson(msg_attach, json))
	{
		int sub_type = json["type"].asInt();
		if (sub_type == CustomMsgType_Jsb) //finger
		{
			show_text = L"[猜拳]";
		}
		else if (sub_type == CustomMsgType_SnapChat)
		{
			show_text = L"[阅后即焚]";
		}
		else if (sub_type == CustomMsgType_Sticker)
		{
			show_text = L"[贴图]";
		}
		else if (sub_type == CustomMsgType_Rts)
		{
			show_text = L"[白板]";
			if (json["data"].isObject())
			{
				int flag = json["data"]["flag"].asInt();
				if (flag == 0)
				{
					show_text = L"我已发起了[白板演示]";
				}
				else if (flag == 1)
				{
					show_text = L"白板演示已结束";
				}
			}
		}
	}
	return show_text;
}
}