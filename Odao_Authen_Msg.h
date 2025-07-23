 /*
 * Odao_Authen_Msg.h
 *
 *  Created on: 2012-6-20
 *      Author: root
 */

#ifndef _ODAO_AUTHEN_MSG_H_
#define _ODAO_AUTHEN_MSG_H_

#include "Game/NetMessage.h"

#define USER_RADIUS_AUTHEN_REQ                   0XD1
#define USER_RADIUS_AUTHEN_RES                   0XD2
#define USER_RADIUS_AUTHEN_RES2                  0XD3
#define GM_DISCONNECT_ACCOUNT_REQ                0XD4     //计费请求
#define GM_USERINFO_DISCONNECT_REQ               0XD5
#define GM_ACTIVITY_INFO                         0xC6
#define GM_ACTIVITY_RES                          0xC7
#define GM_DBACCOUNT_TODB                        0xC8

#define GAME_ACTIVITY_HONGBAO_INFO               0xCA

//---------------------------------活动认证 add at 2014/09/12------------------------------------------
#define SITE_ACTIVITY_PLAYER_PROGRESS_REQ       0XD012       //玩家活动进度请求
#define SITE_ACTIVITY_PLAYER_PROGRESS_RES       0XD013       //玩家活动进度回应
#define SITE_ACTIVITY_PLAYER_PROGRESS_UPDATE    0XD014       //玩家活动进度更新
#define SITE_ACTIVITY_PLAYER_LOG_REQ            0XD015       //玩家活动奖励日志
#define SITE_ACTIVITY_PLAYER_AWARD_DB           0XD016       //玩家获奖数据记录更新
//--------------------------------------------------------------------------------------------------------------

//**************************************点赞拉黑 20141104***************************************************

const unsigned short  GAMEFCT_MSG_ONE_PRAISE_ANOTHER_REQ =	0XD017;	//一个玩家赞另外一个玩家
const unsigned short  GAMEFCT_MSG_ONE_PRAISE_ANOTHER_RSP =  0XD018; //一个玩家赞另外一个玩家返回消息
const unsigned short  GAMEFCT_MSG_ONE_RIDICULE_ANOTHER_REQ =	0XD019;	//一个玩家吐槽另外一个玩家
const unsigned short  GAMEFCT_MSG_ONE_RIDICULE_ANOTHER_RSP =  0XD020; //一个玩家吐槽另外一个玩家返回消息

//******************************************************************************************************

typedef struct DBAuthenReq                      //AUTHEN_REQ_MSG  玩家房间登陆验证请求
{
    MsgHeader      msgHeadInfo;
    int            iUserID;                     //用户ID
    char           szPasswd[PASSWD_LEN];        //用户密码
    char           szUserToken[20];             //用户Token

    unsigned short usGameID;                    //当前选在GameID
    unsigned short usServerID;                  //当前选择ServerID

    unsigned long  ulPlayerNode;
    unsigned long  ulPlayerNodeId;
}DBAuthenReqDef;


typedef struct DBGetActivityInfo                //GM_ACTIVITY_INFO 取得比赛信息
{
		MsgHeader      msgHeadInfo;
		int            iUserID;
		
		unsigned long  ulPlayerNode;
    unsigned long  ulPlayerNodeId;
}DBGetActivityInfoDef;


typedef struct DBGetActivityRes
{
	  MsgHeader      msgHeadInfo;
	  int            iISActivityOn;
	  int            iCurPlayTime;
	  int            iCurGiftID;
	  int            iTypeID;
	  
		unsigned long ulPlayerNode;
    unsigned long ulPlayerNodeId;
}DBGetActivityResDef;


typedef struct DBAccountToDB
{
		MsgHeader      msgHeadInfo;
		int            iUserID;
		int            iSiteID;
		int            iCurGameTime;
		int            iCurGiftID;
		char           cUserName[32];
}DBAccountToDBRes;


//DBAuthenResDef 返回各种类型
#define  RETURN_USER_AGAIN_LOGIN                    1          //玩家掉线重入游戏
#define  DBRETURN_CANNOT_FIND_USER_NAME             2          //用户名不存在
#define  DBRETURN_PASSWORD_ERROR                    3          //帐号密码错误       ?
#define  DBRETURN_ACCOUNT_DISABLE_USING             4          //账号被禁用         ?
#define  DBRETURN_CANNOT_FIND_USERID                5          //用户ID不存在
#define  DBRETURN_TOKEN_ERROR                       6          //TOKEN错误
#define  DBRETURN_ALREADY_IN_GAME                   7          //账号已经在游戏中
#define  DBRETURN_DB_OPERATE_ERROR                  8          //数据操作异常       ?
#define  DBRETURN_INSERT_PLAYING_STATE_ERROR        9          //插入在线状态失败    ?
#define  DBRETURN_MACHINE_ERROR                     10         //机器码错误
#define  DBRETURN_IP_ERROR                          11         //IP错误 已经被禁用
#define  DBRETURN_DEFAULT_LOGIN                     12         //固定机器登录


