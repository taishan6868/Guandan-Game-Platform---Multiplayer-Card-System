#include "DBAccounts.h"
#include "Common/Common.h"
#include "Game/NetMessage.h"
#include "aes/aeslib.h"
#include <errno.h>

extern CIniFile g_cfg;
const long RETRY_COUNT = 1;

#define EXEC_NONE_FLAG          0x00000000
#define EXEC_RETURN_VALUE_FLAG  0x00000001
#define EXEC_SELECT_MORE_FLAG   0x00000002

unsigned char hex2byte(unsigned char hex)
{
    const char *const lpszTable = "0123456789ABCDEF";

    return (0x0F >= hex) ? lpszTable[hex] : -1;
}

unsigned char str2byte(unsigned char szchar)
{
    unsigned char cResult = 0xFF;

    if ((0x30 <= szchar) && (0x39 >= szchar))
    {
        cResult = szchar - 0x30;
    }
    else if ((0x41 <= szchar) && (0x5A >= szchar))
    {
        cResult = szchar - 0x41 + 0x0A;
    }
    else if ((0x61 <= szchar) && (0x7A >= szchar))
    {
        cResult = szchar - 0x61 + 0x0A;
    }

    return cResult;
}

long str2byte(char *lpszStr)
{
    long lResult = -1;

    if (NULL != lpszStr)
    {
        unsigned long len = strlen(lpszStr);

        if (0 == (len % 2))
        {
            lResult = len / 2;

            for (int i = 0; i < lResult; i++)
            {
                lpszStr[i] = str2byte(lpszStr[i * 2]);
                lpszStr[i] = (lpszStr[i] << 4) | str2byte(lpszStr[i * 2 + 1]);
            }

            lpszStr[lResult] = 0;
        }
    }

    return lResult;
}

bool decrypt(char *lpszEncode, char *lpszBuffer, int len)
{
    bool bResult = false;

    if ((NULL != lpszEncode) && (NULL != lpszBuffer) && (len > 0))
    {
        long lStrLen = str2byte(lpszEncode);
        if (-1 != lStrLen)
        {
            if (0 == aes_dec_r(lpszEncode, lStrLen, "", 0, lpszBuffer, &len))
            {
                lpszBuffer[len] = 0;
                bResult = true;
            }
        }
    }

    return bResult;
}

bool ConvertTime(long tmTime, TIMESTAMP_STRUCT& stamp)
{
	tm *ptm = localtime(&tmTime);

	if (NULL != ptm)
	{
		stamp.year   = ptm->tm_year + 1900;
		stamp.month  = ptm->tm_mon + 1;
		stamp.day    = ptm->tm_mday;
		stamp.hour   = ptm->tm_hour;
		stamp.minute = ptm->tm_min;
		stamp.second = ptm->tm_sec;
	}
	return (NULL != ptm);
}

long ConvertTimeToLong(TIMESTAMP_STRUCT& stamp)
{
	long lTime = -1;
	tm target_time = {0};
	target_time.tm_year = stamp.year - 1900;
	target_time.tm_mon  = stamp.month - 1;     // 月 - 1
	target_time.tm_mday = stamp.day;           // 日
	target_time.tm_hour = stamp.hour  ;        // 时
	target_time.tm_min  = stamp.minute ;      // 分
	target_time.tm_sec  = stamp.second ;      // 秒

	lTime = mktime(&target_time);
	return lTime;	
}

DBAccounts::DBAccounts()
{
    m_bOpen = false;
}

DBAccounts::~DBAccounts()
{

}

bool DBAccounts::OpenDSN(const char *lpName, ODBCExt& db, ODBCContext& ctx)
{
    bool bResult = false;

    char szDSN[32], szUser[32], szPwd[128], szDecode[1024];

    if (0 != g_cfg.GetValueStr(lpName, "dsn", szDSN, sizeof(szDSN)))
    {
        if (0 != g_cfg.GetValueStr(lpName, "usr", szUser, sizeof(szUser)))
        {
            if (0 != g_cfg.GetValueStr(lpName, "pwd", szPwd, sizeof(szPwd)))
            {
                int len = sizeof(szDecode);
                if (true == decrypt(szPwd, szDecode, len))
                {
                    if (SQL_SUCCEEDED(db.Open(szDSN, szUser, szDecode)))
                    {
                        if (true == ctx.Initialize(&db))
                        {
                            long l = ctx.SQLExec("set ansi_warnings on");

                            if (SQL_SUCCEEDED(l) || (SQL_NO_DATA == l))
                            {
                                __log(_DEBUG, "DBAccounts::OpenDSN", "name[%s], dsn[%s], db.Open() success !", lpName, szDSN);
                                int iQueryTimeout = g_cfg.GetValueInt(lpName, "query_timeout", 0);
                                ctx.SetStmtAttr(SQL_ATTR_QUERY_TIMEOUT, (void*)iQueryTimeout, 0);

                                bResult = true;
                            }
                            else
                            {
                                if (false == m_bOpen) //重连不打印log
                                {
                                    __log(_ERROR, "DBAccounts::OpenDSN", "name[%s], 'set ansi_warnings on' error, %s !", lpName, ctx.GetErrorInfo());
                                }
                            }
                        }
                        else
                        {
                            if (false == m_bOpen) //重连不打印log
                            {
                                __log(_ERROR, "DBAccounts::OpenDSN", "name[%s], ctx.Initialize() error, %s !", lpName, ctx.GetErrorInfo());
                            }
                        }
                    }
                    else
                    {
                        __log(_ERROR, "DBAccounts::OpenDSN", "name[%s], ctx.Open() error, %s !", lpName, db.GetErrorInfo());
                    }
                }
                else
                {
                    if (false == m_bOpen) //重连不打印log
                    {
                        __log(_ERROR, "DBAccounts::OpenDSN", "decrypt() error !");
                    }
                }
            }
            else
            {
                __log(_ERROR, "DBAccounts::OpenDSN", "name[%s], g_cfg.GetValueStr() 'pwd' error !", lpName);
            }
        }
        else
        {
            __log(_ERROR, "DBAccounts::OpenDSN", "name[%s], g_cfg.GetValueStr() 'usr' error !", lpName);
        }
    }
    else
    {
        __log(_ERROR, "DBAccounts::OpenDSN", "name[%s], g_cfg.GetValueStr() 'dsn' error !", lpName);
    }

    return bResult;
}

bool DBAccounts::Open()
{
    m_ctxQPGameData.Close();
    m_dbQPGameData.Close();

    if (true == OpenDSN("qp_gamedata_db", m_dbQPGameData, m_ctxQPGameData))
    {
        m_bOpen = true;
    }

    return m_bOpen;
}

