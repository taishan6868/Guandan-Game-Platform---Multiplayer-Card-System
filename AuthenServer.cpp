#include "AuthenServer.h"
#include "AuthenPlayer.h"
#include "ServerDef.h"
#include "authen_server_msg.h"
#include "login_server_msg.h"
#include "Odao_Authen_Msg.h"
#include "BankInfoDef_Trans.h"
#include <string.h>

CIniFile g_cfg;

AuthenServer::AuthenServer()
{
    m_lpAuthenServer = NULL;

    g_cfg.InitFile(CONFIG_FILE);

    int iInterval = g_cfg.GetValueInt("network", "interval", 20);
    int iTimeout = g_cfg.GetValueInt("network", "timeout", 120);
    int iMaxSendCount = g_cfg.GetValueInt("network", "MAX_SEND_COUNT", 0);
    int iMaxRecvCount = g_cfg.GetValueInt("network", "MAX_RECV_COUNT", 0);

    if (true == GameProvider::Initialize(iMaxRecvCount, iMaxSendCount, iInterval, iTimeout))
    {
        __log(_DEBUG, "AuthenServer::AuthenServer", "interval[%d], timeout[%d]", iInterval, iTimeout);
    }
    else
    {
        __log(_ERROR, "AuthenServer::AuthenServer", "GameProvider::Initialize() error !");
    }
}

AuthenServer::~AuthenServer()
{
    GameProvider::Shutdown();
}

long AuthenServer::Start()
{
    long lResult = -1;
    char szBuffer[32];

    if (true == m_db.Open())
    {
        if (0 != g_cfg.GetValueStr("listen", "addr", szBuffer, sizeof(szBuffer)))
        {
            int iMaxConn = g_cfg.GetValueInt("listen", "maxconnection", 10000);

            char szAddr[32] = {0};
            int iPort = 0;
            sscanf(szBuffer, "%[^:]:%d", szAddr, &iPort);

            m_lpAuthenServer = GameProvider::Create(true, szAddr, iPort, 0, iMaxConn);

            if (NULL != m_lpAuthenServer)
            {
                if (true == GameProvider::RecvData(m_lpAuthenServer))
                {
                    __log(_DEBUG, "AuthenServer::Start", "listen and start service success, host[%s], port[%d], node = %08x !", szAddr, iPort, m_lpAuthenServer);

                    lResult = 0;
                }
                else
                {
                    __log(_ERROR, "AuthenServer::Start", "RecvData() error !, node = %08x", m_lpAuthenServer);

                    GameProvider::Close(m_lpAuthenServer);
                }
            }
            else
            {
                __log(_ERROR, "AuthenServer::Start", "GameProvider::Create() error, host[%s], port[%d], errno[%d], info[%s] !", szAddr, iPort, errno, strerror(errno));
            }
        }
        else
        {
            __log(_ERROR, "AuthenServer::Start", "g_cfg.GetValueStr() 'addr' error !");
        }
    }
    else
    {
        __log(_ERROR, "AuthenServer::Start", "m_db.Open() error !");
    }

    return lResult;
}

