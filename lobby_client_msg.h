#ifndef __LOBBY_CLIENT_MSG_H__
#define __LOBBY_CLIENT_MSG_H__

#include "Game/NetMessage.h"

//************************************************************
//************       大厅客户端 <-> 登录服务器         **********
//************************************************************
enum
{
    LOBBY_AUTHEN_REQ = 0x8000,
    LOBBY_AUTHEN_RES,
    LOBBY_AUTHEN_RES_2,
	LOBBY_HECHENG_REQ,				//合成参赛卷
	LOBBY_HECHEGN_RES,
	LOBBY_HECHENG_RES_2,
	LOBBY_GET_GAME_PRIZE_REQ,   //获取未输入信息领取的实物奖
	LOBBY_GET_GAME_PRIZE_RES,
	LOBBY_GET_HONGBAO_INFO_REQ, //获取红包信息
	LOBBY_GET_HONGBAO_INFO_RES,
	LOBBY_GET_HONGBAO_CONTENT_REQ, //领取红包内容
	LOBBY_GET_HONGBAO_CONTENT_RES,
    GAME_SERVERINFO_NOTICE, 

	LOBBY_AUTHEN_WITH_NO_SITE_ID_REQ = 0x8017,
	LOBBY_AUTHEN_WITH_NO_SITE_ID_RES, 
	LOBBY_SEND_SELF_LOCATION,
	
	//passpod验证
	LOBBY_PASSPOD_VERIFY_ERROR_RSP,
	LOBBY_SET_USER_NICKNAME_REQ,
	LOBBY_SET_USER_NICKNAME_RSP,

	//修改登录密码
	LOBBY_MODIFY_LOGIN_PASSWORD_REQ,
	LOBBY_MODIFY_LOGIN_PASSWORD_RES,

	//获取彩票信息
	LOBBY_GET_CAIPIAO_INFO_REQ,		
	LOBBY_GET_CAIPIAO_INFO_RES,
};

//大厅客户端验证请求;  大厅客户端 -> 登录服务器
typedef struct tagLobbyAuthenReq
{
	HEADER;
	int				iGameID;
	int				iSiteID;

	char			szUserName[NAME_LEN];
	char			szPassword[PASSWD_LEN];

	unsigned char	ucPasspodVerifyStatus;

	char			szChannelID[33];
	char			szVersionStr[33];

	char			szMachineSerial[33];
	char			szClientIP[16];

}LobbyAuthenReq,  *PLobbyAuthenReq;

//不知道站点ID的情况下，请求登录
typedef struct tagLobbyAuthenWithNoSiteIdReq
{
	HEADER;
	int  iGameID;
	char szUserName[49];    
	char szPassword[PASSWD_LEN];

	unsigned char	ucPasspodVerifyStatus;		// 1=需要查询DB中的标记; 2=需要向U盾服务器验证
	char			szPasspod[PASSWD_LEN];

	char			szMachineSerial[33];

}LobbyAuthenWithNoSiteIdReq, *PLobbyAuthenWithNoSiteIdReq;

//修改玩家昵称请求
typedef struct tagLobbySetNicknameReq
{
	HEADER;

	int				iUserID;
	char			szNickname[49];

}LobbySetNicknameReq;

//修改玩家昵称回应
typedef struct tagLobbySetNicknameRsp
{
	HEADER;

	int				iUserID;
	int				iErrorCode;
	char			szNickname[NAME_LEN];
	char			szErrorDescibe[128];

}LobbySetNicknameRsp;

//客户端登陆发送自己的设备号以及
typedef struct tagLobbySendSelfLocationReq
{
	HEADER;
	int  iUserId;		//玩家ID
	int  iAppId;		//游戏ID
	char iGuId[64];			//设备号
	char iMachineSerial[64];//机器码
	char iIp[32];		//ip
	int  iGpsX;			//经度
	int  iGpsY;			//维度
	char szAppChannelInfo[64];//应用渠道信息
}LobbySendSelfLocationReq, *PLobbySendSelfLocationReq;

typedef struct LobbyHeCheng
{
	HEADER;
    int             iUserID;                        //用户ID
    char            szPassword[PASSWD_LEN];         //用户密码
	int             cChoose;
}LobbyHeChengReq,  *PLobbyHeChengReq;



typedef struct tagLobbyHeChengRes
{
	HEADER;
	long long       nScore;                         //用户银子
	unsigned long   ulCoin2Award;                   //银券通
	unsigned long   ulAwardScore;                   //奖品券
}LobbyHeChengRes,  *PLobbyHeChengRes;

