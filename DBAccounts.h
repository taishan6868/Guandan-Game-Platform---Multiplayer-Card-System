#ifndef __DBACCOUNTS_H__
#define __DBACCOUNTS_H__

#include "ODBCExt/ODBCExt.h"
#include "lobby_client_msg.h"
#include "authen_server_msg.h"
#include "Odao_Authen_Msg.h"
#include "BankInfoDef_Trans.h"
#include "lobby_bank_authen_msg.h"

class DBAccounts
{
public:
    DBAccounts();
    virtual ~DBAccounts();

    bool Open();

    //-1 执行失败; 存储过程必须返回非负数
    long login_authen_accountinfo_select(LobbyAuthenReq& req, LobbyAuthenRes& info);
	//不知道站点ID的情况下请求登录
	long login_authen_with_no_siteid_accountinfo_select(LobbyAuthenWithNoSiteIdReq_Trans& req, LobbyAuthenWithNoSiteIdRes& info);
	long SPSetUserNickname(LobbySetNicknameReq& req, LobbySetNicknameRsp& info);
	//发送自己的位置等信息
	long login_send_self_location(LobbySendSelfLocationReq& req);
    //long mobile_authen_accountinfo_select(const char *lpszUserName, const char *lpszPassword, MobileAuthenRes& info);
    long login_combin_prize(LobbyHeChengReq& req, LobbyHeChengRes& res);
    long login_get_game_prize(LobbyGetGamePrizeReq& req, LobbyGetGamePrizeRes& res);
    long login_get_hongbao_info(LobbyGetHongbaoInfoReq& req, LobbyHongbaoInfo* lpInfo, unsigned long& ulInfoCount);
	long login_get_hongbao_content(LobbyGetHongbaoContentReq& req);
	long login_get_caipiao_info(LobbyGetCaiPiaoInfoReqDef &req, LobbyGetCaiPiaoInfoResDef &info);
    long game_authen_accountinfo_select(int iUserID, const char *lpszPassword, DBAuthenRes& info, DBAuthenRes_2& info2);
    long game_cost_accountinfo_update(RadiusDisconnect& account);
    long game_onlinestatus_delete(int iUserID);


    //‘玩就送’活动
    long game_present_play_duration_info_select(DBGetActivityInfo& info, DBGetActivityRes& res);
    long game_present_play_duration_info_update(DBAccountToDB& info);

    //红包信息
    long game_hongbao_info_insert(UserHongBaoInfo& hongbaoInfo);
    //银行操作
    long game_bank_login(int iUserID, const char *lpszPassword, LoginBankResult& result);
    long game_bank_get_coin(int iUserID, int iGameID, int iServerID, GetBankCoinReq_trans& getcoin);
    long game_bank_save_coin(int iUserID, int iGameID, int iServerID, SaveCoinReq_trans& savecoin);
    long game_bank_modify_password(int iUserID, const char *lpszOldPwd, const char *lpszNewPwd);
	 
	//手机端银行操作
	long mobile_bank_authen(MobileLoginServerBankAuthenReqPtr pBankAuthen, MobileLoginServerBankAuthenSuccessRsp &result);
	long mobile_bank_get_coin(MobileLoginServerBankGetCoinReqPtr pBankGetCoin, MobileLoginServerBankGetCoinSuccessRsp &result);
	long mobile_bank_save_coin(MobileLoginServerBankSaveCoinReqPtr pBankSaveCoin, MobileLoginServerBankSaveCoinSuccessRsp &result);
	long mobile_bank_modify_password(MobileLoginServerBankModifyPasswordReqPtr pBankModifyPwd);

	//大厅修改登录密码
	long lobby_modify_login_password(LobbyModifyLoginPasswordReq_Trans* modifyPassword);

	//平台活动
	long game_activity_progress_select(int iUserID,int iServerID,int iGameID,DBUserActivityProgressRes *pItem, unsigned long &ulItemCount);
	long game_activity_log_insert(DBSiteActivityLog &SiteActivityLog);

	long game_activity_progress_update(DBUserActivityProgressUpdate &pUpdate);
	long game_activity_userdata_update(DBActivityUserData &pUserData);

	long game_one_praise_another_insert(int fromUserId, int toUserId, unsigned long& praiseCount);
	long game_one_ridicule_another_insert(int fromUserId, int toUserId, unsigned long& ridiculeCount);

private:
    bool OpenDSN(const char *lpName, ODBCExt& db, ODBCContext& ctx);
    bool ExecSP(ODBCContext& ctx, unsigned long ulExecFlag, const char *lpszSP, long argc, ...);

private:
    bool m_bOpen;
    ODBCExt m_dbQPGameData;
    ODBCContext m_ctxQPGameData;
};

#endif //__DBACCOUNTS_H__