void AuthenServer::OnLoginServerIdNotice(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(LoginServerIdNotice) == length)
    {
        __log(_DEBUG, "AuthenServer::OnLoginServerIdNotify", "login server id [%d]", ((PLoginServerIdNotice)buffer)->ulLoginServerID);

        ((AuthenPlayer*)lpPlayerNode)->m_ulServerID = ((PLoginServerIdNotice)buffer)->ulLoginServerID;
    }
    else
    {
        __log(_DEBUG, "AuthenServer::OnLoginServerIdNotify", "buffer length is not match ! length[%u] != sizeof(%u)", length, sizeof(LoginServerIdNotice));
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnNetMessage(IPlayerNode *lpPlayerNode, unsigned short type, void *buffer, long length)
{
    if (NULL != lpPlayerNode)
    {
        switch (type)
        {
        case LOGIN_SERVER_ID_NOTICE:
            OnLoginServerIdNotice(lpPlayerNode, buffer, length);
            break;

        case LOBBY_AUTHEN_REQ:
            OnLobbyAuthenReq(lpPlayerNode, buffer, length);
            break;
		case LOBBY_AUTHEN_WITH_NO_SITE_ID_REQ:
			OnLobbyAuthenWithNoSiteIdReq(lpPlayerNode, buffer, length);
			break;
        case LOBBY_SET_USER_NICKNAME_REQ:
            OnLobbySetUserNicknameReq(lpPlayerNode, buffer, length);
            break;
		case LOBBY_SEND_SELF_LOCATION:
			OnLobbySendSelfLocationReq(lpPlayerNode, buffer, length);
			break;
        //case MOBILE_AUTHEN_REQ:
            //OnMobileAuthenReq(lpPlayerNode, buffer, length);
            //break;

        case LOBBY_HECHENG_REQ:
            OnLobbyHeChengReq(lpPlayerNode, buffer, length);
            break;

        case LOBBY_GET_GAME_PRIZE_REQ:
            OnLobbyGetGamePrizeReq(lpPlayerNode, buffer, length);
            break;

        case LOBBY_GET_HONGBAO_INFO_REQ:
            OnLobbyGetHongbaoInfoReq(lpPlayerNode, buffer, length);
            break;

        case LOBBY_GET_HONGBAO_CONTENT_REQ:
            OnLobbyGetHongbaoContentReq(lpPlayerNode, buffer, length);
            break;

        case USER_RADIUS_AUTHEN_REQ:
            OnUserRadiusAuthenReq(lpPlayerNode, buffer, length);
            break;

        case GM_DISCONNECT_ACCOUNT_REQ:
            OnDisconnectAccountReq(lpPlayerNode, buffer, length);
            break;

        case GM_USERINFO_DISCONNECT_REQ:
            OnUserInfoDisconnectReq(lpPlayerNode, buffer, length);
            break;

        case RD_USER_LOGIN_BANK_REQ:
            OnUserLoginBankReq(lpPlayerNode, buffer, length);
            break;

        case RD_GET_BANK_COIN_REQ:
            OnGetBankCoinReq(lpPlayerNode, buffer, length);
            break;

        case RD_USER_SAVE_COIN_REQ:
            OnUserSaveCoinReq(lpPlayerNode, buffer, length);
            break;

        case RD_MODIFY_BANK_PASSWD_REQ:
            OnModifyBankPasswdReq(lpPlayerNode, buffer, length);
            break;

        //‘玩就送’活动
        case GM_ACTIVITY_INFO:
            OnDBGetActivityInfo(lpPlayerNode, buffer, length);
            break;

        case GM_DBACCOUNT_TODB:
            OnDBAccountToDB(lpPlayerNode, buffer, length);
            break;

		//手机端银行操作
		case MOBILE_LOGIN_SERVER_BANK_AUTHEN_REQ:
			OnMobileBankAnthenReq(lpPlayerNode, buffer, length);
			break;

		 case GAME_ACTIVITY_HONGBAO_INFO:
            OnUserHongBaoInfo(lpPlayerNode, buffer, length);
            break;
		case MOBILE_LOGIN_SERVER_BANK_GET_COIN_REQ:
			OnMobileBankGetCoinReq(lpPlayerNode, buffer, length);
			break;

		case MOBILE_LOGIN_SERVER_BANK_SAVE_COIN_REQ:
			OnMobileBankSaveCoinReq(lpPlayerNode, buffer, length);
			break;

		case MOBILE_LOGIN_SERVER_BANK_MODIFY_PASSWORD_REQ:
			OnMobileBankModifyPasswdReq(lpPlayerNode, buffer, length);
			break;

		case LOBBY_GET_CAIPIAO_INFO_REQ:
			OnLobbyGetCaiPiaoInfoReq(lpPlayerNode, buffer, length);
			break;

		case LOBBY_MODIFY_LOGIN_PASSWORD_REQ:
			OnLobbyModifyLoginPasswordReq(lpPlayerNode, buffer, length);
			break;

		////////////////////////////// 平台活动相关 //////////////////////////////////
		case SITE_ACTIVITY_PLAYER_PROGRESS_REQ:
			OnSiteActivityPlayProgressReq(lpPlayerNode, buffer, length);
			break;

		case SITE_ACTIVITY_PLAYER_PROGRESS_UPDATE:
			OnSiteActivityPlayProgressUpdate(lpPlayerNode, buffer, length);
			break;

		case SITE_ACTIVITY_PLAYER_LOG_REQ:
			OnSiteActivityPlayLogReq(lpPlayerNode, buffer, length);
			break;

		case SITE_ACTIVITY_PLAYER_AWARD_DB:
			OnSiteActivityAwardReq(lpPlayerNode, buffer, length);
			break;
		//////////////////////////////// 活动 end////////////////////////////////////

		case NM_KEEP_ALIVE:
			break;

		case GAMEFCT_MSG_ONE_PRAISE_ANOTHER_REQ:
			HandleOnePraiseAnotherReq(lpPlayerNode, buffer, length);
			break;
		case GAMEFCT_MSG_ONE_RIDICULE_ANOTHER_REQ:
			HandleOneRidiculeAnotherReq(lpPlayerNode, buffer, length);
			break;
        default:
            __log(_ERROR, "AuthenServer::OnNetMessage", "type invalid ! type[%x]", type);
            GameProvider::Close(lpPlayerNode);
            break;
        }
    }
    else
    {
        __log(_ERROR, "AuthenServer::OnNetMessage", "lpPlayerNode = NULL, type[%x]", type);
    }
}

void AuthenServer::OnClosed(IPlayerNode *lpPlayerNode, bool bClosed)
{
    if (lpPlayerNode == m_lpAuthenServer)
    {
        __log(_ERROR, "AuthenServer::OnClosed", "authen server has been closed !, bClosed[%d]", bClosed);
    }
    else if (0 != ((AuthenPlayer*)lpPlayerNode)->m_ulServerID)
    {
        __log(_ERROR, "AuthenServer::OnClosed", "server has been closed !, bClosed[%d], server id [%d]", bClosed, ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
    }

    GameProvider::OnClosed(lpPlayerNode, bClosed);
}

void AuthenServer::OnTimer(long tmNow)
{

}

IPlayerNode* AuthenServer::OnCreatePlayer()
{
    return new AuthenPlayer;
}

