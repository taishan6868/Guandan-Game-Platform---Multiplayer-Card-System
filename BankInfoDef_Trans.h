/*
 * BankInfoDef_Trans.h
 *
 *  Created on: 2012-7-17
 *      Author: root
 */

#ifndef _BANKINFODEF_TRANS_H_
#define _BANKINFODEF_TRANS_H_

#include "BankInfoDef.h"
enum
{
    RD_USER_LOGIN_BANK_REQ = 0XD6,
    RD_USER_LOGIN_RESULT = 0XD7,
    RD_GET_BANK_COIN_REQ = 0XD8,
    RD_GET_COIN_RESULT_RES = 0XD9,
    RD_USER_SAVE_COIN_REQ = 0XDA,
    RD_USER_SAVE_COIN_RES = 0XDB,
    RD_MODIFY_BANK_PASSWD_REQ = 0XDC,
    RD_MODIFY_BANK_PASSWD_RES = 0XDD
};


typedef struct UserBankLoginReq_trans :public UserBankLoginReqDef
{
    unsigned long   ulPlayerNode;                                       //返回大厅客户端连接对象(必须在结构体最后)
    unsigned long   ulPlayerNodeId;                                     //返回大厅客户端连接对象ID(必须在结构体最后)
}UserBankLoginReq_trans;

typedef struct LoginBankResult_trans :public LoginBankResultDef
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LoginBankResult_trans;

typedef struct GetBankCoinReq_trans :public GetBankCoinReqDef
{
    int             iServerID;
    int             iGameID;
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}GetBankCoinReq_trans;

typedef struct GetCoinResult_trans :public GetCoinResultDef
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}GetCoinResult_trans;

typedef struct SaveCoinReq_trans :public SaveCoinReqDef
{
    int             iServerID;
    int             iGameID;
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
    long long       llModifyCoin;                          //内存中计费
}SaveCoinReq_trans;

typedef struct SaveCoinRes_trans :public SaveCoinResDef
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}SaveCoinRes_trans;

typedef struct ModifyBankPasswdReq_trans :public ModifyBankPasswdReqDef
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}ModifyBankPasswdReq_trans;

typedef struct ModifyBankPasswdRes_trans :public ModifyBankPasswdResDef
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}ModifyBankPasswdRes_trans;

#endif /* _BANKINFODEF_TRANS_H_ */
