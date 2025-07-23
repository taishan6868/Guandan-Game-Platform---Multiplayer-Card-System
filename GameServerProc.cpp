#include "AuthenServer.h"
#include "AuthenPlayer.h"
#include "Odao_Authen_Msg.h"
#include "BankInfoDef_Trans.h"

//游戏服务器转发游戏客户端的消息处理
void AuthenServer::OnUserRadiusAuthenReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(DBAuthenReq) == length)
    {
        DBAuthenReq *pReq = (DBAuthenReq*)buffer;

        DBAuthenRes msgRes = {0};
        DBAuthenRes_2 msgRes_2 = {0};

        msgRes_2.usPlayGameID = pReq->usGameID;
        msgRes_2.usPlayServerID = pReq->usServerID;

        long lType, lResult = m_db.game_authen_accountinfo_select(pReq->iUserID, pReq->szPasswd, msgRes, msgRes_2);

        if ((0 != lResult) || (0 != msgRes_2.usPlayGameID)) //执行失败, 或者重入验证
        {
			msgRes_2.iUserID = pReq->iUserID;
            msgRes_2.cReturnType = lResult;
            msgRes_2.ulPlayerNode = pReq->ulPlayerNode;
            msgRes_2.ulPlayerNodeId = pReq->ulPlayerNodeId;

            lType = USER_RADIUS_AUTHEN_RES2;
            buffer = &msgRes_2;
            length = sizeof(msgRes_2);
        }
        else
        {
            msgRes.ulPlayerNode = pReq->ulPlayerNode;
            msgRes.ulPlayerNodeId = pReq->ulPlayerNodeId;

			msgRes.iUserID = pReq->iUserID;

            lType = USER_RADIUS_AUTHEN_RES;
            buffer = &msgRes;
            length = sizeof(msgRes);
        }

        if (true == GameProvider::SendData(lpPlayerNode, lType, buffer, length))
        {
            __log(_DEBUG, "GameServerProc::OnUserRadiusAuthenReq", "req_game_id[%d], req_server_id[%u], user_id[%d], online_game_id[%u], online_server_id[%u], "
                    "game_coin[%lld], bank_coin[%lld], result[%d], send success !",
                    pReq->usGameID, pReq->usServerID, pReq->iUserID, msgRes_2.usPlayGameID, msgRes_2.usPlayServerID, msgRes.iFirstMoney, msgRes.iBankMoney, lResult);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnUserRadiusAuthenReq", "req_game_id[%d], req_server_id[%u], user_id[%d], online_game_id[%u], online_server_id[%u], "
                    "game_coin[%lld], bank_coin[%lld], result[%d], send failed !",
                    pReq->usGameID, pReq->usServerID, pReq->iUserID, msgRes_2.usPlayGameID, msgRes_2.usPlayServerID, msgRes.iFirstMoney, msgRes.iBankMoney, lResult);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnUserRadiusAuthenReq", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(DBAuthenReq));
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnDisconnectAccountReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(RadiusDisconnect) == length)
    {
        RadiusDisconnect *pReq = (RadiusDisconnect*)buffer;

        long lResult = m_db.game_cost_accountinfo_update(*pReq);

        if (0 == lResult)
        {
            __log(_DEBUG, "GameServerProc::OnDisconnectAccountReq", "user_id[%u], game_id[%u], clean_flag[%d], llRDMoney[%lld], iRDWinMoney[%lld], iRDAmount[%d], iRDLostMoney[%d], "
                    "iRDPunishMoney[%d], iRDPunishNum[%d], iRDWinNum[%d], iRDLostNum[%d], iRDDrawNum[%d], iRDAllPlayCoin[%d], iRDAllWinCoin[%d], iRDCtrValueA[%d], iRDCtrValueB[%d],"
                    "iRDControlNum[%d], iRDLotteryCtrNum[%d], iRDPropNum[%d], iRDTaskNUm[%d], iRDExpValue[%d], iRDOnlineTime[%d], iRDPlayTime[%d], iRDPrizeNum[%d], iRDGameExpVlue[%d], iMatchTicket[%d], "
                    "iTakeCount[%d], llGameScore[%lld], llMatchScore[%lld], iSecondMoney[%d], iRDYuanBao[%d], cMobileType[%d]",
                    pReq->iUserID, pReq->playBaseInfo.iGameID, pReq->playBaseInfo.cCleanFlag, pReq->playBaseInfo.llRDMoney, pReq->playBaseInfo.iRDWinMoney, pReq->playBaseInfo.iRDAmount,
                    pReq->playBaseInfo.iRDLostMoney, pReq->playBaseInfo.iRDPunishMoney, pReq->playBaseInfo.iRDPunishNum, pReq->playBaseInfo.iRDWinNum, pReq->playBaseInfo.iRDLostNum,
                    pReq->playBaseInfo.iRDDrawNum, pReq->playBaseInfo.iRDAllPlayCoin, pReq->playBaseInfo.iRDAllWinCoin, pReq->playBaseInfo.iRDCtrValueA, pReq->playBaseInfo.iRDCtrValueB,
                    pReq->playBaseInfo.iRDControlNum, pReq->playBaseInfo.iRDLotteryCtrNum, pReq->playBaseInfo.iRDPropNum, pReq->playBaseInfo.iRDTaskNUm, pReq->playBaseInfo.iRDExpValue,
                    pReq->playBaseInfo.iRDOnlineTime, pReq->playBaseInfo.iRDPlayTime, pReq->playBaseInfo.iRDPrizeNum, pReq->playBaseInfo.iRDGameExpVlue, pReq->playBaseInfo.iMatchTicket,
                    pReq->playBaseInfo.iTakeCount, pReq->playBaseInfo.llGameScore, pReq->playBaseInfo.llMatchScore, pReq->playBaseInfo.iSecondMoney, pReq->playBaseInfo.iRDYuanBao, pReq->playBaseInfo.cMobileType);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnDisconnectAccountReq", "user_id[%u], game_id[%u], clean_flag[%d], llRDMoney[%lld], iRDWinMoney[%lld], iRDAmount[%d], iRDLostMoney[%d], "
                    "iRDPunishMoney[%d], iRDPunishNum[%d], iRDWinNum[%d], iRDLostNum[%d], iRDDrawNum[%d], iRDAllPlayCoin[%d], iRDAllWinCoin[%d], iRDCtrValueA[%d], "
                    "iRDControlNum[%d], iRDLotteryCtrNum[%d], iRDPropNum[%d], iRDTaskNUm[%d], iRDExpValue[%d], iRDOnlineTime[%d], iRDPlayTime[%d], iRDPrizeNum[%d], iRDGameExpVlue[%d], iMatchTicket[%d], "
                    "iTakeCount[%d], llGameScore[%lld], llMatchScore[%lld], iSecondMoney[%d], iRDYuanBao[%d], cMobileType[%d], lResult[%d]",
                    pReq->iUserID, pReq->playBaseInfo.iGameID, pReq->playBaseInfo.cCleanFlag, pReq->playBaseInfo.llRDMoney, pReq->playBaseInfo.iRDWinMoney, pReq->playBaseInfo.iRDAmount,
                    pReq->playBaseInfo.iRDLostMoney, pReq->playBaseInfo.iRDPunishMoney, pReq->playBaseInfo.iRDPunishNum, pReq->playBaseInfo.iRDWinNum, pReq->playBaseInfo.iRDLostNum,
                    pReq->playBaseInfo.iRDDrawNum, pReq->playBaseInfo.iRDAllPlayCoin, pReq->playBaseInfo.iRDAllWinCoin, pReq->playBaseInfo.iRDCtrValueA,
                    pReq->playBaseInfo.iRDControlNum, pReq->playBaseInfo.iRDLotteryCtrNum, pReq->playBaseInfo.iRDPropNum, pReq->playBaseInfo.iRDTaskNUm, pReq->playBaseInfo.iRDExpValue,
                    pReq->playBaseInfo.iRDOnlineTime, pReq->playBaseInfo.iRDPlayTime, pReq->playBaseInfo.iRDPrizeNum, pReq->playBaseInfo.iRDGameExpVlue, pReq->playBaseInfo.iMatchTicket,
                    pReq->playBaseInfo.iTakeCount, pReq->playBaseInfo.llGameScore, pReq->playBaseInfo.llMatchScore, pReq->playBaseInfo.iSecondMoney, pReq->playBaseInfo.iRDYuanBao, pReq->playBaseInfo.cMobileType, lResult);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnDisconnectAccountReq", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(RadiusDisconnect));
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnUserInfoDisconnectReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(RadiusUserInfoDisconnect) == length)
    {
        RadiusUserInfoDisconnect *pReq = (RadiusUserInfoDisconnect*)buffer;

        if (0 == m_db.game_onlinestatus_delete(pReq->iUserID))
        {
            __log(_DEBUG, "GameServerProc::OnUserInfoDisconnectReq", "user_id[%u]", pReq->iUserID);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnUserInfoDisconnectReq", "user_id[%u], db error !", pReq->iUserID);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnUserInfoDisconnectReq", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(RadiusUserInfoDisconnect));
        GameProvider::Close(lpPlayerNode);
    }
}

