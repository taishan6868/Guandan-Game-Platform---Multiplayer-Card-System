#ifndef __DBINFODEF_H__
#define __DBINFODEF_H__

#include "Game/NetMessage.h"

typedef struct tagDBLoginInfo
{
    int         iGender;                       //用户性别
    int         iUserID;                       //用户I D
    long long   nScore;                        //用户银子
}DBLoginInfo, *PDBLoginInfo;

typedef struct tagDBGameInfo
{
    int                         iGameID;                    //用户所在游戏ID
    int                         iServerID;                  //用户所在服务器ID
    char                        szUserName[NAME_LEN];       //用户名
    char                        szComputerID[NAME_LEN + 1]; //机器码
    char                        szAreaName[20];             //玩家地区名
    char                        szIP[20];                   //玩家IP

    unsigned char               cSexType;                   //性别，1男0女
    unsigned char               cVipType;                   //VIP会员等级
    unsigned char               cMasterType;                //管理等级

    unsigned char               ucRegisterPresent;          //注册赠送标志
    unsigned char               ucSpecialIdentify;          //特殊身份
    unsigned char               ucSpecialAvatar;            //特殊头像

    unsigned short              usIconNum;                  //人物图像图标号
    int                         iUserMasterRight;           //管理权限
    int                         iAreaID;                    //玩家地区ID
    int                         iUserLevel;                 //用户等级
    long long                   iFirstMoney;                //一级货币(银子)
    long long                   iBankMoney;                 //玩家银行储蓄
    int                         iSecondMoney;               //二级货币
    int                         iYBNum;                     //元宝
    //针对单个游戏的输赢
    int                         iWinNum;                    //胜利次数
    int                         iLostNum;                   //失败次数
    int                         iDrawNum;                   //和局盘数
    int                         iOffLineNum;                //玩家断线次数
    int                         iExpValue;                  //用户经验
    int                         iPrizeNum;                  //奖品券数量
    long                        lOnLineTime;                //玩家在线时间

    //针对所有游戏
    int                         iCompeteControlValue;       //控制值
    int                         iPayContributionValue;      //充值贡献度
    int                         iYBConttributionValue;      //元宝贡献度
}DBGameInfo, *PDBGameInfo;

typedef struct tagDBUpdateAccount
{
    long long   llMoney;        //输赢银子
    int         iExpValue;      //经验值
    int         iOnlineTime;    //在线时间
    int         iPrizeNum;      //奖品券数量
}DBUpdateAccount, *PDBUpdateAccount;

typedef struct tagDBCoinData
{
    long long   llGameCoin;     //身上银子
    long long   llBankCoin;     //银行存款
}DBCoinData, *PDBCoinData;


#endif //__DBINFODEF_H__
