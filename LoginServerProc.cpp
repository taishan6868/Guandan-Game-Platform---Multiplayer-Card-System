#include "AuthenServer.h"
#include "AuthenPlayer.h"
#include "authen_server_msg.h"

//登录服务器转发大厅客户端的消息处理

void AuthenServer::OnLobbyAuthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(LobbyAuthenReq_Trans) == length)
    {
        PLobbyAuthenReq_Trans pAuthenReq = (PLobbyAuthenReq_Trans)buffer;

		LobbyAuthenRes_Trans msgRes;
        LobbyAuthenRes_2_Trans msgRes2;

		pAuthenReq->szUserName[sizeof(pAuthenReq->szUserName) - 1] = 0;
		pAuthenReq->szPassword[sizeof(pAuthenReq->szPassword) - 1] = 0;

		pAuthenReq->szChannelID[sizeof(pAuthenReq->szChannelID) - 1] = 0;
		pAuthenReq->szVersionStr[sizeof(pAuthenReq->szVersionStr) - 1] = 0;

		long lType, lResult = m_db.login_authen_accountinfo_select(*pAuthenReq, msgRes);

		if (0 == lResult) //执行成功
		{
			msgRes.ulPlayerNode = pAuthenReq->ulPlayerNode;
			msgRes.ulPlayerNodeId = pAuthenReq->ulPlayerNodeId;
  
			lType = LOBBY_AUTHEN_RES;
			buffer = &msgRes;
			length = sizeof(msgRes);
		}
		else
		{
			msgRes2.lError = (LOBBY_AUTHEN_ERROR)lResult;
			msgRes2.ulPlayerNode = pAuthenReq->ulPlayerNode;
			msgRes2.ulPlayerNodeId = pAuthenReq->ulPlayerNodeId;

			lType = LOBBY_AUTHEN_RES_2;
			buffer = &msgRes2;
			length = sizeof(msgRes2);
		}

        if (true == GameProvider::SendData(lpPlayerNode, lType, buffer, length))
        {
            __log(_DEBUG, "LoginServerProc::OnLobbyAuthenReq", "server_id[%u], m_db.login_authen_accountinfo_select() = %d, user_name[%s], pwd[%s], send success !,iUserID[%u],nScore[%lld],ulCoin2Award[%u],ulCompeteScore[%u],ulAwardScore[%u],ucGender[%d],IsSetNickname[%d],Nickname[%s]",
                            ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pAuthenReq->szUserName, pAuthenReq->szPassword,msgRes.ulUserID,
                             msgRes.nScore,msgRes.ulCoin2Award,msgRes.ulCompeteScore,msgRes.ulAwardScore,msgRes.ucGender,msgRes.ucIsSetNickname,msgRes.szNickname);
        }
        else
        {
            __log(_ERROR, "LoginServerProc::OnLobbyAuthenReq", "server_id[%u], m_db.login_authen_accountinfo_select() = %d, send failed, user_name[%s], pwd[%s] !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pAuthenReq->szUserName, pAuthenReq->szPassword);
        }
    }
    else
    {
        __log(_ERROR, "LoginServerProc::OnLobbyAuthenReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !", length, sizeof(LobbyAuthenReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnLobbyAuthenWithNoSiteIdReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(LobbyAuthenWithNoSiteIdReq_Trans) == length)
	{
		PLobbyAuthenWithNoSiteIdReq_Trans pAuthenReq = (PLobbyAuthenWithNoSiteIdReq_Trans)buffer;

		LobbyAuthenWithNoSiteIdRes_Trans msgRes;
		LobbyAuthenRes_2_Trans msgRes2;

		pAuthenReq->szUserName[sizeof(pAuthenReq->szUserName) - 1] = 0;
		pAuthenReq->szPassword[sizeof(pAuthenReq->szPassword) - 1] = 0;

		long lType, lResult = m_db.login_authen_with_no_siteid_accountinfo_select(*pAuthenReq, msgRes);

		__log(_DEBUG, "LoginServerProc::OnLobbyAuthenWithNoSiteIdReq", "不知道站点的情况下，登录服务器返回的站点ID【%d】lResult[%ld]", msgRes.siteId, lResult);
		if (0 == lResult) //执行成功
		{
			msgRes.ulPlayerNode = pAuthenReq->ulPlayerNode;
			msgRes.ulPlayerNodeId = pAuthenReq->ulPlayerNodeId;
			__log(_DEBUG, "LoginServerProc::OnLobbyAuthenWithNoSiteIdReq", "不知道站点的情况下，登录成功，服务器返回的站点ID【%d】", msgRes.siteId);

			lType = LOBBY_AUTHEN_WITH_NO_SITE_ID_RES;
			buffer = &msgRes;
			length = sizeof(msgRes);
		}
		else
		{
			msgRes2.lError = (LOBBY_AUTHEN_ERROR)lResult;
			msgRes2.ulPlayerNode = pAuthenReq->ulPlayerNode;
			msgRes2.ulPlayerNodeId = pAuthenReq->ulPlayerNodeId;

			lType = LOBBY_AUTHEN_RES_2;
			buffer = &msgRes2;
			length = sizeof(msgRes2);
		}

		if (true == GameProvider::SendData(lpPlayerNode, lType, buffer, length))
		{
			__log(_DEBUG, "LoginServerProc::OnLobbyAuthenWithNoSiteIdReq", "server_id[%u], m_db.login_authen_accountinfo_select() = %d, user_name[%s], pwd[%s], send success !,iUserID[%u],nScore[%lld],ulCoin2Award[%u],ulCompeteScore[%u],ulAwardScore[%u],ucGender[%d]",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pAuthenReq->szUserName, pAuthenReq->szPassword,msgRes.ulUserID,
				msgRes.nScore,msgRes.ulCoin2Award,msgRes.ulCompeteScore,msgRes.ulAwardScore,msgRes.ucGender);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnLobbyAuthenWithNoSiteIdReq", "server_id[%u], m_db.login_authen_accountinfo_select() = %d, send failed, user_name[%s], pwd[%s] !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pAuthenReq->szUserName, pAuthenReq->szPassword);
		}
	}
	else
	{
		__log(_ERROR, "LoginServerProc::OnLobbyAuthenWithNoSiteIdReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !", length, sizeof(LobbyAuthenReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
		GameProvider::Close(lpPlayerNode);
	}
}

void AuthenServer::OnLobbySetUserNicknameReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(LobbySetNicknameReq_Trans) == length)
	{
		LobbySetNicknameReq_Trans *pReq = (LobbySetNicknameReq_Trans*)buffer;

		LobbySetNicknameRsp_Trans Rsp;
		memset(&Rsp, 0, sizeof(LobbySetNicknameRsp_Trans));

		long lResult = m_db.SPSetUserNickname(*pReq, Rsp);
		
		Rsp.ulPlayerNode = pReq->ulPlayerNode;
		Rsp.ulPlayerNodeId = pReq->ulPlayerNodeId;

		Rsp.iUserID = pReq->iUserID;
		Rsp.iErrorCode = lResult;
		snprintf(Rsp.szNickname, NAME_LEN-1, "%s", pReq->szNickname);

		if (true == GameProvider::SendData(lpPlayerNode, LOBBY_SET_USER_NICKNAME_RSP, &Rsp, sizeof(LobbySetNicknameRsp_Trans)))
		{
			__log(_DEBUG, "LoginServerProc::OnLobbySetUserNicknameReq", "server_id[%u], Result[%d], UserID[%d], Nickname[%s], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pReq->iUserID, pReq->szNickname);
		}
		else
		{
			__log(_DEBUG, "LoginServerProc::OnLobbySetUserNicknameReq", "server_id[%u], Result[%d], UserID[%d], Nickname[%s], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pReq->iUserID, pReq->szNickname);
		}
	}
	else
	{
		__log(_ERROR, "LoginServerProc::OnLobbySetUserNicknameReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !", length, sizeof(LobbySetNicknameReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
		GameProvider::Close(lpPlayerNode);
	}
}

void AuthenServer::OnLobbySendSelfLocationReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(LobbySendSelfLocationReq_Trans) == length)
	{
		PLobbySendSelfLocationReq_Trans pAuthenReq = (PLobbySendSelfLocationReq_Trans)buffer;

		long lType, lResult = m_db.login_send_self_location(*pAuthenReq);

		if (lResult == 0)
		{
			__log(_ERROR, "LoginServerProc::OnLobbySendSelfLocationReq", "save success!");
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnLobbySendSelfLocationReq", "save failed!");
		}
	}
	else
	{
		__log(_ERROR, "LoginServerProc::OnLobbySendSelfLocationReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !", length, sizeof(LobbySendSelfLocationReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
		GameProvider::Close(lpPlayerNode);
	}
}

/*
void AuthenServer::OnMobileAuthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(MobileAuthenReq_Trans) == length)
    {
        PMobileAuthenReq_Trans pAuthenReq = (PMobileAuthenReq_Trans)buffer;
        MobileAuthenRes_Trans msgRes;
        MobileAuthenRes_2_Trans msgRes2;
        pAuthenReq->szPassword[sizeof(pAuthenReq->szPassword) - 1] = 0;

        long lType, lResult = m_db.mobile_authen_accountinfo_select(pAuthenReq->szUserName, pAuthenReq->szPassword, msgRes);

        if (0 == lResult) //执行成功
        {
            msgRes.ulPlayerNode = pAuthenReq->ulPlayerNode;
            msgRes.ulPlayerNodeId = pAuthenReq->ulPlayerNodeId;
            lType = MOBILE_AUTHEN_RES;
            buffer = &msgRes;
            length = sizeof(msgRes);
        }
        else
        {
            msgRes2.lError = (LOBBY_AUTHEN_ERROR)lResult;
            msgRes2.ulPlayerNode = pAuthenReq->ulPlayerNode;
            msgRes2.ulPlayerNodeId = pAuthenReq->ulPlayerNodeId;
            lType = MOBILE_AUTHEN_RES_2;
            buffer = &msgRes2;
            length = sizeof(msgRes2);
        }

        if (true == GameProvider::SendData(lpPlayerNode, lType, buffer, length))
        {
            __log(_DEBUG, "LoginServerProc::OnMobileAuthenReq", "server_id[%u], m_db.mobile_authen_accountinfo_select() = %d, user_name[%s], pwd[%s], send success !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pAuthenReq->szUserName, pAuthenReq->szPassword);
        }
        else
        {
            __log(_ERROR, "LoginServerProc::OnMobileAuthenReq", "server_id[%u], m_db.mobile_authen_accountinfo_select() = %d, send failed, user_name[%s], pwd[%s] !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pAuthenReq->szUserName, pAuthenReq->szPassword);
        }
    }
    else
    {
        __log(_ERROR, "LoginServerProc::OnMobileAuthenReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
                length, sizeof(MobileAuthenReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
        GameProvider::Close(lpPlayerNode);
    }
}
*/
void AuthenServer::OnLobbyHeChengReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(LobbyHeChengReq_Trans) == length)
    {
        PLobbyHeChengReq_Trans pHeChengReq = (PLobbyHeChengReq_Trans)buffer;
        LobbyHeCheng_Trans msgRes;
        LobbyHeCheng_2_Trans msgRes2;

        memset(&msgRes, 0, sizeof(msgRes));
        msgRes.ulAwardScore = pHeChengReq->cChoose;

        long lType, lResult = m_db.login_combin_prize(*pHeChengReq, msgRes);

        if (0 == lResult) //执行成功
        {
            msgRes.ulPlayerNode = pHeChengReq->ulPlayerNode;
            msgRes.ulPlayerNodeId = pHeChengReq->ulPlayerNodeId;
            lType = LOBBY_HECHEGN_RES;
            buffer = &msgRes;
            length = sizeof(msgRes);
        }
        else
        {
            msgRes2.lError = (LOBBY_HECHENG_ERROR_DEF)lResult;
            msgRes2.ulPlayerNode = pHeChengReq->ulPlayerNode;
            msgRes2.ulPlayerNodeId = pHeChengReq->ulPlayerNodeId;
            lType = LOBBY_HECHENG_RES_2;
            buffer = &msgRes2;
            length = sizeof(msgRes2);
        }

        if (true == GameProvider::SendData(lpPlayerNode, lType, buffer, length))
        {
            __log(_DEBUG, "LoginServerProc::OnLobbyHeChengReq", "server_id[%u], m_db.login_combin_prize() = %d, userid[%d], send success !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pHeChengReq->iUserID);
        }
        else
        {
            __log(_ERROR, "LoginServerProc::OnLobbyHeChengReq", "server_id[%u], m_db.login_combin_prize() = %d, userid[%d], send failed !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pHeChengReq->iUserID);
        }
    }
    else
    {
        __log(_ERROR, "LoginServerProc::OnLobbyHeChengReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
                length, sizeof(LobbyHeChengReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnLobbyGetGamePrizeReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(LobbyGetGamePrizeReq_Trans) == length)
    {
        PLobbyGetGamePrizeReq_Trans pGetPrizeReq = (PLobbyGetGamePrizeReq_Trans)buffer;
        LobbyGetGamePrizeRes_Trans msgRes;

        memset(&msgRes, 0, sizeof(msgRes));

        long lResult = m_db.login_get_game_prize(*pGetPrizeReq, msgRes);

        msgRes.ulPlayerNode = pGetPrizeReq->ulPlayerNode;
        msgRes.ulPlayerNodeId = pGetPrizeReq->ulPlayerNodeId;

        if (true == GameProvider::SendData(lpPlayerNode, LOBBY_GET_GAME_PRIZE_RES, &msgRes, sizeof(msgRes)))
        {
            __log(_DEBUG, "LoginServerProc::OnLobbyGetGamePrizeReq", "server_id[%u], m_db.login_get_game_prize() = %d, userid[%d], send success !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pGetPrizeReq->iUserID);
        }
        else
        {
            __log(_ERROR, "LoginServerProc::OnLobbyGetGamePrizeReq", "server_id[%u], m_db.login_get_game_prize() = %d, userid[%d], send failed !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pGetPrizeReq->iUserID);
        }
    }
    else
    {
        __log(_ERROR, "LoginServerProc::OnLobbyGetGamePrizeReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
                length, sizeof(LobbyGetGamePrizeReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnLobbyGetCaiPiaoInfoReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(LobbyGetCaiPiaoInfoReq_Trans) == length)
	{
		LobbyGetCaiPiaoInfoReq_Trans *pGetCaiPiaoReq = (LobbyGetCaiPiaoInfoReq_Trans *)buffer;
		LobbyGetCaiPiaoInfoRes_Trans msgRes;

		long lResult = m_db.login_get_caipiao_info(*pGetCaiPiaoReq, msgRes);

		if (0 == lResult)
		{
			msgRes.ulPlayerNode = pGetCaiPiaoReq->ulPlayerNode;
			msgRes.ulPlayerNodeId = pGetCaiPiaoReq->ulPlayerNodeId;

			msgRes.nUserID = pGetCaiPiaoReq->nUserID;

			if (false == GameProvider::SendData(lpPlayerNode, LOBBY_GET_CAIPIAO_INFO_RES, &msgRes, sizeof(msgRes)))
			{
				__log(_ERROR, "LoginServerProc::OnLobbyGetCaiPiaoInfoReq", "send lobby_get_game_prize_res failed !");
			}
		}
	}
	else
	{
		__log(_ERROR, "LoginServerProc::OnLobbyGetCaiPiaoInfoReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
			length, sizeof(LobbyGetCaiPiaoInfoReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);

		GameProvider::Close(lpPlayerNode);
	}
}

void AuthenServer::OnLobbyGetHongbaoInfoReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(LobbyGetHongbaoInfoReq_Trans) == length)
    {
        PLobbyGetHongbaoInfoReq_Trans pGetHongbaoReq = (PLobbyGetHongbaoInfoReq_Trans)buffer;
        LobbyGetHongbaoInfoRes_Trans msgRes;

        LobbyHongbaoInfo info[10000];
        unsigned long ulCount = sizeof(info) / sizeof(info[0]);

        long lResult = m_db.login_get_hongbao_info(*pGetHongbaoReq, info, ulCount);

        if (0 == lResult)
        {
            __log(_DEBUG, "LoginServerProc::OnLobbyGetHongbaoInfoReq", "get hong bao count[%d]", ulCount);

            msgRes.iCount = 0;
            msgRes.ulPlayerNode = pGetHongbaoReq->ulPlayerNode;
            msgRes.ulPlayerNodeId = pGetHongbaoReq->ulPlayerNodeId;

            for (int i = 0; i < ulCount; i++)
            {
                msgRes.info[msgRes.iCount++] = info[i];
                if (msgRes.iCount == sizeof(msgRes.info) / sizeof(msgRes.info[0]))
                {
                    if (false == GameProvider::SendData(lpPlayerNode, LOBBY_GET_HONGBAO_INFO_RES, &msgRes, sizeof(msgRes)))
                    {
                        __log(_ERROR, "LoginServerProc::OnLobbyGetHongbaoInfoReq", "send lobby_get_game_prize_res failed !");
                        break;
                    }
                    msgRes.iCount = 0;
                }
            }

            if (0 < msgRes.iCount)
            {
                if (false == GameProvider::SendData(lpPlayerNode, LOBBY_GET_HONGBAO_INFO_RES, &msgRes, sizeof(msgRes)))
                {
                    __log(_ERROR, "LoginServerProc::OnLobbyGetHongbaoInfoReq", "send lobby_get_game_prize_res failed !");
                }
            }
        }
        else
        {

        }
    }
    else
    {
        __log(_ERROR, "LoginServerProc::OnLobbyGetHongbaoInfoReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
                length, sizeof(LobbyGetHongbaoInfoReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnLobbyGetHongbaoContentReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(LobbyGetHongbaoContentReq_Trans) == length)
    {
        PLobbyGetHongbaoContentReq_Trans pGetHongbaoReq = (PLobbyGetHongbaoContentReq_Trans)buffer;
        LobbyGetHongbaoContentRes_Trans msgRes;

        long lResult = m_db.login_get_hongbao_content(*pGetHongbaoReq);

        msgRes.ulPlayerNode = pGetHongbaoReq->ulPlayerNode;
        msgRes.ulPlayerNodeId = pGetHongbaoReq->ulPlayerNodeId;
        msgRes.iErrorCode = lResult;

        if (true == GameProvider::SendData(lpPlayerNode, LOBBY_GET_HONGBAO_CONTENT_RES, &msgRes, sizeof(msgRes)))
        {
            __log(_DEBUG, "LoginServerProc::OnLobbyGetHongbaoContentReq", "server_id[%u], m_db.login_get_hongbao_content() = %d, userid[%d], send success !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pGetHongbaoReq->iUserID);
        }
        else
        {
            __log(_ERROR, "LoginServerProc::OnLobbyGetHongbaoContentReq", "server_id[%u], m_db.login_get_hongbao_content() = %d, userid[%d], send failed !",
                    ((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pGetHongbaoReq->iUserID);
        }
    }
    else
    {
        __log(_ERROR, "LoginServerProc::OnLobbyGetHongbaoContentReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
                length, sizeof(LobbyGetHongbaoContentReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnMobileBankAnthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	//assert (sizeof(MobileLoginServerBankAuthenReq) == length);
	if (sizeof(MobileLoginServerBankAuthenReq) != length)
	{
		__log(_ERROR, "LoginServerProc::OnMobileBankAnthenReq", 
			"buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
			length, sizeof(MobileLoginServerBankAuthenReq), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);

		GameProvider::Close(lpPlayerNode);
		return;
	}

	MobileLoginServerBankAuthenReqPtr pBankAuthen = (MobileLoginServerBankAuthenReqPtr)buffer;
	MobileLoginServerBankAuthenSuccessRsp BankAuthenSuc;
	memset(&BankAuthenSuc, 0, sizeof(BankAuthenSuc));

	long lResult = m_db.mobile_bank_authen(pBankAuthen, BankAuthenSuc);

	//认证成功
	if (0 == lResult)
	{
		BankAuthenSuc.ulPlayerNode = pBankAuthen->ulPlayerNode;
		BankAuthenSuc.ulPlayerNodeId = pBankAuthen->ulPlayerNodeId;
		
		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_AUTHEN_SUCCESS_RSP, &BankAuthenSuc, sizeof(BankAuthenSuc)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankAnthenReq", "server_id[%u], m_db.mobile_bank_authen() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankAuthen->iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankAnthenReq", "server_id[%u], m_db.mobile_bank_authen() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankAuthen->iUserID);
		}
	} 
	else
	{
		MobileLoginServerBankAuthenErrorRsp BankAuthenErr;
		memset(&BankAuthenErr, 0, sizeof(BankAuthenErr));

		BankAuthenErr.ulPlayerNode = pBankAuthen->ulPlayerNode;
		BankAuthenErr.ulPlayerNodeId = pBankAuthen->ulPlayerNodeId;

		BankAuthenErr.lError = ERROR_LOBBY_BANK_CLIENT_AUTHEN_BANK_PASS_ERROR;

		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_AUTHEN_ERROR_RSP, &BankAuthenErr, sizeof(BankAuthenErr)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankAnthenReq", "server_id[%u], m_db.mobile_bank_authen() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankAuthen->iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankAnthenReq", "server_id[%u], m_db.mobile_bank_authen() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankAuthen->iUserID);
		}
	}
}

void AuthenServer::OnMobileBankGetCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	//assert (sizeof(MobileLoginServerBankGetCoinReq) == length);
	if (sizeof(MobileLoginServerBankGetCoinReq) != length)
	{
		__log(_ERROR, "LoginServerProc::OnMobileBankGetCoinReq", 
			"buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
			length, sizeof(MobileLoginServerBankGetCoinReq), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);

		GameProvider::Close(lpPlayerNode);
		return;
	}

	MobileLoginServerBankGetCoinReqPtr pBankGetCoin = (MobileLoginServerBankGetCoinReqPtr)buffer;
	MobileLoginServerBankGetCoinSuccessRsp BankGetCoinSuc;
	memset(&BankGetCoinSuc, 0, sizeof(BankGetCoinSuc));

	long lResult = m_db.mobile_bank_get_coin(pBankGetCoin, BankGetCoinSuc);

	//认证成功
	if (0 == lResult)
	{
		BankGetCoinSuc.llGetNum = pBankGetCoin->llGetNum;
		BankGetCoinSuc.ulPlayerNode = pBankGetCoin->ulPlayerNode;
		BankGetCoinSuc.ulPlayerNodeId = pBankGetCoin->ulPlayerNodeId;

		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_GET_COIN_SUCCESS_RSP, &BankGetCoinSuc, sizeof(BankGetCoinSuc)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankGetCoinReq", "server_id[%u], m_db.mobile_bank_get_coin() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankGetCoin->authen.iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankGetCoinReq", "server_id[%u], m_db.mobile_bank_get_coin() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankGetCoin->authen.iUserID);
		}
	} 
	else
	{
		MobileLoginServerBankGetCoinErrorRsp BankGetCoinErr;
		memset(&BankGetCoinErr, 0, sizeof(BankGetCoinErr));

		BankGetCoinErr.ulPlayerNode = pBankGetCoin->ulPlayerNode;
		BankGetCoinErr.ulPlayerNodeId = pBankGetCoin->ulPlayerNodeId;
		
		switch (lResult)
		{
			//@ERROR_USERID_LOGIN_SOMEWHERE=1
		case 1:	BankGetCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_USERID_LOGIN_SOMEWHERE_ELSE; break;	
			//@ERROR_NOFIND_USERID_ERROR=2
		case 2:	BankGetCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_NO_BANK_RECORD_OF_THE_USERID; break;	
			//@ERROR_NOMORE_ENOUGH_COIN=4
		case 4:	BankGetCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_GET_SAVE_NOENOUGH_COIN; break;	
			//@ERROR_DATEBASE_ERROR=3
		default: BankGetCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_SYSTEM_BUSY; break;
		}

		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_GET_COIN_ERROR_RSP, &BankGetCoinErr, sizeof(BankGetCoinErr)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankGetCoinReq", "server_id[%u], m_db.mobile_bank_get_coin() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankGetCoin->authen.iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankGetCoinReq", "server_id[%u], m_db.mobile_bank_get_coin() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankGetCoin->authen.iUserID);
		}
	}
}

void AuthenServer::OnMobileBankSaveCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	//assert (sizeof(MobileLoginServerBankSaveCoinReq) == length);
	if (sizeof(MobileLoginServerBankSaveCoinReq) != length)
	{
		__log(_ERROR, "LoginServerProc::OnMobileBankSaveCoinReq", 
			"buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
			length, sizeof(MobileLoginServerBankSaveCoinReq), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);

		GameProvider::Close(lpPlayerNode);
		return;
	}

	MobileLoginServerBankSaveCoinReqPtr pBankSaveCoin = (MobileLoginServerBankSaveCoinReqPtr)buffer;
	MobileLoginServerBankSaveCoinSuccessRsp BankSaveCoinSuc;
	memset(&BankSaveCoinSuc, 0, sizeof(BankSaveCoinSuc));

	long lResult = m_db.mobile_bank_save_coin(pBankSaveCoin, BankSaveCoinSuc);

	//认证成功
	if (0 == lResult)
	{
		BankSaveCoinSuc.llSaveNum = pBankSaveCoin->llSaveNum;

		BankSaveCoinSuc.ulPlayerNode = pBankSaveCoin->ulPlayerNode;
		BankSaveCoinSuc.ulPlayerNodeId = pBankSaveCoin->ulPlayerNodeId;

		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_SAVE_COIN_SUCCESS_RSP, &BankSaveCoinSuc, sizeof(BankSaveCoinSuc)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankSaveCoinReq", "server_id[%u], m_db.mobile_bank_save_coin() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankSaveCoin->authen.iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankSaveCoinReq", "server_id[%u], m_db.mobile_bank_save_coin() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankSaveCoin->authen.iUserID);
		}
	} 
	else
	{
		MobileLoginServerBankSaveCoinErrorRsp BankSaveCoinErr;
		memset(&BankSaveCoinErr, 0, sizeof(BankSaveCoinErr));

		BankSaveCoinErr.ulPlayerNode = pBankSaveCoin->ulPlayerNode;
		BankSaveCoinErr.ulPlayerNodeId = pBankSaveCoin->ulPlayerNodeId;

		switch (lResult)
		{
			//@ERROR_USERID_LOGIN_SOMEWHERE=1
		case 1:	BankSaveCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_USERID_LOGIN_SOMEWHERE_ELSE; break;	
			//@ERROR_NOFIND_USERID_ERROR=2
		case 2:	BankSaveCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_NO_BANK_RECORD_OF_THE_USERID; break;	
			//@ERROR_NOMORE_ENOUGH_COIN=4
		case 4:	BankSaveCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_GET_SAVE_NOENOUGH_COIN; break;	
			//@ERROR_DATEBASE_ERROR=3
		default: BankSaveCoinErr.lError = ERROR_LOBBY_BANK_CLIENT_SYSTEM_BUSY; break;
		}

		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_SAVE_COIN_ERROR_RSP, &BankSaveCoinErr, sizeof(BankSaveCoinErr)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankSaveCoinReq", "server_id[%u], m_db.mobile_bank_save_coin() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankSaveCoin->authen.iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankSaveCoinReq", "server_id[%u], m_db.mobile_bank_save_coin() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankSaveCoin->authen.iUserID);
		}
	}
}

void AuthenServer::OnMobileBankModifyPasswdReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	//assert (sizeof(MobileLoginServerBankModifyPasswordReq) == length);
	if (sizeof(MobileLoginServerBankModifyPasswordReq) != length)
	{
		__log(_ERROR, "LoginServerProc::OnMobileBankModifyPasswdReq", 
			"buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
			length, sizeof(MobileLoginServerBankModifyPasswordReq), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);

		GameProvider::Close(lpPlayerNode);
		return;
	}

	MobileLoginServerBankModifyPasswordReqPtr pBankModifyPwd = (MobileLoginServerBankModifyPasswordReqPtr)buffer;	

	long lResult = m_db.mobile_bank_modify_password(pBankModifyPwd);

	//认证成功
	if (0 == lResult)
	{
		MobileLoginServerBankModifyPasswordSuccessRsp BankModifyPwdSuc;
		memset(&BankModifyPwdSuc, 0, sizeof(BankModifyPwdSuc));

		BankModifyPwdSuc.ulPlayerNode = pBankModifyPwd->ulPlayerNode;
		BankModifyPwdSuc.ulPlayerNodeId = pBankModifyPwd->ulPlayerNodeId;

		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_MODIFY_PASSWORD_SUCCESS_RSP, &BankModifyPwdSuc, sizeof(BankModifyPwdSuc)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankModifyPasswdReq", "server_id[%u], m_db.mobile_bank_modify_password() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankModifyPwd->authen.iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankModifyPasswdReq", "server_id[%u], m_db.mobile_bank_modify_password() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankModifyPwd->authen.iUserID);
		}
	} 
	else
	{
		MobileLoginServerBankModifyPasswordErrorRsp BankModifyPwdErr;
		memset(&BankModifyPwdErr, 0, sizeof(BankModifyPwdErr));

		BankModifyPwdErr.ulPlayerNode = pBankModifyPwd->ulPlayerNode;
		BankModifyPwdErr.ulPlayerNodeId = pBankModifyPwd->ulPlayerNodeId;

		switch (lResult)
		{
			//@ERROR_NOFIND_USERID_ERROR=1
		case 1:	BankModifyPwdErr.lError = ERROR_LOBBY_BANK_CLIENT_NO_BANK_RECORD_OF_THE_USERID; break;	
			//@ERROR_PASSWORD_ERROR=2
		case 2:	BankModifyPwdErr.lError = ERROR_LOBBY_BANK_CLIENT_OLD_PASSWD_DIFFERENT; break;	
			//@ERROR_DATEBASE_ERROR=3
		default: BankModifyPwdErr.lError = ERROR_LOBBY_BANK_CLIENT_SYSTEM_BUSY; break;
		}

		if (true == GameProvider::SendData(lpPlayerNode, MOBILE_LOGIN_SERVER_BANK_MODIFY_PASSWORD_ERROR_RSP, &BankModifyPwdErr, sizeof(BankModifyPwdErr)))
		{
			__log(_DEBUG, "LoginServerProc::OnMobileBankModifyPasswdReq", "server_id[%u], m_db.mobile_bank_modify_password() = %d, userid[%d], send success !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankModifyPwd->authen.iUserID);
		}
		else
		{
			__log(_ERROR, "LoginServerProc::OnMobileBankModifyPasswdReq", "server_id[%u], m_db.mobile_bank_modify_password() = %d, userid[%d], send failed !",
				((AuthenPlayer*)lpPlayerNode)->m_ulServerID, lResult, pBankModifyPwd->authen.iUserID);
		}
	}
}

void AuthenServer::OnLobbyModifyLoginPasswordReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(LobbyModifyLoginPasswordReq_Trans) != length)
	{
		__log(_ERROR, "LoginServerProc::OnLobbyModifyLoginPasswordReq", "buffer length is not match ! length[%u] != sizeof(%u), server_id[%x] !",
			length, sizeof(LobbyModifyLoginPasswordReq_Trans), ((AuthenPlayer*)lpPlayerNode)->m_ulServerID);

		GameProvider::Close(lpPlayerNode);
		return;
	}

	LobbyModifyLoginPasswordReq_Trans *reqMsg = (LobbyModifyLoginPasswordReq_Trans*)buffer;
	LobbyModifyLoginPasswordRes_Trans resMsg;

	long lResult = m_db.lobby_modify_login_password(reqMsg);

	__log(_DEBUG, "LoginServerProc::OnLobbyModifyLoginPasswordReq", "lResult = %ld", lResult);

	resMsg.iErrorCode = lResult;

	resMsg.ulPlayerNode = reqMsg->ulPlayerNode;
	resMsg.ulPlayerNodeId = reqMsg->ulPlayerNodeId;

	if (true == GameProvider::SendData(lpPlayerNode, LOBBY_MODIFY_LOGIN_PASSWORD_RES, &resMsg, sizeof(resMsg)))
	{
		__log(_DEBUG, "LoginServerProc::OnLobbyModifyLoginPasswordReq", "msg LOBBY_MODIFY_LOGIN_PASSWORD_REQ, send success !");
	}
	else
	{
		__log(_ERROR, "LoginServerProc::OnLobbyModifyLoginPasswordReq", "msg LOBBY_MODIFY_LOGIN_PASSWORD_REQ, send failed !");
	}
}