//登录服务器返回给大厅客户端的玩家数据; 登录服务器 -> 大厅客户端
typedef struct tagLobbyAuthenRes
{
    HEADER;
    unsigned long   ulUserID;                       //用户ID
    long long       nScore;                         //用户银子
    unsigned long   ulCoin2Award;                   //银券通
    unsigned long   ulCompeteScore;                 //参赛券
    unsigned long   ulAwardScore;                   //奖品券
	unsigned char   ucGender;                       //用户性别

	unsigned char   ucIsSetNickname;				//是否设置了昵称
	char			szNickname[NAME_LEN];			//玩家昵称
	//0918 孤獨的巧克力添加 是否首次充值标志
	int				uFirstCharge;					// 1 代表为首充用户
	//1031  周年庆VIP标识
	int				uVipForAnnivesary;				// 0 不是VIP用户 98 黄金会员  99白金会员
	int				nSendTimes;		
	int				nHeadImage;						//设置头像索引
	long long		nBankScore;						//银行积分
}LobbyAuthenRes, *PLobbyAuthenRes;

//请求的时候，不知道站点ID
typedef struct tagLobbyAuthenWithNoSiteIdRes
{
	HEADER;
	unsigned long   ulUserID;                       //用户ID
	long long       nScore;                         //用户银子
	unsigned long   ulCoin2Award;                   //银券通
	unsigned long   ulCompeteScore;                 //参赛券
	unsigned long   ulAwardScore;                   //奖品券
	unsigned char   ucGender;                       //用户性别
	int				siteId;

	unsigned char   ucIsSetNickname;				//是否设置了昵称
	char			szNickname[NAME_LEN];			//玩家昵称
	//0918 孤獨的巧克力添加 是否首次充值标志
	int				uFirstCharge;					// 1 代表为首充用户
	//1031  周年庆VIP标识
	int				uVipForAnnivesary;				// 0 不是VIP用户 98 黄金会员  99白金会员
	int				nSendTimes;			
	int				nHeadImage;						//设置头像索引
	long long		nBankScore;						//银行积分
}LobbyAuthenWithNoSiteIdRes, *PLobbyAuthenWithNoSiteIdRes;

//passpod验证失败回应
typedef struct tagLobbyPasspodVerifyErrorRsp
{
	HEADER;

	int				iUserID;						//用户ID
	int				iErrorCode;						//错误码
	char			szErrorDetail[128];				//错误描述

}LobbyPasspodVerifyErrorRsp, *PLobbyPasspodVerifyErrorRsp;

typedef enum LOBBY_AUTHEN_ERROR
{
	ERROR_AUTHEN_SERVER_DISCONNECT					= -2,		//服务器连接失败。
	ERROR_AUTHEN_EXECUTE_PROCEDURE_FAILED			,			//存储失败
	ERROR_AUTHEN_DBRETURN_USER_PASS_ERROR			= 1,		//账号密码错误
	ERROR_AUTHEN_DBRETURN_OPERATE_ERROR				= 2,		//insert if not exist error 数据库操作失败 
	ERROR_AUTHEN_DBRETURN_MARKET_ACCOUNT			= 3,		//新手帐号
	ERROR_AUTHEN_DBRETURN_ACCOUNT_DISABLE			= 4,		//帐号禁用
	ERROR_AUTHEN_DBRETURN_NOT_MATCH_SITE_ID			= 5,		//账号站点ID不匹配
	ERROR_AUTHEN_DBRETURN_CONFIG_VERSION_ERROR		= 6,		//配置文件版本错误
	ERROR_AUTHEN_DBRETURN_NEED_PASSPOD_VERIFY		= 7			//需要U盾认证

}LOBBY_AUTHEN_ERROR;

typedef enum LOBBY_HECHENG_ERROR
{
	ERROR_LOBBY_HECHENG_EXECUTE_PROCEDURE_FAILED = -1, 	//存储过程执行失败
	ERROR_LOBBY_USER_PASSWORD_ERROR = 1,
	ERROR_LOBBY_HECHENG_SELECT_ERROR,			//数据库select没有数据
	ERROR_LOBBY_HECHENG_UPDATE_COIN_ERROR,			//更新银子失败
	ERROR_LOBBY_HECHENG_UPDATE_COIN2TICKET_ERROR,		//更新银券通失败
	ERROR_LOBBY_HECHENG_UPDATE_PRIZE_ERROR,			//更新奖品券失败
	ERROR_LOBBY_HECHENG_EXIST_GAME,				//游戏中不能进行合成操作
	ERROR_LOBBY_HECHENG_NOENOUGH_COIN,			//银子不够
	ERROR_LOBBY_HECHENG_NOENOUGH_COIN2AWARD			//银券通不够
}LOBBY_HECHENG_ERROR_DEF;



//登录服务器返回给大厅客户端的验证错误码; 登录服务器 -> 大厅客户端
typedef struct tagLobbyHeChengRes_2
{
	HEADER;
	LOBBY_HECHENG_ERROR lError; //错误码
}LobbyHeCheng_2, *PLobbyHeCheng_2;

//登录服务器返回给大厅客户端的验证错误码; 登录服务器 -> 大厅客户端
typedef struct tagLobbyAuthenRes_2
{
    HEADER;
    LOBBY_AUTHEN_ERROR lError; //错误码
}LobbyAuthenRes_2, *PLobbyAuthenRes_2;