//游戏基础道具信息 
typedef struct tagGamePropInfo
{
	  int                         iBuffA0;
    int                         iBuffA1;
    int                         iBuffA2;
    int                         iBuffA3;
    int                         iBuffA4;
    int                         iBuffB0;
    int                         iBuffB1;
    int                         iBuffB2;
    int                         iBuffB3;
    int                         iBuffB4;
}tagGamePropInfoDef;

//公用道具信息
typedef struct tagBasePropInfo
{
    int                         iBufferA0;
    int                         iBufferA1;
    int                         iBufferA2;
    int                         iBufferA3;
    int                         iBufferA4;
    int                         iBufferA5;
    int                         iBufferA6;
    int                         iBufferA7;
    int                         iBufferA8;
    int                         iBufferA9;
}tagBasePropInfoDef;


typedef struct DBAuthenRes
{
    MsgHeader                   msgHeadInfo;
	int							iUserID;
    char                        cReturnType;            //返回类型
    char                        szUserName[NAME_LEN];   //用户名
    char						szNickName[NAME_LEN];   //玩家昵称
    char                        szComputerID[NAME_LEN+1];//机器码
    char                        szAreaName[20];         //玩家地区名
    char                        szIP[20];               //玩家IP

    unsigned char               cSexType;               //性别，1男0女
    unsigned char               cVipType;               //VIP会员等级
    unsigned char               cMasterType;            //管理等级

    unsigned char               ucRegisterPresent;      //注册赠送标志
    unsigned char               ucSpecialIdentify;      //特殊身份
    unsigned char               ucSpecialAvatar;        //特殊头像
    char                        cPhotoVerify;           //自定图像是否审核通过
    
    unsigned short              usIconNum;              //人物图像图标号
    int                         iPhotoKey;              //自定义图像ID
    int                         iUserMasterRight;       //管理权限
    int                         iAreaID;                //玩家地区ID
    int                         iTypeID;                //玩家账号申请区间ID
    int                         iUserLevel;             //用户等级
    long long                   iFirstMoney;            //一级货币(银子)
    long long                   iBankMoney;             //玩家银行储蓄
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    long long                   llAllPlayCoin;          //总玩？
    long long                   llAllWinCoin;           //总赢？
    long long                   llAllCtrCoinA;          //A类总净分？
    long long                   llAllCtrCoinB;          //B类总净分？
    
    int                         iControlNum;            //充值贡献度？
    int                         iLotteryCtrNum;         //奖券贡献度？
    int                         iTaskLotteryNum;        //任务获得奖券？
    int                         iPropNum;               //道具数目？
    
    int                         iSecondMoney;           //二级货币
    int                         iYBNum;                 //元宝
    //针对单个游戏的输赢
    int                         iWinNum;                //胜利次数
    int                         iLostNum;               //失败次数
    int                         iDrawNum;               //和局盘数
    int                         iOffLineNum;            //玩家断线次数
    int                         iExpValue;              //用户经验
    int                         iGameExpValue;          //游戏中的经验值  
    int                         iPrizeNum;              //奖品券数量
    long                        lOnLineTime;            //玩家在线时间
    
    int                         iMatchTicket;           //单个游戏参赛券数目
    int                         iTakeCount;             //单个游戏参加比赛次数
    long long                   iGameScore;             //单个游戏的游戏积分
    long long                   iMatchScore;            //单个游戏的比赛积分

	int                         iIsBindMobile;          //是否绑定手机   
    unsigned long				iPraiseCount;			//玩家被赞的次数	
	unsigned long				iRidiculeCount;			//玩家被吐槽的次数	
    //针对所有游戏
    //int                         iCompeteControlValue;   //控制值
    //int                         iPayContributionValue;  //充值贡献度
    //int                         iYBConttributionValue;  //元宝贡献度
    
    tagBasePropInfoDef          tagBasePropInfo;
    tagGamePropInfoDef          tagGamePropInfo;

    unsigned long ulPlayerNode;
    unsigned long ulPlayerNodeId;
    
    // 给游戏的预留数据库字段
    /*
    */
}DBAuthenResDef;


typedef struct DBAuthenRes_2                           //掉线重入数据库返回
{
    MsgHeader            msgHeadInfo;
	int					 iUserID;
    char                 cReturnType;
    unsigned short       usPlayServerID;
    unsigned short       usPlayGameID;
    unsigned long        ulPlayerNode;
    unsigned long        ulPlayerNodeId;

}DBAuthenRes_2Def;