bool DBAccounts::ExecSP(ODBCContext& ctx, unsigned long ulExecFlag, const char *lpszSP, long argc, ...)
{
    bool bResult = false;

    if (NULL != lpszSP)
    {
        long lRetryCount = 0;

        char szParamters[512] = {0};
        char szCallSP[1024];
        long lArgCount = (ulExecFlag == EXEC_RETURN_VALUE_FLAG) ? (argc - 1) : argc;

        if (lArgCount > 0)
        {
            strcat(szParamters, "(?");

            for (int i = 1; i < lArgCount; i++)
            {
                strcat(szParamters, ",?");
            }

            strcat(szParamters, ")");
        }

        if (EXEC_RETURN_VALUE_FLAG == ulExecFlag)
        {
            sprintf(szCallSP, "{? = call %s%s}", lpszSP, szParamters);
        }
        else
        {
            sprintf(szCallSP, "{call %s%s}", lpszSP, szParamters);
        }

        while (lRetryCount <= RETRY_COUNT)
        {
            if (SQL_SUCCEEDED(ctx.SQLPrepare((LPTSTR)szCallSP)))
            {
                va_list va;
                va_start(va, argc);

                SQLLEN cbLen = SQL_NTS;
                long i = 1;

                for (; i <= argc; i++)
                {
                    void *arg_addr = (void*)va_arg(va, int);
                    long arg_len = va_arg(va, int);
                    long *arg_cblen = (long*)va_arg(va, int);
                    long column_len = va_arg(va, int);
                    long decimal_len = va_arg(va, int);
                    SQLSMALLINT arg_sqltype = va_arg(va, int);
                    SQLSMALLINT arg_ctype = va_arg(va, int);
                    long arg_paramtype = va_arg(va, int);

                    if (!SQL_SUCCEEDED(ctx.BindParam(i, arg_addr, arg_len, arg_cblen, column_len, decimal_len, arg_sqltype, arg_ctype, arg_paramtype)))
                    {
                        return false;
                    }
                }

                long l = ctx.SQLExec();

                if (SQL_SUCCEEDED(l))
                {
                    if (EXEC_SELECT_MORE_FLAG == ulExecFlag)
                    {
                        bResult = true;
                        break;
                    }

                    while (false == bResult)
                    {
                        l = ctx.SQLMoreResults();

                        if (SQL_NO_DATA == l)
                        {
                            bResult = true;
                        }
                        else if (!SQL_SUCCEEDED(l))
                        {
                            break;
                        }
                    }
                }
                else if (SQL_NO_DATA == l)
                {
                    bResult = true;
                }
            }

            if ((false == bResult) && (true == m_dbQPGameData.IsDisconnect()))
            {
                if (&ctx == &m_ctxQPGameData)
                {
                    __log(_ERROR, "AuthenDB::ExecSP", "reconnect QPGameDataDB, ulExecFlag[%d], errno[%d], info: %s !", ulExecFlag, errno, strerror(errno));

                    OpenDSN("qp_gamedata_db", m_dbQPGameData, m_ctxQPGameData);
                }

                lRetryCount++;
            }
            else
            {
                break;
            }
        }
    }

    return bResult;
}

