#ifndef __AUTHENSERVER_H__
#define __AUTHENSERVER_H__

#include "Game/GameProvider.h"
#include "DBAccounts.h"

class AuthenServer : public GameProvider
{
public:
    AuthenServer();
    virtual ~AuthenServer();

protected:
    //IGameProvider
    virtual long Start();
    virtual void OnNetMessage(IPlayerNode *lpPlayerNode, unsigned short type, void *buffer, long length);
    virtual void OnClosed(IPlayerNode *lpPlayerNode, bool bClosed);
    virtual void OnTimer(long tmNow);
    virtual IPlayerNode* OnCreatePlayer();

private:
    //login server
    void OnLoginServerIdNotice(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnLobbyAuthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnLobbyAuthenWithNoSiteIdReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnLobbySetUserNicknameReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnLobbySendSelfLocationReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnMobileAuthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnLobbyHeChengReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnLobbyGetGamePrizeReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnLobbyGetHongbaoInfoReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnLobbyGetHongbaoContentReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnLobbyGetCaiPiaoInfoReq(IPlayerNode *lpPlayerNode, void *buffer, long length);

    //game server
    void OnUserRadiusAuthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnDisconnectAccountReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnUserInfoDisconnectReq(IPlayerNode *lpPlayerNode, void *buffer, long length);

    //银行操作
    void OnUserLoginBankReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnGetBankCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnUserSaveCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnModifyBankPasswdReq(IPlayerNode *lpPlayerNode, void *buffer, long length);

    //'玩就送'活动
    void OnDBGetActivityInfo(IPlayerNode *lpPlayerNode, void *buffer, long length);
    void OnDBAccountToDB(IPlayerNode *lpPlayerNode, void *buffer, long length);

	//手机端银行操作
	void OnMobileBankAnthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnMobileBankGetCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnMobileBankSaveCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnMobileBankModifyPasswdReq(IPlayerNode *lpPlayerNode, void *buffer, long length);

	//手机登陆密码修改
	void OnLobbyModifyLoginPasswordReq(IPlayerNode *lpPlayerNode, void *buffer, long length);

    //红包信息
    void OnUserHongBaoInfo(IPlayerNode *lpPlayerNode, void *buffer, long length);

	//平台活动
	void OnSiteActivityPlayProgressReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnSiteActivityPlayProgressUpdate(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnSiteActivityPlayLogReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void OnSiteActivityAwardReq(IPlayerNode *lpPlayerNode, void *buffer, long length);

	//玩家点赞拉黑
	void HandleOnePraiseAnotherReq(IPlayerNode *lpPlayerNode, void *buffer, long length);
	void HandleOneRidiculeAnotherReq(IPlayerNode *lpPlayerNode, void *buffer, long length);

private:
    DBAccounts m_db;
    IPlayerNode *m_lpAuthenServer;
};

#endif //__LOGINSERVER_H__