/****************************这里这个小是根据玩实际游戏长度来判断,当在GAME_WAIT_INFO 状态的话，直接清理PlayerServerID 和PlayerGameID*******
 ****************************在其他状态GAME_WAIT_DESK或者游戏中计费时候,先计费PlayBaseInfoDef， 然后呢还得清理ServerID和GameID***********
 *****************************************************************************************************************************/
typedef struct PlayBaseInfo
{
    long long                   llRDMoney;              //累积计费，这里还是有必要改，直接更新现有状态总会有问题
    long long                   iRDWinMoney;            //赢分,用于计费
    long long                   llGameScore;            //单个游戏积分   (非累计计费 直接set  而不是 set value = value + llGameScore)
    long long                   llMatchScore;           //单个游戏比赛积分 非累计计费
    int                         iSecondMoney;           //第二货币 银券通
    
    int                         iGameID;
    int                         iRDAmount;              //用于计费 ，目前仅仅只累计了iFirstMoney
    int                         iRDLostMoney;           //输分
    int                         iRDPunishMoney;         //掉线惩罚分数
    int                         iRDPunishNum;           //掉线惩罚次数 用于计费
    int                         iRDWinNum;
    int                         iRDLostNum;
    int                         iRDDrawNum;              //和局盘数
    int                         iMatchTicket;            //单个游戏参赛券数目  非累计计费
    int                         iTakeCount;              //单个游戏参加比赛次数 非累计计费
    
    int                         iRDAllPlayCoin;          //玩家的总完积分？
    int                         iRDAllWinCoin;           //玩家的总赢积分？
    int                         iRDCtrValueA;            //A类控制控制值？
    int                         iRDCtrValueB;            //B类控制值？
    
    int                         iRDControlNum;           //充值贡献度？
    int                         iRDLotteryCtrNum;        //奖券贡献度？
    
    int                         iRDPropNum;              //道具？
    int                         iRDTaskNUm;              //任务获得奖券？

    int                         iRDExpValue;             //用户经验
    int                         iRDGameExpVlue;          //单个游戏中的经验
    
    int                         iRDOnlineTime;           //在线时间
    int                         iRDPlayTime;             //玩家游戏时间  
    
    int                         iRDYuanBao;              //元宝计费？
    int                         iRDPrizeNum;             //奖品券数量
    char                        cCleanFlag;              //0表示不需要清理PlayerServerID 和playerGameID  1需要清理
    char                        cMobileType;             //移动通讯 
    tagBasePropInfoDef          tagBasePropInfo;
    tagGamePropInfoDef          tagGamePropInfo;         //
}PlayBaseInfoDef;

typedef struct RadiusUserInfoDisconnect    //这里需要清理PlayerServerID 和playerGameID信息，不需要计费处理
{
    MsgHeader       msgHeadInfo;
    int             iUserID;
}RadiusUserInfoDisconnectDef;

typedef struct RadiusDisconnect            //玩家等待入座状态下线通知，具体需要还是不需要清理 要看PlayBaseInfoDef中的cCleanFlag
{
    MsgHeader       msgHeadInfo;
    int             iUserID;
    PlayBaseInfoDef playBaseInfo;
}RadiusDisconnectDef;

//红包发送LOG记录
typedef struct UserHongBaoInfo
{
	HEADER;
	int iUserID;
	char szUserName[32];                //玩家用户名 //add
	int iGameID;
	int iServerID;                      //对应的游戏服务器
	int iHongBaoType;                   //
	int iCoinNum;                       //积分
	int iPrizeNum;                      //奖品
	int iMatchTicketNum;                //参赛券
	int iCoinAward;                     //银券通
	int iInGotNum;                      //元宝
}UserHongBaoInfoDef;


/*********************************活动相关***************************************/

typedef struct tagDBUserActivityProgressReq
{
	HEADER;
	unsigned  long  ulPlayerNode;
	unsigned  long  ulPlayerNodeId;
	int       iUserID;
	int       iGameID;
	int       iServerID;
};
//玩家活动进度
typedef struct tagDBUserActivityProgressRes
{
	int     iUserID;                    //用户ID
	int     iActivityID;                //活动ID
	int     iSiteID;                    //站点ID
	int     iGameID;                    //游戏ID
	int     iServerID;                  //房间ID
	int     iTypeID;                    //类型ID
	int     iGameCoin;                  //已获得的游戏积分, 用做上限判断
	int     iPrizeTicket;               //已获得的奖品券, 用做上限判断
	int     iCoinTicket;                //已获得的银券通, 用做上限判断
	int     iActivityTicket;            //已获得的活动券, 用做上限判断
	int     iLotteryTicket;             //已获得的抽奖券, 用做上限判断
	int     iProgress1;                 //进度1
	int     iProgress2;                 //进度2
	int     iUpdateTime;                //最后更新进度的时间
}DBUserActivityProgressRes, *PDBUserActivityProgressRes;