long DBAccounts::login_authen_accountinfo_select(LobbyAuthenReq& req, LobbyAuthenRes& info)
{
    long lResult = 1;

    __log(_DEBUG, "DBAccounts::login_authen_accountinfo_select", "usr[%s], pwd[%s], iGameID[%d], iSiteID[%d], channelID[%s], ver[%s]!", 
		req.szUserName, req.szPassword, req.iGameID, req.iSiteID, req.szChannelID, req.szVersionStr);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "login_authen_mobile_accountinfo_select_1001_test", 23,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(req.szUserName, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
						MAKEPARAM_IN(req.szPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
						MAKEPARAM_IN(&req.ucPasspodVerifyStatus, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
                        MAKEPARAM_IN(&req.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_IN(&req.iSiteID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_IN(req.szClientIP, 15, 0, 15, 0, SQL_CHAR, SQL_C_CHAR),
						MAKEPARAM_IN(req.szMachineSerial, 32, 0, 32, 0, SQL_CHAR, SQL_C_CHAR),
						MAKEPARAM_IN(req.szChannelID, 32, 0, 32, 0, SQL_VARCHAR, SQL_C_CHAR),
						MAKEPARAM_IN(req.szVersionStr, 32, 0, 32, 0, SQL_VARCHAR, SQL_C_CHAR),
                        MAKEPARAM_OUT(&info.ulUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.ucGender, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
                        MAKEPARAM_OUT(&info.nScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT),
                        MAKEPARAM_OUT(&info.ulCoin2Award, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.ulAwardScore, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_OUT(&info.ulCompeteScore, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_OUT(&info.ucIsSetNickname, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
						MAKEPARAM_OUT(&info.szNickname, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
						MAKEPARAM_OUT(&info.uFirstCharge, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_OUT(&info.uVipForAnnivesary, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_OUT(&info.nSendTimes, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_OUT(&info.nHeadImage, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_OUT(&info.nBankScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT)
                        ))
    {
        __log(_ERROR, "DBAccounts::login_authen_accountinfo_select", "ExecSP() error ! usr[%s], pwd[%s], iGameID[%d], iSiteID[%d], info: %s", req.szUserName, req.szPassword, req.iGameID, req.iSiteID, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::login_authen_with_no_siteid_accountinfo_select(LobbyAuthenWithNoSiteIdReq_Trans& req, LobbyAuthenWithNoSiteIdRes& info)
{
	long lResult = 1;

	__log(_DEBUG, "DBAccounts::login_authen_with_no_siteid_accountinfo_select", "usr[%s], pwd[%s], iGameID[%d], passpodVStatus[%d]!", req.szUserName, req.szPassword, req.iGameID, req.ucPasspodVerifyStatus);

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "login_authen_ios_accountinfo_select_1001_test", 21,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(req.szUserName, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_IN(req.szPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_IN(&req.ucPasspodVerifyStatus, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
		MAKEPARAM_IN(&req.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(req.szClientIP, 15, 0, 15, 0, SQL_CHAR, SQL_C_CHAR),
		MAKEPARAM_IN(req.szMachineSerial, 32, 0, 32, 0, SQL_CHAR, SQL_C_CHAR),
		MAKEPARAM_OUT(&info.siteId, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.ulUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.ucGender, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
		MAKEPARAM_OUT(&info.nScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT),
		MAKEPARAM_OUT(&info.ulCoin2Award, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.ulAwardScore, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.ulCompeteScore, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.ucIsSetNickname, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
		MAKEPARAM_OUT(&info.szNickname, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_OUT(&info.uFirstCharge, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.uVipForAnnivesary, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nSendTimes, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nHeadImage, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nBankScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT)
		))
	{
		__log(_ERROR, "DBAccounts::login_authen_with_no_siteid_accountinfo_select", "ExecSP() error ! usr[%s], pwd[%s], iGameID[%d], info: %s", req.szUserName, req.szPassword, req.iGameID, m_ctxQPGameData.GetErrorInfo());
	}

	return lResult;
}

long DBAccounts::SPSetUserNickname(LobbySetNicknameReq& req, LobbySetNicknameRsp& info)
{
	long lResult = 1;

	__log(_DEBUG, "DBAccounts::SPSetUserNickname", "UserID[%d], Nickname[%s]!", req.iUserID, req.szNickname);

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "sp_mobile_set_user_nickname", 4,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&req.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(req.szNickname, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_OUT(info.szErrorDescibe, sizeof(info.szErrorDescibe), 0, sizeof(info.szErrorDescibe), 0, SQL_VARCHAR, SQL_C_CHAR)
		))
	{
		__log(_ERROR, "DBAccounts::SPSetUserNickname", "ExecSP() error ! UserID[%d], Nickname[%s], ErrorDescribe[%s], info: %s", 
			req.iUserID, req.szNickname, info.szErrorDescibe, m_ctxQPGameData.GetErrorInfo());
	}
	else
	{
		__log(_DEBUG, "DBAccounts::SPSetUserNickname", "ExecSP() success ! UserID[%d], Nickname[%s], ErrorDescribe[%s]", 
			req.iUserID, req.szNickname, info.szErrorDescibe);
	}

	return lResult;
}

long DBAccounts::login_send_self_location(LobbySendSelfLocationReq& req)
{
	long lResult = 0;

	__log(_DEBUG, "DBAccounts::login_send_self_location", "iUserId[%d], iAppId[%d], iGuId[%s], iMachineSerial[%s]!", req.iUserId, req.iAppId, req.iGuId, req.iMachineSerial);

	if (false == ExecSP(m_ctxQPGameData, EXEC_NONE_FLAG, "QPGameUserDB.dbo.SP_InsertUserLoginLog", 8,
		MAKEPARAM_IN(&req.iUserId, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&req.iAppId, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(req.iGuId, 64, 0, 64, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_IN(req.iMachineSerial, 64, 0, 64, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_IN(req.iIp, 32, 0, 32, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_IN(&req.iGpsX, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&req.iGpsY, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(req.szAppChannelInfo, 64, 0, 64, 0, SQL_VARCHAR, SQL_C_CHAR)
		))
	{
		__log(_ERROR, "DBAccounts::login_send_self_location", "ExecSP() error ! iUserId[%d], iAppId[%d], iGuId[%s], iMachineSerial[%s], info: %s", req.iUserId, req.iAppId, req.iGuId, req.iMachineSerial, m_ctxQPGameData.GetErrorInfo());

		lResult = -1;
	}

	return lResult;
}

/*
long DBAccounts::mobile_authen_accountinfo_select(const char *lpszUserName, const char *lpszPassword, MobileAuthenRes& info)
{
    long lResult = 1;

    if (NULL == lpszUserName) lpszUserName = "";
    if (NULL == lpszPassword) lpszPassword = "";

    __log(_DEBUG, "DBAccounts::mobile_authen_accountinfo_select", "usr[%s], pwd[%s] !", lpszUserName, lpszPassword);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "mobile_authen_accountinfo_select", 9,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(lpszUserName, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
                        MAKEPARAM_IN(lpszPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
                        MAKEPARAM_OUT(&info.ulUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.ucGender, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
                        MAKEPARAM_OUT(&info.nScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT),
                        MAKEPARAM_OUT(&info.ulCoin2Award, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.ulAwardScore, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.ulCompeteScore, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)
                        ))
    {
        __log(_ERROR, "DBAccounts::mobile_authen_accountinfo_select", "ExecSP() error ! usr[%s], pwd[%s], info: %s", lpszUserName, lpszPassword, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}
*/

long DBAccounts::login_combin_prize(LobbyHeChengReq& req, LobbyHeChengRes& res)
{
    long lResult = -1;

    __log(_DEBUG, "DBAccounts::login_combin_prize", "userid[%d], prizenum[%d]", req.iUserID, res.ulAwardScore);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "login_combin_prize", 6,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&req.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(req.szPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
                        MAKEPARAM_INOUT(&res.ulAwardScore, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&res.nScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT),
                        MAKEPARAM_OUT(&res.ulCoin2Award, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
    {
        __log(_ERROR, "DBAccounts::login_combin_prize", "userid[%d], prizenum[%d], info: %s", req.iUserID, res.ulAwardScore, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::login_get_game_prize(LobbyGetGamePrizeReq& req, LobbyGetGamePrizeRes& res)
{
    long lResult = -1;

    __log(_DEBUG, "DBAccounts::login_get_game_prize", "userid[%d], iGameID[%d]", req.iUserID, req.iGameID);

    if (false == ExecSP(m_ctxQPGameData, EXEC_SELECT_MORE_FLAG, "login_get_game_prize", 3,
                        MAKEPARAM_IN(&req.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&req.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&req.szPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR)
                        ))
    {
        __log(_ERROR, "DBAccounts::login_get_game_prize", "userid[%d], iGameID[%d], info: %s", req.iUserID, req.iGameID, m_ctxQPGameData.GetErrorInfo());
    }
    else
    {
        LobbyExchangePrize item;
        TIMESTAMP_STRUCT tsWinDate, tsExpDate;
        if (SQL_SUCCEEDED(m_ctxQPGameData.BindColumns(6, MAKECOLUMN(&item.lKeyID, 0, 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&item.szPrizeName, sizeof(item.szPrizeName), 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&item.lPrizeID, 0, 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&item.lCompeteRank, 0, 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&tsWinDate, 0, 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&tsExpDate, 0, 0, SQL_C_DEFAULT)
                                                         )))
        {
            res.iCount = 0;
            int iItemCount = sizeof(res.prize) / sizeof(res.prize[0]);
            long l;
            while ((0 < iItemCount) && SQL_SUCCEEDED(l = m_ctxQPGameData.FetchNext()))
            {
                tm tm = {0};
                tm.tm_year = tsWinDate.year - 1900;
                tm.tm_mon = tsWinDate.month - 1;
                tm.tm_mday = tsWinDate.day;
                tm.tm_hour = tsWinDate.hour;
                tm.tm_min = tsWinDate.minute;
                tm.tm_sec = tsWinDate.second;
                item.lWinDate = mktime(&tm);
                tm.tm_year = tsExpDate.year - 1900;
                tm.tm_mon = tsExpDate.month - 1;
                tm.tm_mday = tsExpDate.day;
                tm.tm_hour = tsExpDate.hour;
                tm.tm_min = tsExpDate.minute;
                tm.tm_sec = tsExpDate.second;
                item.lExpDate = mktime(&tm);
                res.prize[res.iCount++] = item;
                iItemCount--;
            }

            if ((SQL_NO_DATA == l) || (0 == iItemCount))
            {
                lResult = 0;
            }
            else
            {
                __log(_ERROR, "DBAccounts::login_get_game_prize", "FetchNext() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
            }
        }
        else
        {
            __log(_ERROR, "DBAccounts::login_get_game_prize", "BindColumns() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
        }
    }

    return lResult;
}

long DBAccounts::login_get_hongbao_info(LobbyGetHongbaoInfoReq& req, LobbyHongbaoInfo* lpInfo, unsigned long& ulInfoCount)
{
    long l, lResult = -1;

    __log(_DEBUG, "DBAccounts::login_get_hongbao_info", "usr_id[%d], game_id[%d] !", req.iUserID, req.iGameID);

    if ((NULL != lpInfo) && (ulInfoCount > 0))
    {
        if (false == ExecSP(m_ctxQPGameData, EXEC_SELECT_MORE_FLAG, "login_get_hongbao_info", 3,
                            MAKEPARAM_IN(&req.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                            MAKEPARAM_IN(&req.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                            MAKEPARAM_IN(req.szPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR)))
        {
            __log(_ERROR, "DBAccounts::login_get_hongbao_info", "usr_id[%d], game_id[%d], info: %s !", req.iUserID, req.iGameID, m_ctxQPGameData.GetErrorInfo());
        }
        else
        {
            LobbyHongbaoInfo item;
            if (SQL_SUCCEEDED(m_ctxQPGameData.BindColumns(07, MAKECOLUMN(&item.iID, 0, 0, SQL_C_DEFAULT),
                                                              MAKECOLUMN(&item.iType, 0, 0, SQL_C_DEFAULT),
                                                              MAKECOLUMN(&item.iCoinNum, 0, 0, SQL_C_DEFAULT),
                                                              MAKECOLUMN(&item.iPrizeNum, 0, 0, SQL_C_DEFAULT),
                                                              MAKECOLUMN(&item.iMatchTickNum, 0, 0, SQL_C_DEFAULT),
                                                              MAKECOLUMN(&item.iCoinAward, 0, 0, SQL_C_DEFAULT),
                                                              MAKECOLUMN(&item.iIngotNum, 0, 0, SQL_C_DEFAULT))))
            {
                unsigned long ulCount = 0;
                while ((ulCount < ulInfoCount) && SQL_SUCCEEDED(l = m_ctxQPGameData.FetchNext()))
                {
                    lpInfo[ulCount++] = item;
                }

                if ((SQL_NO_DATA == l) || (ulCount == ulInfoCount))
                {
                    ulInfoCount = ulCount;
                    lResult = 0;
                }
                else
                {
                    __log(_ERROR, "DBRoomInfo::login_get_hongbao_info", "FetchNext() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
                }
            }
            else
            {
                __log(_ERROR, "DBRoomInfo::login_get_hongbao_info", "BindColumns() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
            }
        }
    }

    return lResult;
}

long DBAccounts::login_get_hongbao_content(LobbyGetHongbaoContentReq& req)
{
    long lResult = -1;

    __log(_DEBUG, "DBAccounts::login_get_hongbao_content", "usr_id[%d], game_id[%d] !", req.iUserID, req.iGameID);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "login_get_hongbao_content", 5,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&req.iID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&req.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&req.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&req.szPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR)))
    {
        __log(_ERROR, "DBAccounts::login_get_hongbao_content", "usr_id[%d], game_id[%d], info: %s !", req.iUserID, req.iGameID, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::login_get_caipiao_info(LobbyGetCaiPiaoInfoReqDef &req, LobbyGetCaiPiaoInfoResDef &info)
{
	long lResult = -1;

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "sp_get_user_caipiao_count", 10,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&req.nUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&req.nSiteID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),

		MAKEPARAM_OUT(&info.nCaiPiaoCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nCaiJinCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nReserve1, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nReserve2, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nReserve3, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nReserve4, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&info.nReserve5, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)
		))
	{
		__log(_ERROR, "DBAccounts::login_get_caipiao_info", "UserID[%d], SiteID[%d], info: %s !", req.nUserID, req.nSiteID, m_ctxQPGameData.GetErrorInfo());
	}

	return lResult;
}

long DBAccounts::game_authen_accountinfo_select(int iUserID, const char *lpszPassword, DBAuthenRes& info, DBAuthenRes_2& info2)
{
    long lResult = -1;

    if (NULL == lpszPassword) lpszPassword = "";

    __log(_DEBUG, "DBAccounts::game_authen_accountinfo_select_new", "usr_id[%d], pwd[%s] !", iUserID, lpszPassword);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_authen_accountinfo_select_mobile_1001_test", 69,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(lpszPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
                        MAKEPARAM_IN(info.szIP, sizeof(info.szIP), 0, sizeof(info.szIP), 0, SQL_VARCHAR, SQL_C_CHAR), //ClientIP
                        MAKEPARAM_IN(&info.szComputerID, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),  //机器码

						MAKEPARAM_OUT(&info.szUserName, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR), //用户名
						MAKEPARAM_OUT(&info.szNickName, NAME_LEN, 0, NAME_LEN, 0, SQL_VARCHAR, SQL_C_CHAR), //用户昵称
                        MAKEPARAM_OUT(&info.cSexType, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT), //性别
                        MAKEPARAM_OUT(&info.cVipType, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT), //会员等级
                        MAKEPARAM_OUT(&info.cMasterType, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT), //管理等级
                        MAKEPARAM_OUT(&info.ucRegisterPresent, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT), //注册赠送标志
                        MAKEPARAM_OUT(&info.usIconNum, 0, 0, 0, 0, SQL_SMALLINT, SQL_C_USHORT), //头像索引
                        MAKEPARAM_OUT(&info.iUserMasterRight, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //管理权限
                        MAKEPARAM_OUT(&info.iAreaID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家地区ID(站点ID）
                        MAKEPARAM_OUT(&info.iTypeID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家注册账号的区间
                        MAKEPARAM_OUT(&info.iUserLevel, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //用户等级

                        MAKEPARAM_OUT(&info.iExpValue, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //用户经验
                        MAKEPARAM_OUT(&info.iPrizeNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //奖品券数量
                        MAKEPARAM_OUT(&info.iSecondMoney, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //银券通数量
                        MAKEPARAM_OUT(&info.iYBNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //元宝数量
                        MAKEPARAM_OUT(&info.lOnLineTime, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家在线时间

                        MAKEPARAM_OUT(&info.ucSpecialIdentify, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT), //特殊身份
                        MAKEPARAM_OUT(&info.ucSpecialAvatar, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT), //特殊头像
                        MAKEPARAM_OUT(&info.cPhotoVerify, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT), //自定义头像是否审核通过
                        MAKEPARAM_OUT(&info.iPhotoKey, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //自定义头像ID
                        MAKEPARAM_INOUT(&info2.usPlayGameID, 0, 0, 0, 0, SQL_SMALLINT, SQL_C_USHORT), //玩家所在游戏ID

                        MAKEPARAM_INOUT(&info2.usPlayServerID, 0, 0, 0, 0, SQL_SMALLINT, SQL_C_USHORT), //玩家所在服务器ID

                        MAKEPARAM_OUT(&info.iFirstMoney, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //一级货币(银子)
                        MAKEPARAM_OUT(&info.iBankMoney, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //玩家银行储蓄

                        MAKEPARAM_OUT(&info.iWinNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //胜利次数
                        MAKEPARAM_OUT(&info.iLostNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //失败次数
                        MAKEPARAM_OUT(&info.iDrawNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //和局盘数
                        MAKEPARAM_OUT(&info.iOffLineNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家断线次数
                        MAKEPARAM_OUT(&info.iGameExpValue, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //游戏中的经验值
                        MAKEPARAM_OUT(&info.iMatchTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),  //游戏中参赛权
                        MAKEPARAM_OUT(&info.iTakeCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),    //游戏的参赛次数
                        MAKEPARAM_OUT(&info.iGameScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //一级货币(银子)
                        MAKEPARAM_OUT(&info.iMatchScore, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //一级货币(银子)

                        MAKEPARAM_OUT(&info.llAllPlayCoin, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //玩家总玩积分
                        MAKEPARAM_OUT(&info.llAllWinCoin, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //玩家总赢积分
                        MAKEPARAM_OUT(&info.llAllCtrCoinA, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //普通棋牌类控制总净分
                        MAKEPARAM_OUT(&info.llAllCtrCoinB, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT), //博彩类游戏控制总净分
                        MAKEPARAM_OUT(&info.iControlNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),    //玩家充值贡献度
                        MAKEPARAM_OUT(&info.iLotteryCtrNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家奖品券贡献度
                        MAKEPARAM_OUT(&info.iTaskLotteryNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //任务获得奖券
                        MAKEPARAM_OUT(&info.iPropNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家身上道具数目

						MAKEPARAM_OUT(&info.iIsBindMobile, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //是否绑定手机
           
                        //公共道具信息
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA0, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA1, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA2, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA3, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA4, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA5, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA6, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA7, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA8, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagBasePropInfo.iBufferA9, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),

                        //单个游戏道具信息
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffA0, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffA1, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffA2, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffA3, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffA4, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),

                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffB0, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffB1, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffB2, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffB3, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_OUT(&info.tagGamePropInfo.iBuffB4, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
						MAKEPARAM_OUT(&info.iPraiseCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //被赞的次数
						MAKEPARAM_OUT(&info.iRidiculeCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER) //被吐槽的次数
                        ))
    {
        __log(_ERROR, "DBAccounts::game_authen_accountinfo_select", "ExecSP() error ! usr_id[%d], pwd[%s], info: %s", iUserID, lpszPassword, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::game_cost_accountinfo_update(RadiusDisconnect& account)
{
    long lResult = -1;

    __log(_DEBUG, "DBAccounts::game_cost_accountinfo_update_new", "usr_id[%d], clean_flag[%d] !", account.iUserID, account.playBaseInfo.cCleanFlag);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_cost_accountinfo_update_new", 48,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.cCleanFlag, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),
                        MAKEPARAM_IN(&account.playBaseInfo.llRDMoney, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDExpValue, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDOnlineTime, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDPlayTime, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDPrizeNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDGameExpVlue, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDWinNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDLostNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDDrawNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iMatchTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),    
                        MAKEPARAM_IN(&account.playBaseInfo.iTakeCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.llGameScore, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
                        MAKEPARAM_IN(&account.playBaseInfo.llMatchScore, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
                        MAKEPARAM_IN(&account.playBaseInfo.iSecondMoney, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.iRDYuanBao, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.cMobileType, 0, 0, 0, 0, SQL_TINYINT, SQL_C_TINYINT),

                        MAKEPARAM_IN(&account.playBaseInfo.iRDAllPlayCoin, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家总玩积分
                        MAKEPARAM_IN(&account.playBaseInfo.iRDAllWinCoin, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家总赢积分
                        MAKEPARAM_IN(&account.playBaseInfo.iRDCtrValueA, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //普通棋牌类控制总净分
                        MAKEPARAM_IN(&account.playBaseInfo.iRDCtrValueB, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //博彩类游戏控制总净分
                        MAKEPARAM_IN(&account.playBaseInfo.iRDControlNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),    //玩家充值贡献度
                        MAKEPARAM_IN(&account.playBaseInfo.iRDLotteryCtrNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家奖品券贡献度
                        MAKEPARAM_IN(&account.playBaseInfo.iRDTaskNUm, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //任务获得奖券
                        MAKEPARAM_IN(&account.playBaseInfo.iRDPropNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER), //玩家身上道具数目

                        //公用道具 字段存储
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA0, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA1, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA2, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA3, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA4, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA5, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA6, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA7, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA8, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagBasePropInfo.iBufferA9, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),

                        //单个游戏中的道具信息
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffA0, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffA1, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffA2, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffA3, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffA4, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffB0, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffB1, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffB2, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffB3, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&account.playBaseInfo.tagGamePropInfo.iBuffB4, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)
                        ))
    {
        __log(_ERROR, "DBAccounts::game_cost_accountinfo_update", "ExecSP() error ! usr_id[%d], clean_flag[%d], info: %s", account.iUserID, account.playBaseInfo.cCleanFlag, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::game_onlinestatus_delete(int iUserID)
{
    long lResult = 0;

    __log(_DEBUG, "DBAccounts::game_onlinestatus_delete", "usr_id[%d] !", iUserID);

    if (false == ExecSP(m_ctxQPGameData, EXEC_NONE_FLAG, "game_onlinestatus_delete", 1,
                        MAKEPARAM_IN(&iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
    {
        __log(_ERROR, "DBAccounts::game_onlinestatus_delete", "ExecSP() error ! usr_id[%d], info: %s", iUserID, m_ctxQPGameData.GetErrorInfo());

        lResult = -1;
    }

    return lResult;
}

//‘玩就送’活动
long DBAccounts::game_present_play_duration_info_select(DBGetActivityInfo& info, DBGetActivityRes& res)
{
    __log(_DEBUG, "DBAccounts::game_present_play_duration_info_select", "usr_id[%d] !", info.iUserID);

    if (false == ExecSP(m_ctxQPGameData, EXEC_SELECT_MORE_FLAG, "QPGameUserDB.dbo.sp_load_present_play_duration_info", 1,
                        MAKEPARAM_IN(&info.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
    {
        __log(_ERROR, "DBAccounts::game_present_play_duration_info_select", "ExecSP() error ! usr_id[%d], info: %s", info.iUserID, m_ctxQPGameData.GetErrorInfo());
    }
    else
    {
        if (SQL_SUCCEEDED(m_ctxQPGameData.BindColumns(4, MAKECOLUMN(&res.iISActivityOn, 0, 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&res.iCurPlayTime, 0, 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&res.iCurGiftID, 0, 0, SQL_C_DEFAULT),
                                                         MAKECOLUMN(&res.iTypeID, 0, 0, SQL_C_DEFAULT))))
        {
            if (SQL_SUCCEEDED(m_ctxQPGameData.FetchNext()))
            {
                m_ctxQPGameData.SQLMoreResults();
            }
            else
            {
                __log(_ERROR, "DBRoomInfo::game_experience_config_select", "FetchNext() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
            }
        }
        else
        {
            __log(_ERROR, "DBRoomInfo::game_experience_config_select", "BindColumns() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
        }
    }

    return 0;
}

long DBAccounts::game_present_play_duration_info_update(DBAccountToDB& info)
{
    long lResult = -1;

    __log(_DEBUG, "DBAccounts::game_present_play_duration_info_update", "user_id[%u], user_name[%s], siteid[%d], iCurGameTime[%d], iCurGiftID[%d]", info.iUserID, info.cUserName, info.iSiteID, info.iCurGameTime, info.iCurGiftID);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "WebSiteActivityDB.dbo.SP_2012ZNQ_UpdatePlayTime", 6,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&info.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&info.cUserName, sizeof(info.cUserName), 0, sizeof(info.cUserName), 0, SQL_VARCHAR, SQL_C_CHAR),
                        MAKEPARAM_IN(&info.iSiteID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&info.iCurGameTime, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&info.iCurGiftID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
    {
        __log(_ERROR, "DBAccounts::game_present_play_duration_info_update", "ExecSP() error ! usr_id[%d], info: %s", info.iUserID, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}
long DBAccounts::game_hongbao_info_insert(UserHongBaoInfo& hongbaoInfo)
{
    long lResult = -1;
    __log(_DEBUG, "DBAccounts::game_hongbao_info_insert", "userid[%d], gameid[%d], serverid[%d] !", hongbaoInfo.iUserID, hongbaoInfo.iGameID, hongbaoInfo.iServerID);

    if(false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_hongbao_info_insert", 11,
            MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.szUserName, sizeof(hongbaoInfo.szUserName), 0, sizeof(hongbaoInfo.szUserName), 0, SQL_VARCHAR, SQL_C_CHAR),
            MAKEPARAM_IN(&hongbaoInfo.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iHongBaoType, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iCoinNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iPrizeNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iMatchTicketNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iCoinAward, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
            MAKEPARAM_IN(&hongbaoInfo.iInGotNum, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)
           ))
    {
        __log(_ERROR, "DBAccounts::game_hongbao_info_insert", "userid[%d], gameid[%d], serverid[%d], info: %s !",
                hongbaoInfo.iUserID, hongbaoInfo.iGameID, hongbaoInfo.iServerID, m_ctxQPGameData.GetErrorInfo());
    }
    return lResult;
}

//银行操作
long DBAccounts::game_bank_login(int iUserID, const char *lpszPassword, LoginBankResult& result)
{
    long lResult = -1;

    if (NULL == lpszPassword) lpszPassword = "";

    __log(_DEBUG, "DBAccounts::game_bank_login", "usr_id[%d], pwd[%s] !", iUserID, lpszPassword);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_bank_login", 5,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(lpszPassword, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
                        MAKEPARAM_OUT(&result.llGameCoin, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT),
                        MAKEPARAM_OUT(&result.llBankCoin, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT)))
    {
        __log(_ERROR, "DBAccounts::game_bank_login", "ExecSP() error ! usr_id[%d], pwd[%s] info: %s", iUserID, lpszPassword, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::game_bank_get_coin(int iUserID, int iGameID, int iServerID, GetBankCoinReq_trans& getcoin)
{
    long lResult = -1;

    __log(_DEBUG, "DBAccounts::game_bank_get_coin", "usr_id[%d], get_coin[%lld], game_id[%d], server_id[%d] !", iUserID, getcoin.llGetNum, iGameID, iServerID);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_bank_get_coin", 5,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&getcoin.llGetNum, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
                        MAKEPARAM_IN(&iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
    {
        __log(_ERROR, "DBAccounts::game_bank_get_coin", "ExecSP() error ! usr_id[%d], get_coin[%lld], game_id[%d], server_id[%d], info: %s", iUserID, getcoin.llGetNum, iGameID, iServerID, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::game_bank_save_coin(int iUserID, int iGameID, int iServerID, SaveCoinReq_trans& savecoin)
{
    long lResult = -1;

    __log(_DEBUG, "DBAccounts::game_bank_save_coin", "usr_id[%d], modify_coin[%lld], save_coin[%lld], game_id[%d], server_id[%d] !", iUserID, savecoin.llModifyCoin, savecoin.llSavingNum, iGameID, iServerID);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_bank_save_coin", 6,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&savecoin.llModifyCoin, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
                        MAKEPARAM_IN(&savecoin.llSavingNum, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
                        MAKEPARAM_IN(&iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
    {
        __log(_ERROR, "DBAccounts::game_bank_save_coin", "ExecSP() error ! usr_id[%d], modify_coin[%lld], save_coin[%lld], game_id[%d], server_id[%d], info: %s", iUserID, savecoin.llModifyCoin, savecoin.llSavingNum, iGameID, iServerID, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::game_bank_modify_password(int iUserID, const char *lpszOldPwd, const char *lpszNewPwd)
{
    long lResult = -1;

    if (NULL == lpszOldPwd) lpszOldPwd = "";
    if (NULL == lpszNewPwd) lpszNewPwd = "";

    __log(_DEBUG, "DBAccounts::game_bank_modify_password", "usr_id[%d], old_pwd[%s], new_pwd[%s] !", iUserID, lpszOldPwd, lpszNewPwd);

    if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_bank_modify_password", 4,
                        MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(&iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
                        MAKEPARAM_IN(lpszOldPwd, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_CHAR, SQL_C_CHAR),
                        MAKEPARAM_IN(lpszNewPwd, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_CHAR, SQL_C_CHAR)))
    {
        __log(_ERROR, "DBAccounts::game_bank_modify_password", "ExecSP() error ! usr_id[%d], old_pwd[%s], new_pwd[%s], info: %s", iUserID, lpszOldPwd, lpszNewPwd, m_ctxQPGameData.GetErrorInfo());
    }

    return lResult;
}

long DBAccounts::mobile_bank_authen(MobileLoginServerBankAuthenReqPtr pBankAuthen, MobileLoginServerBankAuthenSuccessRsp &result)
{
	long lResult = -1;

	__log(_DEBUG, "DBAccounts::mobile_bank_authen", "usr_id[%d], pwd[%s] !", pBankAuthen->iUserID, pBankAuthen->szBankPasswd);

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "mobile_bank_authen", 5,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pBankAuthen->iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(pBankAuthen->szBankPasswd, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_OUT(&result.llGameCoin, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT),
		MAKEPARAM_OUT(&result.llBankCoin, 0, 0, 0, 0, SQL_DOUBLE, SQL_C_SBIGINT)))
	{
		__log(_ERROR, "DBAccounts::mobile_bank_authen", "ExecSP() error ! usr_id[%d], pwd[%s] info: %s", pBankAuthen->iUserID, pBankAuthen->szBankPasswd, m_ctxQPGameData.GetErrorInfo());
	}

	return lResult;
}

long DBAccounts::mobile_bank_get_coin(MobileLoginServerBankGetCoinReqPtr pBankGetCoin, MobileLoginServerBankGetCoinSuccessRsp &result)
{
	long lResult = -1;

	__log(_DEBUG, "DBAccounts::mobile_bank_get_coin", 
		"usr_id[%d], get_coin[%lld], game_id[%d], server_id[%d] !", 
		pBankGetCoin->authen.iUserID, pBankGetCoin->llGetNum, pBankGetCoin->iGameID, pBankGetCoin->iServerID);

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "mobile_bank_get_coin", 6,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pBankGetCoin->authen.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(pBankGetCoin->authen.szBankPasswd, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_IN(&pBankGetCoin->llGetNum, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
		MAKEPARAM_IN(&pBankGetCoin->iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pBankGetCoin->iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
	{
		__log(_ERROR, "DBAccounts::mobile_bank_get_coin", 
			"ExecSP() error ! usr_id[%d], get_coin[%lld], game_id[%d], server_id[%d], info: %s", 
			pBankGetCoin->authen.iUserID, pBankGetCoin->llGetNum, pBankGetCoin->iGameID, pBankGetCoin->iServerID, m_ctxQPGameData.GetErrorInfo());
	}

	return lResult;
}

long DBAccounts::mobile_bank_save_coin(MobileLoginServerBankSaveCoinReqPtr pBankSaveCoin, MobileLoginServerBankSaveCoinSuccessRsp &result)
{
	long lResult = -1;

	__log(_DEBUG, "DBAccounts::mobile_bank_save_coin", 
		"usr_id[%d], save_coin[%lld], game_id[%d], server_id[%d] !", 
		pBankSaveCoin->authen.iUserID, pBankSaveCoin->llSaveNum, pBankSaveCoin->iGameID, pBankSaveCoin->iServerID);

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "mobile_bank_save_coin", 6,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pBankSaveCoin->authen.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(pBankSaveCoin->authen.szBankPasswd, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_VARCHAR, SQL_C_CHAR),
		MAKEPARAM_IN(&pBankSaveCoin->llSaveNum, 0, 0, 0, 0, SQL_BIGINT, SQL_C_SBIGINT),
		MAKEPARAM_IN(&pBankSaveCoin->iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pBankSaveCoin->iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)))
	{
		__log(_ERROR, "DBAccounts::mobile_bank_save_coin", 
			"ExecSP() error ! usr_id[%d], save_coin[%lld], game_id[%d], server_id[%d], info: %s", 
			pBankSaveCoin->authen.iUserID, pBankSaveCoin->llSaveNum, pBankSaveCoin->iGameID, pBankSaveCoin->iServerID, m_ctxQPGameData.GetErrorInfo());
	}

	return lResult;
}

long DBAccounts::mobile_bank_modify_password(MobileLoginServerBankModifyPasswordReqPtr pBankModifyPwd)
{
	long lResult = -1;

	__log(_DEBUG, "DBAccounts::mobile_bank_modify_password", 
		"usr_id[%d], old_pwd[%s], new_pwd[%s] !", 
		pBankModifyPwd->authen.iUserID, pBankModifyPwd->authen.szBankPasswd, pBankModifyPwd->szNewPasswd);

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "mobile_bank_modify_password", 4,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pBankModifyPwd->authen.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(pBankModifyPwd->authen.szBankPasswd, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_CHAR, SQL_C_CHAR),
		MAKEPARAM_IN(pBankModifyPwd->szNewPasswd, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_CHAR, SQL_C_CHAR)))
	{
		__log(_ERROR, "DBAccounts::mobile_bank_modify_password", 
			"ExecSP() error ! usr_id[%d], old_pwd[%s], new_pwd[%s], info: %s", 
			pBankModifyPwd->authen.iUserID, pBankModifyPwd->authen.szBankPasswd, pBankModifyPwd->szNewPasswd, m_ctxQPGameData.GetErrorInfo());
	}

	return lResult;
}

long DBAccounts::lobby_modify_login_password(LobbyModifyLoginPasswordReq_Trans* modifyPassword)
{
	long lResult = -1;

	__log(_DEBUG, "DBAccounts::lobby_modify_login_password", "usr_id[%d], old_pwd[%s], new_pwd[%s] !", 
		modifyPassword->nUserID, modifyPassword->nOldPassMd5, modifyPassword->nNewPassMd5);

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "sp_modify_logon_password", 4,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&modifyPassword->nUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(modifyPassword->nOldPassMd5, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_CHAR, SQL_C_CHAR),
		MAKEPARAM_IN(modifyPassword->nNewPassMd5, PASSWD_LEN, 0, PASSWD_LEN, 0, SQL_CHAR, SQL_C_CHAR)))
	{
		__log(_ERROR, "DBAccounts::lobby_modify_login_password", "ExecSP() error ! usr_id[%d], old_pwd[%s], new_pwd[%s], info: %s", 
			modifyPassword->nUserID, modifyPassword->nOldPassMd5, modifyPassword->nNewPassMd5, m_ctxQPGameData.GetErrorInfo());
	}

	return lResult;
}

///////////////////////////////////////////////// 平台活动相关 ////////////////////////////////////////////////

long DBAccounts::game_activity_progress_select(int iUserID,int iServerID,int iGameID,DBUserActivityProgressRes *pItem, unsigned long &ulItemCount)
{
	long l, lResult = -1;

	TIMESTAMP_STRUCT   time1  = {0};

	if (NULL != pItem && ulItemCount > 0)
	{
		__log(_DEBUG, "DBRoomInfo::game_activity_progress_select", "get activity_progress config !");

		if (false == ExecSP(m_ctxQPGameData, EXEC_SELECT_MORE_FLAG, "WebSiteActivityDB.dbo.site_activity_progress_select", 3,
			MAKEPARAM_IN(&iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
			MAKEPARAM_IN(&iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
			MAKEPARAM_IN(&iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT)
			))    
		{
			__log(_ERROR, "DBRoomInfo::game_activity_progress_select", "ExecSP() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
		}
		else
		{
			int iLen = 0;
			DBUserActivityProgressRes Item = {0};
			if(SQL_SUCCEEDED(m_ctxQPGameData.BindColumns(14,
				MAKECOLUMN(&Item.iUserID, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iActivityID, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iSiteID, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iGameID, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iServerID, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iTypeID, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iGameCoin, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iPrizeTicket, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iCoinTicket, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iActivityTicket, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iLotteryTicket, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iProgress1, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&Item.iProgress2, 0, &iLen, SQL_C_DEFAULT),
				MAKECOLUMN(&time1, 0, &iLen,SQL_C_DEFAULT)
				)))
			{

				while ((0 < ulItemCount) && SQL_SUCCEEDED(l = m_ctxQPGameData.FetchNext()))
				{
					Item.iUpdateTime   =  ConvertTimeToLong(time1);
					memset(&time1,0,sizeof(time1));	            
					printf("两个时间【%d】\n",Item.iUpdateTime);

					*pItem++ = Item;
					ulItemCount--;
				}

				if ((SQL_NO_DATA == l) || (0 == ulItemCount))
				{
					m_ctxQPGameData.SQLMoreResults();
					lResult = 0;
				}
				else
				{
					__log(_ERROR, "DBRoomInfo::game_activity_progress_select", "FetchNext() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
				}
			}
			else
			{
				__log(_ERROR, "DBRoomInfo::game_activity_progress_select", "BindColumns() error ! info: %s", m_ctxQPGameData.GetErrorInfo());
			}
		}
	}
	return lResult;
}

long DBAccounts::game_activity_log_insert(DBSiteActivityLog &SiteActivityLog)
{
	long lResult = -1;
	__log(_DEBUG, "DBAccounts::game_activity_log_insert", "usr_id[%d], ActivityID[%d], GameID[%d],SiteID[%d]!",
		SiteActivityLog.iUserID,SiteActivityLog.iActivityID, SiteActivityLog.iGameID,SiteActivityLog.iSiteID);          
	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "WebSiteActivityDB.dbo.site_activity_log_insert", 14,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iActivityID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iSiteID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iTypeID, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iGameCoin, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iPrizeTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iCoinTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iActivityTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&SiteActivityLog.iLotteryTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(SiteActivityLog.szIPAddress,sizeof(SiteActivityLog.szIPAddress),0 , sizeof(SiteActivityLog.szIPAddress), 0,SQL_VARCHAR, SQL_C_CHAR), 
		MAKEPARAM_IN(&SiteActivityLog.iStatus, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)
		))
	{
		__log(_ERROR, "DBAccounts::sp_WriteUserFreePresentLog", "ExecSP() error ! iUserID[%d], iSiteID[%d], iScore[%d", SiteActivityLog.iUserID, SiteActivityLog.iSiteID, SiteActivityLog.iGameCoin);
	}

	return lResult;
}

long DBAccounts::game_activity_progress_update(DBUserActivityProgressUpdate &pUpdate)
{
	long lResult = -1;

	TIMESTAMP_STRUCT UpdateTime = {0};

	if (false == ConvertTime(pUpdate.iUpdateTime, UpdateTime))
	{
		__log(_ERROR, "DBAccounts::game_activity_progress_update", "ConvertTime failed! UserID[%d]", pUpdate.iUserID);
		return lResult;
	}

	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "WebSiteActivityDB.dbo.site_activity_progress_update", 15,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pUpdate.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iActivityID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iSiteID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iGameID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),

		MAKEPARAM_IN(&pUpdate.iServerID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iTypeID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iGameCoin, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iPrizeTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iCoinTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iActivityTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iLotteryTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iProgress1, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUpdate.iProgress2, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&UpdateTime, 0, 0, 0, 0, SQL_TYPE_TIMESTAMP, SQL_C_TYPE_TIMESTAMP)
		))
	{
		__log(_ERROR, "DBAccounts::game_activity_progress_update", "用户ID = [%d],---ExecSP() error ! info: %s",pUpdate.iUserID,m_ctxQPGameData.GetErrorInfo());
	}
	return lResult; 
}

long DBAccounts::game_activity_userdata_update(DBActivityUserData &pUserData)
{
	long lResult = -1;
	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "WebSiteActivityDB.dbo.site_activity_userdata_update", 4,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&pUserData.iUserID, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUserData.iActivityTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT),
		MAKEPARAM_IN(&pUserData.iLotteryTicket, 0, 0, 0, 0, SQL_INTEGER, SQL_C_DEFAULT)
		))
	{
		__log(_ERROR, "DBAccounts::game_activity_userdata_update", "用户ID = [%d],---ExecSP() error ! info: %s",pUserData.iUserID,m_ctxQPGameData.GetErrorInfo());
	}
	return lResult; 
}
/////////////////////////////////////////// 平台活动 end //////////////////////////////////////////////

long DBAccounts::game_one_praise_another_insert(int fromUserId, int toUserId, unsigned long& praiseCount)
{
	long lResult = -1;
	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_praise_info_insert", 3,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&toUserId, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&praiseCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)
		))
	{
		__log(_ERROR, "DBAccounts::game_one_praise_another_insert", "%s %d UserID[%d] praise UserID[%d] ERROR!", __FUNCTION__, __LINE__, fromUserId, toUserId);
	}
	return lResult; 
}

long DBAccounts::game_one_ridicule_another_insert(int fromUserId, int toUserId, unsigned long& ridiculeCount)
{
	long lResult = -1;
	if (false == ExecSP(m_ctxQPGameData, EXEC_RETURN_VALUE_FLAG, "game_ridicule_info_insert", 3,
		MAKEPARAM_OUT(&lResult, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_IN(&toUserId, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER),
		MAKEPARAM_OUT(&ridiculeCount, 0, 0, 0, 0, SQL_INTEGER, SQL_INTEGER)
		))
	{
		__log(_ERROR, "DBAccounts::game_one_ridicule_another_insert", "%s %d UserID[%d] praise UserID[%d] ERROR!", __FUNCTION__, __LINE__, fromUserId, toUserId);
	}
	return lResult; 
}