// 银行操作
void AuthenServer::OnUserLoginBankReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(UserBankLoginReq_trans) == length)
    {
        UserBankLoginReq_trans *pReq = (UserBankLoginReq_trans*)buffer;

        pReq->szBankPass[sizeof(pReq->szBankPass) - 1] = 0;

        LoginBankResult_trans msgRes;

        msgRes.ucResultType = m_db.game_bank_login(pReq->iUserID, pReq->szBankPass, msgRes);

        msgRes.ulPlayerNode = pReq->ulPlayerNode;
        msgRes.ulPlayerNodeId = pReq->ulPlayerNodeId;

        if (true == GameProvider::SendData(lpPlayerNode, RD_USER_LOGIN_RESULT, &msgRes, sizeof(msgRes)))
        {
            __log(_DEBUG, "GameServerProc::OnUserLoginBankReq", "user_id[%u], pwd[%s], result[%d], send success !", pReq->iUserID, pReq->szBankPass, msgRes.ucResultType);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnUserLoginBankReq", "user_id[%u], pwd[%s], result[%d], send failed !", pReq->iUserID, pReq->szBankPass, msgRes.ucResultType);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnUserLoginBankReq", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(UserBankLoginReq_trans));
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnGetBankCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(GetBankCoinReq_trans) == length)
    {
        GetBankCoinReq_trans *pReq = (GetBankCoinReq_trans*)buffer;

        GetCoinResult_trans msgRes;

        msgRes.ucResultType = m_db.game_bank_get_coin(pReq->iUserID, pReq->iGameID, pReq->iServerID, *pReq);

        msgRes.llGetCoinNum = pReq->llGetNum;
        msgRes.ulPlayerNode = pReq->ulPlayerNode;
        msgRes.ulPlayerNodeId = pReq->ulPlayerNodeId;

        if (true == GameProvider::SendData(lpPlayerNode, RD_GET_COIN_RESULT_RES, &msgRes, sizeof(msgRes)))
        {
            __log(_DEBUG, "GameServerProc::OnGetBankCoinReq", "game_id[%u], server_id[%u], user_id[%u], get_coin[%lld], result[%d], send success !",
                    pReq->iGameID, pReq->iServerID, pReq->iUserID, pReq->llGetNum, msgRes.ucResultType);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnGetBankCoinReq", "game_id[%u], server_id[%u], user_id[%u], get_coin[%lld], result[%d], send failed !",
                    pReq->iGameID, pReq->iServerID, pReq->iUserID, pReq->llGetNum, msgRes.ucResultType);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnGetBankCoinReq", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(GetBankCoinReq_trans));
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnUserSaveCoinReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(SaveCoinReq_trans) == length)
    {
        SaveCoinReq_trans *pReq = (SaveCoinReq_trans*)buffer;

        SaveCoinRes_trans msgRes;

        msgRes.ucResultType = m_db.game_bank_save_coin(pReq->iUserID, pReq->iGameID, pReq->iServerID, *pReq);

        msgRes.llSelfCoin = pReq->llSavingNum;
        msgRes.ulPlayerNode = pReq->ulPlayerNode;
        msgRes.ulPlayerNodeId = pReq->ulPlayerNodeId;

        if (true == GameProvider::SendData(lpPlayerNode, RD_USER_SAVE_COIN_RES, &msgRes, sizeof(msgRes)))
        {
            __log(_DEBUG, "GameServerProc::OnUserSaveCoinReq", "game_id[%u], server_id[%u], user_id[%u], modify_coin[%lld], save_coin[%lld], result[%d], send success !",
                    pReq->iGameID, pReq->iServerID, pReq->iUserID, pReq->llModifyCoin, pReq->llSavingNum, msgRes.ucResultType);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnUserSaveCoinReq", "game_id[%u], server_id[%u], user_id[%u], modify_coin[%lld], save_coin[%lld], result[%d], send failed !",
                    pReq->iGameID, pReq->iServerID, pReq->iUserID, pReq->llModifyCoin, pReq->llSavingNum, msgRes.ucResultType);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnUserSaveCoinReq", "buffer length is not match ! length[%u] != sizeof(%u)!", length, sizeof(SaveCoinReq_trans));
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnModifyBankPasswdReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(ModifyBankPasswdReq_trans) == length)
    {
        ModifyBankPasswdReq_trans *pReq = (ModifyBankPasswdReq_trans*)buffer;

        ModifyBankPasswdRes_trans msgRes;

        pReq->szOldPassWd[sizeof(pReq->szOldPassWd) - 1] = 0;
        pReq->szNewPassWd[sizeof(pReq->szNewPassWd) - 1] = 0;

        msgRes.ucResultType = m_db.game_bank_modify_password(pReq->iUserID, pReq->szOldPassWd, pReq->szNewPassWd);

        msgRes.ulPlayerNode = pReq->ulPlayerNode;
        msgRes.ulPlayerNodeId = pReq->ulPlayerNodeId;

        if (true == GameProvider::SendData(lpPlayerNode, RD_MODIFY_BANK_PASSWD_RES, &msgRes, sizeof(msgRes)))
        {
            __log(_DEBUG, "GameServerProc::OnModifyBankPasswdReq", "user_id[%u], old_pwd[%s], new_pwd[%s], result[%d], send success !",
                    pReq->iUserID, pReq->szOldPassWd, pReq->szNewPassWd, msgRes.ucResultType);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnModifyBankPasswdReq", "user_id[%u], old_pwd[%s], new_pwd[%s], result[%d], send failed !",
                    pReq->iUserID, pReq->iUserID, pReq->szOldPassWd, pReq->szNewPassWd, msgRes.ucResultType);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnModifyBankPasswdReq", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(ModifyBankPasswdReq_trans));
        GameProvider::Close(lpPlayerNode);
    }
}