//玩家活动进度列表
typedef struct tagDBActivityProgressList
{
	HEADER;
	unsigned  long  ulPlayerNode;
	unsigned  long  ulPlayerNodeId;
	int     iCount;
	DBUserActivityProgressRes uap[20];
}DBActivityProgressList, *PDBActivityProgressList;

//玩家数据更新
typedef struct tagDBActivityUserData
{
	HEADER;
	int     iUserID;                    //用户ID
	int     iActivityTicket;            //活动券
	int     iLotteryTicket;             //抽奖券
	int     iActivityID;                //活动ID
	int     iGameID;                    //游戏ID
}DBActivityUserData, *PDBActivityUserData;


//活动日志
typedef struct tagDBSiteActivityLog
{
	HEADER;
	int     iUserID;                    //用户ID
	int     iActivityID;                //活动ID
	int     iSiteID;                    //站点ID
	int     iGameID;                    //游戏ID
	int     iServerID;                  //房间ID
	int     iTypeID;                    //类型ID
	int     iGameCoin;                  //游戏积分
	int     iPrizeTicket;               //奖品券
	int     iCoinTicket;                //银券通
	int     iActivityTicket;            //活动券
	int     iLotteryTicket;             //抽奖券
	char    szIPAddress[16];            //IP地址
	int     iStatus;                    //活动状态
}DBSiteActivityLog, *PDBSiteActivityLog;


//玩家活动进度更新
typedef struct tagDBUserActivityProgressUpdate
{
	HEADER;
	int     iUserID;                    //用户ID
	int     iActivityID;                //活动ID
	int     iSiteID;                    //站点ID
	int     iGameID;                    //游戏ID
	int     iServerID;                  //房间ID
	int     iTypeID;                    //类型ID
	int     iGameCoin;                  //已获得的游戏积分, 用做上限判断
	int     iPrizeTicket;               //已获得的奖品券, 用做上限判断
	int     iCoinTicket;                //已获得的银券通, 用做上限判断
	int     iActivityTicket;            //已获得的活动券, 用做上限判断
	int     iLotteryTicket;             //已获得的抽奖券, 用做上限判断
	int     iProgress1;                 //进度1
	int     iProgress2;                 //进度2
	int     iUpdateTime;                //最后更新进度的时间
}DBUserActivityProgressUpdate, *PDBUserActivityProgressUpdate;

/************************************************************************/


//一个玩家请求赞另外一个玩家
typedef struct tagOnePraiseAnotherReq
{
	MsgHeader				msgHeadInfo;
	int						fromUserId;			//谁准备赞玩家
	int						toUserId;			//哪个玩家被赞了
}OnePraiseAnotherReq;

typedef struct tagOnePraiseAnotherReq_Trans : public OnePraiseAnotherReq
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}OnePraiseAnotherReq_Trans, *POnePraiseAnotherReq_Trans;

//服务器广播某个玩家被另外的玩家赞了，这个玩家被赞了总共多少次
typedef struct tagOnePraiseAnotherRsp
{
	MsgHeader				msgHeadInfo;
	int						errorCode;			//操作结果	0：成功 1：失败
	int						fromUserId;			//谁准备赞玩家
	int						toUserId;			//哪个玩家被赞了
	unsigned long			totalPraiseCount;	//被赞的玩家总共被赞的次数
}OnePraiseAnotherRsp;

typedef struct tagOnePraiseAnotherRsp_Trans : public OnePraiseAnotherRsp
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}OnePraiseAnotherRsp_Trans, *POnePraiseAnotherRsp_Trans;

//一个玩家吐槽另外一个玩家
typedef struct tagOneRidiculeAnotherReq
{
	HEADER;
	int						fromUserId;			//谁准备吐槽玩家
	int						toUserId;			//哪个玩家被吐槽了
}OneRidiculeAnotherReq;

typedef struct tagOneRidiculeAnotherReq_Trans : public OneRidiculeAnotherReq
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}OneRidiculeAnotherReq_Trans, *POneRidiculeAnotherReq_Trans;

//服务器返回吐槽玩家的结果
typedef struct tagOneRidiculeAnotherRsp
{
	HEADER;
	int						errorCode;			//操作结果	0：成功 >=1：失败
	int						fromUserId;			//谁准备吐槽玩家
	int						toUserId;			//哪个玩家被吐槽了
	unsigned long			totalRidiculeCount;	//被吐槽的玩家总共被吐槽的次数
}OneRidiculeAnotherRsp;

typedef struct tagOneRidiculeAnotherRsp_Trans : public OneRidiculeAnotherRsp
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}OneRidiculeAnotherRsp_Trans, *POneRidiculeAnotherRsp_Trans;

#endif /* _ODAO_AUTHEN_MSG_H_ */