//登录服务器发送房间信息给大厅客户端; 登录服务器 -> 大厅客户端
//服务器列表信息结构体(要跟管理服务器的ServerInfo保持一致)
typedef struct tagGameServerInfo
{
    unsigned short  usGameID;           //游戏ID
    unsigned short  usServerID;         //房间ID
    char            szServerName[32];   //房间名称
    unsigned long   ulLimitPoint;       //房间限制
    unsigned long   ulBaseTime;         //房间底注
    unsigned short  usOnlineCount;      //在线人数
    unsigned short  usServerPort;       //房间PORT
    unsigned long   ulServerIP;         //房间IP
    unsigned char   ucServerType;       //房间类型(比赛、积分、游戏币）
    unsigned char   ucOpenFlag;         //此房间是否开启, 0: 关闭;
}GameServerInfo, *PGameServerInfo;

typedef struct tagGameServerInfoNotice
{
    HEADER;
    unsigned short  usListCount;        //服务器列表数量
    GameServerInfo  ServerInfoList[30]; //服务器列表信息
}GameServerInfoNotice, *PGameServerInfoNotice;

//获取未输入信息领取的实物奖
typedef struct tagLobbyGetGamePrizeReq
{
    HEADER;
    int             iUserID;                        //用户ID
    int             iGameID;                        //游戏ID
    char            szPassword[PASSWD_LEN];         //用户密码
}LobbyGetGamePrizeReq, *PLobbyGetGamePrizeReq;

typedef struct tagLobbyExchangePrize
{
    long        lKeyID;
    char        szPrizeName[32];
    long        lPrizeID;
    long        lCompeteRank;
    long        lWinDate;
    long        lExpDate;
}LobbyExchangePrize, *PLobbyExchangePrize;

typedef struct tagLobbyGetGamePrizeRes
{
    HEADER;
    int         iCount;             //奖品个数
    LobbyExchangePrize prize[10];   //最多10个
}LobbyGetGamePrizeRes, *PLobbyGetGamePrizeRes;

//LOBBY_GET_HONGBAO_INFO_REQ
typedef struct tagLobbyGetHongbaoInfoReq
{
    HEADER;
    int             iUserID;                        //用户ID
    int             iGameID;                        //游戏ID
    char            szPassword[PASSWD_LEN];         //用户密码
}LobbyGetHongbaoInfoReq, *PLobbyGetHongbaoInfoReq;

typedef struct tagLobbyHongbaoInfo
{
    int         iID;                //红包ID
    int         iType;              //红包类型
    int         iCoinNum;           //银子
    int         iPrizeNum;          //奖品券
    int         iMatchTickNum;      //参赛券
    int         iCoinAward;         //银券通
    int         iIngotNum;          //元宝
}LobbyHongbaoInfo, *PLobbyHongbaoInfo;

//LOBBY_GET_HONGBAO_INFO_RES
typedef struct tagLobbyGetHongbaoInfoRes
{
    HEADER;
    int                 iCount;      //红包个数
    LobbyHongbaoInfo    info[100];   //红包信息, 1次最多发送100个
}LobbyGetHongbaoInfoRes, *PLobbyGetHongbaoInfoRes;

//LOBBY_GET_HONGBAO_CONTENT_REQ
typedef struct tagLobbyGetHongbaoContentReq
{
    HEADER;
    int             iUserID;                        //用户ID
    int             iGameID;                        //游戏ID
    char            szPassword[PASSWD_LEN];         //用户密码
    int             iID;                            //红包ID
}LobbyGetHongbaoContentReq, *PLobbyGetHongbaoContentReq;

//LOBBY_GET_HONGBAO_CONTENT_RES
typedef struct tagLobbyGetHongbaoContentRes
{
    HEADER;
    int                 iErrorCode; //错误码, 0: 成功
}LobbyGetHongbaoContentRes, *PLobbyGetHongbaoContentRes;

typedef struct tagLobbyModifyLoginPasswordReq
{
	HEADER;
	int						nUserID;
	char					nOldPassMd5[33];
	char					nNewPassMd5[33];
}LobbyModifyLoginPasswordReqDef;

typedef struct tagLobbyModifyLoginPasswordRes
{
	HEADER;
	long					iErrorCode; //错误码
}LobbyModifyLoginPasswordResDef;

typedef struct tagLobbyGetCaiPiaoInfoReq
{
	HEADER;

	int						nUserID;
	int						nSiteID;
}LobbyGetCaiPiaoInfoReqDef;

typedef struct tagLobbyGetCaiPiaoInfoRes
{
	HEADER;

	int						nUserID;
	int						nCaiPiaoCount;		//彩票数
	int						nCaiJinCount;		//彩金数
	int						nReserve1;			//保留字段
	int						nReserve2;
	int						nReserve3;
	int						nReserve4;
	int						nReserve5;
}LobbyGetCaiPiaoInfoResDef;

#endif //__LOBBY_CLIENT_MSG_H__