//'玩就送'活动
void AuthenServer::OnDBGetActivityInfo(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(DBGetActivityInfo) == length)
    {
        DBGetActivityInfo *pReq = (DBGetActivityInfo*)buffer;

        DBGetActivityRes msgRes = {0};

        m_db.game_present_play_duration_info_select(*pReq, msgRes);

        msgRes.ulPlayerNode = pReq->ulPlayerNode;
        msgRes.ulPlayerNodeId = pReq->ulPlayerNodeId;

        if (true == GameProvider::SendData(lpPlayerNode, GM_ACTIVITY_RES, &msgRes, sizeof(msgRes)))
        {
            __log(_DEBUG, "GameServerProc::OnDBGetActivityInfo", "user_id[%u], IsActivityOn[%d], iCurPlayTime[%d], iCurGiftID[%d], iTypeID[%d], send success !",
                    pReq->iUserID, msgRes.iISActivityOn, msgRes.iCurPlayTime, msgRes.iCurGiftID, msgRes.iTypeID);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnDBGetActivityInfo", "user_id[%u], IsActivityOn[%d], iCurPlayTime[%d], iCurGiftID[%d], iTypeID[%d], send failed !",
                    pReq->iUserID, msgRes.iISActivityOn, msgRes.iCurPlayTime, msgRes.iCurGiftID, msgRes.iTypeID);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnDBGetActivityInfo", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(DBGetActivityInfo));
        GameProvider::Close(lpPlayerNode);
    }
}

void AuthenServer::OnDBAccountToDB(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(DBAccountToDB) == length)
    {
        DBAccountToDB *pReq = (DBAccountToDB*)buffer;

        if (0 == m_db.game_present_play_duration_info_update(*pReq))
        {
            __log(_DEBUG, "GameServerProc::OnDBAccountToDB", "user_id[%u], user_name[%s], siteid[%d], iCurGameTime[%d], iCurGiftID[%d]", pReq->iUserID, pReq->cUserName, pReq->iSiteID, pReq->iCurGameTime, pReq->iCurGiftID);
        }
        else
        {
            __log(_ERROR, "GameServerProc::OnDBAccountToDB", "user_id[%u], user_name[%s], siteid[%d], iCurGameTime[%d], iCurGiftID[%d], db error !", pReq->iUserID, pReq->cUserName, pReq->iSiteID, pReq->iCurGameTime, pReq->iCurGiftID);
        }
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnDBAccountToDB", "buffer length is not match ! length[%u] != sizeof(%u) !", length, sizeof(DBAccountToDB));
        GameProvider::Close(lpPlayerNode);
    }
}
//红包记录
void AuthenServer::OnUserHongBaoInfo(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
    if (sizeof(UserHongBaoInfo) == length)
    {
        UserHongBaoInfo *pHongbaoInfo = (UserHongBaoInfo*)buffer;

        long lResult = m_db.game_hongbao_info_insert(*pHongbaoInfo);

        __log((0 == lResult) ? _DEBUG : _ERROR, "GameServerProc::OnUserHongBaoInfo", "userid[%d], gameid[%d], server_id[%u], result[%d]!",
                pHongbaoInfo->iUserID, pHongbaoInfo->iGameID, pHongbaoInfo->iServerID, lResult);
    }
    else
    {
        __log(_ERROR, "GameServerProc::OnUserHongBaoInfo", "buffer length is not match ! iMsgLen[%u] != sizeof(%u)", length, sizeof(UserHongBaoInfo));
        GameProvider::Close(lpPlayerNode);
    }
}


/********************************************** 平台活动 ××××××××××××××××××××××××××××××××××××××××××××××××××××*/
void AuthenServer::OnSiteActivityPlayProgressReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(tagDBUserActivityProgressReq) == length)
	{
		tagDBUserActivityProgressReq* pProgressReq = (tagDBUserActivityProgressReq *)buffer;
		DBUserActivityProgressRes LProgress[100] = {0};
		unsigned long ulCount = sizeof(LProgress) / sizeof(LProgress[0]);

		if (0 == m_db.game_activity_progress_select(pProgressReq->iUserID, pProgressReq->iServerID,pProgressReq->iGameID,LProgress, ulCount))
		{
			ulCount = sizeof(LProgress) / sizeof(LProgress[0]) - ulCount;
		}
		else
		{
			ulCount = 0;
		}

		__log(_DEBUG, "GameServerProc::OnSiteActivityPlayProgressReq", "gameid[%d], serverid[%d], ulCount[%d]",pProgressReq->iGameID, pProgressReq->iServerID,ulCount);

		DBActivityProgressList activityProgress = {0};
		activityProgress.ulPlayerNode = pProgressReq->ulPlayerNode;
		activityProgress.ulPlayerNodeId = pProgressReq->ulPlayerNodeId;

		int iIndex = 0;
		while (ulCount > 0)
		{
			activityProgress.uap[activityProgress.iCount] = LProgress[iIndex];
			activityProgress.iCount++;
			iIndex++;
			ulCount--;

			if (sizeof(activityProgress.uap) / sizeof(activityProgress.uap[0]) == activityProgress.iCount)
			{
				__log(_DEBUG, "GameServerProc::OnSiteActivityPlayProgressReq", "第几次发送啊!!");

				if (false == GameProvider::SendData(lpPlayerNode, SITE_ACTIVITY_PLAYER_PROGRESS_RES, &activityProgress, sizeof(activityProgress)))
				{
					__log(_ERROR, "GameServerProc::OnSiteActivityPlayProgressReq", "gameid[%d], serverid[%d], ulCount[%d]",pProgressReq->iGameID, pProgressReq->iServerID,ulCount);
					activityProgress.iCount = 0;
					break;
				}
				activityProgress.iCount = 0;
			}
		}

		if (0 <= activityProgress.iCount)
		{
			__log(_DEBUG, "GameServerProc::OnSiteActivityPlayProgressReq", "第几次发送啊!!");
			if (false == GameProvider::SendData(lpPlayerNode, SITE_ACTIVITY_PLAYER_PROGRESS_RES, &activityProgress, sizeof(activityProgress) - sizeof(activityProgress.uap) +  activityProgress.iCount * sizeof(activityProgress.uap[0])))
			{
				__log(_ERROR, "GameServerProc::OnSiteActivityPlayProgressReq", "gameid[%d], serverid[%d], ulCount[%d]",pProgressReq->iGameID, pProgressReq->iServerID,ulCount);
			}
			activityProgress.iCount = 0;
		}
	}
	else
	{
		__log(_ERROR, "GameServerProc::OnGameServerTaskInfoReq", "buffer length is not match ! length[%u] != sizeof(%u)", length, sizeof(tagDBUserActivityProgressReq));
		GameProvider::Close(lpPlayerNode);
	}
}

void AuthenServer::OnSiteActivityPlayProgressUpdate(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(DBUserActivityProgressUpdate) == length)
	{
		DBUserActivityProgressUpdate *pProgressUpdate = (DBUserActivityProgressUpdate*)buffer;
		long lResult = m_db.game_activity_progress_update(*pProgressUpdate);
		__log((0 == lResult) ? _DEBUG : _ERROR, "GameServerProc::OnSiteActivityPlayProgressUpdate", "userid[%d], gameid[%d], server_id[%d],Activity_id[%d] result[%d]!",
			pProgressUpdate->iUserID, pProgressUpdate->iGameID, pProgressUpdate->iServerID,pProgressUpdate->iActivityID,lResult);
	}
	else
	{
		__log(_ERROR, "GameServerProc::OnSiteActivityPlayProgressUpdate", "buffer length is not match ! iMsgLen[%u] != sizeof(%u)", length, sizeof(DBUserActivityProgressUpdate));
		GameProvider::Close(lpPlayerNode);
	}
}

void AuthenServer::OnSiteActivityPlayLogReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if (sizeof(DBSiteActivityLog) == length)
	{
		DBSiteActivityLog *pActivityInfo = (DBSiteActivityLog*)buffer;
		long lResult = m_db.game_activity_log_insert(*pActivityInfo);

		__log((0 == lResult) ? _DEBUG : _ERROR, "GameServerProc::OnSiteActivityPlayLogReq", "userid[%d], gameid[%d], server_id[%d],Activity_id[%d] result[%d]!",
			pActivityInfo->iUserID, pActivityInfo->iGameID, pActivityInfo->iServerID,pActivityInfo->iActivityID,lResult);
	}
	else
	{
		__log(_ERROR, "GameServerProc::OnSiteActivityPlayLogReq", "buffer length is not match ! iMsgLen[%u] != sizeof(%u)", length, sizeof(DBSiteActivityLog));
		GameProvider::Close(lpPlayerNode);
	}
}

void AuthenServer::OnSiteActivityAwardReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	if(sizeof(DBActivityUserData) == length)
	{
		DBActivityUserData *pActivityData = (DBActivityUserData*)buffer;
		long lResult = m_db.game_activity_userdata_update(*pActivityData);
		__log((0 == lResult) ? _DEBUG : _ERROR, "GameServerProc::OnSiteActivityAwardReq", "iUserID[%d], iActivityTicket[%d], iLotteryTicket[%d]!",
			pActivityData->iUserID, pActivityData->iActivityTicket, pActivityData->iLotteryTicket,lResult);
	}
	else
	{
		__log(_ERROR, "GameServerProc::OnSiteActivityAwardReq", "buffer length is not match ! iMsgLen[%u] != sizeof(%u)", length, sizeof(DBActivityUserData));
		GameProvider::Close(lpPlayerNode);
	}
}

/********************************************** 平台活动 end ××××××××××××××××××××××××××××××××××××××××××××××××××*/

void AuthenServer::HandleOnePraiseAnotherReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	__log(_WARN, "GameLogic::HandleOnePraiseAnotherReq:", "%s %d", __FUNCTION__, __LINE__);

	if (sizeof(OnePraiseAnotherReq_Trans) == length)
	{
		OnePraiseAnotherReq_Trans *pOnePraiseAnother = (OnePraiseAnotherReq_Trans*)buffer;

		unsigned long praisesCount = 0;

		long lResult = m_db.game_one_praise_another_insert(pOnePraiseAnother->fromUserId, pOnePraiseAnother->toUserId, praisesCount);

		OnePraiseAnotherRsp_Trans onPraiseAnotherRsp;
		onPraiseAnotherRsp.fromUserId = pOnePraiseAnother->fromUserId;
		onPraiseAnotherRsp.toUserId = pOnePraiseAnother->toUserId;
		onPraiseAnotherRsp.totalPraiseCount = praisesCount;
		onPraiseAnotherRsp.errorCode = lResult;
		onPraiseAnotherRsp.ulPlayerNode = pOnePraiseAnother->ulPlayerNode;
		onPraiseAnotherRsp.ulPlayerNodeId = pOnePraiseAnother->ulPlayerNodeId;

		__log((0 == lResult) ? _DEBUG : _ERROR, "GameServerProc::HandleOnePraiseAnotherReq", "%s %d fromUserId[%d]toUserId[%d]errorCode[%ld]totalPraiseCount[%ld]",
			__FUNCTION__, __LINE__, onPraiseAnotherRsp.fromUserId, onPraiseAnotherRsp.toUserId, lResult, onPraiseAnotherRsp.totalPraiseCount);

		if (false == GameProvider::SendData(lpPlayerNode, GAMEFCT_MSG_ONE_PRAISE_ANOTHER_RSP, &onPraiseAnotherRsp, sizeof(OnePraiseAnotherRsp_Trans)))
		{
			__log(_ERROR, "GameServerProc::HandleOnePraiseAnotherReq", "%s %d fromUserId[%d] toUserId[%d]", __FUNCTION__, __LINE__, onPraiseAnotherRsp.fromUserId, onPraiseAnotherRsp.toUserId);
		}
	}
	else
	{
		__log(_ERROR, "GameServerProc::HandleOnePraiseAnotherReq", "%s %d buffer length is not match ! iMsgLen[%u] != sizeof(%u)", __FUNCTION__, __LINE__, length, sizeof(OnePraiseAnotherReq));
		GameProvider::Close(lpPlayerNode);
	}
}

void AuthenServer::HandleOneRidiculeAnotherReq(IPlayerNode *lpPlayerNode, void *buffer, long length)
{
	__log(_WARN, "GameLogic::HandleOneRidiculeAnotherReq:", "%s %d", __FUNCTION__, __LINE__);

	if (sizeof(OneRidiculeAnotherReq_Trans) == length)
	{
		OneRidiculeAnotherReq_Trans *pOneRidiculeAnother = (OneRidiculeAnotherReq_Trans*)buffer;

		unsigned long ridiculeCount = 0;

		long lResult = m_db.game_one_ridicule_another_insert(pOneRidiculeAnother->fromUserId, pOneRidiculeAnother->toUserId, ridiculeCount);

		OneRidiculeAnotherRsp_Trans onRidiculeAnotherRsp;
		onRidiculeAnotherRsp.errorCode = lResult;
		onRidiculeAnotherRsp.fromUserId = pOneRidiculeAnother->fromUserId;
		onRidiculeAnotherRsp.toUserId = pOneRidiculeAnother->toUserId;
		onRidiculeAnotherRsp.totalRidiculeCount = ridiculeCount;
		onRidiculeAnotherRsp.ulPlayerNode = pOneRidiculeAnother->ulPlayerNode;
		onRidiculeAnotherRsp.ulPlayerNodeId = pOneRidiculeAnother->ulPlayerNodeId;

		__log((0 == lResult) ? _DEBUG : _ERROR, "GameServerProc::HandleOneRidiculeAnotherReq", "%s %d fromUserId[%d]toUserId[%d]errorCode[%ld]totalRidiculeCount[%d]",
			__FUNCTION__, __LINE__, pOneRidiculeAnother->fromUserId, pOneRidiculeAnother->toUserId, lResult, onRidiculeAnotherRsp.totalRidiculeCount);

		if (false == GameProvider::SendData(lpPlayerNode, GAMEFCT_MSG_ONE_RIDICULE_ANOTHER_RSP, &onRidiculeAnotherRsp, sizeof(OneRidiculeAnotherRsp_Trans)))
		{
			__log(_ERROR, "GameServerProc::HandleOneRidiculeAnotherReq", "%s %d fromUserId[%d] toUserId[%d]", __FUNCTION__, __LINE__, pOneRidiculeAnother->fromUserId, pOneRidiculeAnother->toUserId);
		}
	}
	else
	{
		__log(_ERROR, "GameServerProc::HandleOneRidiculeAnotherReq", "%s %d buffer length is not match ! iMsgLen[%u] != sizeof(%u)", __FUNCTION__, __LINE__, length, sizeof(OneRidiculeAnotherReq_Trans));
		GameProvider::Close(lpPlayerNode);
	}
}