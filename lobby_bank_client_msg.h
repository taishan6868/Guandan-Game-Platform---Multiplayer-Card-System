#ifndef __LOBBY_BANK_CLIENT_MSG_H__
#define __LOBBY_BANK_CLIENT_MSG_H__

#pragma pack(push, 4)

#include "Game/NetMessage.h"

enum
{
	//大厅客户端发起
	LOBBY_BANK_CLIENT_AUTHEN_REQ = 0x8800,
	LOBBY_BANK_CLIENT_GET_COIN_REQ,
	LOBBY_BANK_CLIENT_SAVE_COIN_REQ,
	LOBBY_BANK_CLIENT_MODIFY_PASSWORD_REQ,

	//登录服务器发起
	LOBBY_BANK_CLIENT_AUTHEN_SUCCESS_RSP,
	LOBBY_BANK_CLIENT_AUTHEN_ERROR_RSP,
	LOBBY_BANK_CLIENT_GET_COIN_SUCCESS_RSP,
	LOBBY_BANK_CLIENT_GET_COIN_ERROR_RSP,
	LOBBY_BANK_CLIENT_SAVE_COIN_SUCCESS_RSP,
	LOBBY_BANK_CLIENT_SAVE_COIN_ERROR_RSP,
	LOBBY_BANK_CLIENT_MODIFY_PASSWORD_SUCCESS_RSP,
	LOBBY_BANK_CLIENT_MODIFY_PASSWORD_ERROR_RSP
};

//LOBBY_BANK_CLIENT_AUTHEN_REQ
typedef struct tagLobbyBankClientAuthenReq
{
	HEADER;

	int								iUserID;						//用户ID
	char							szBankPasswd[PASSWD_LEN];		//银行密码
}LobbyBankClientAuthenReq, *LobbyBankClientAuthenReqPtr;

//LOBBY_BANK_CLIENT_AUTHEN_SUCCESS_RSP
typedef struct tagLobbyBankClientAuthenSuccessRsp
{
	HEADER;

	long long						llGameCoin;						//身上银子
	long long						llBankCoin;						//银行存款
}LobbyBankClientAuthenSuccessRsp, *LobbyBankClientAuthenSuccessRspPtr;

typedef enum LOBBY_BANK_CLIENT_ERROR
{
	ERROR_LOBBY_BANK_CLIENT_OPERATE_TIME_LIMIT_ERROR				= -3,
	ERROR_LOBBY_BANK_CLIENT_AUTHEN_SERVER_DISCONNECT				= -2,
	ERROR_LOBBY_BANK_CLIENT_EXECUTE_PROCEDURE_FAILED				= -1,
	ERROR_LOBBY_BANK_CLIENT_AUTHEN_BANK_PASS_ERROR					= 1,		//authen error res
	ERROR_LOBBY_BANK_CLIENT_GET_SAVE_NOENOUGH_COIN					= 2,		//get and save error res
	ERROR_LOBBY_BANK_CLIENT_NO_BANK_RECORD_OF_THE_USERID			= 3,
	ERROR_LOBBY_BANK_CLIENT_OLD_PASSWD_DIFFERENT					= 4,		//modify passwd error res
	ERROR_LOBBY_BANK_CLIENT_USERID_LOGIN_SOMEWHERE_ELSE				= 5,		//modify passwd error res
	ERROR_LOBBY_BANK_CLIENT_SYSTEM_BUSY								= 6,		//modify passwd error res
}LOBBY_BANK_CLIENT_ERROR;

//LOBBY_BANK_CLIENT_AUTHEN_ERROR_RSP
typedef struct tagLobbyBankClientAuthenErrorRsp
{
	HEADER;

	LOBBY_BANK_CLIENT_ERROR			lError;							//错误码
}LobbyBankClientAuthenErrorRsp, *LobbyBankClientAuthenErrorRspPtr;

//LOBBY_BANK_CLIENT_GET_COIN_REQ
typedef struct tagLobbyBankClientGetCoinReq
{
	LobbyBankClientAuthenReq		authen;
	int								iGameID;						//游戏ID
	int								iServerID;						//房间ID
	long long						llGetNum;
}LobbyBankClientGetCoinReq, *LobbyBankClientGetCoinReqPtr;

//LOBBY_BANK_CLIENT_GET_COIN_SUCCESS_RSP
typedef struct tagLobbyBankClientGetCoinSuccessRsp
{
	HEADER;

	long long						llGetNum;
}LobbyBankClientGetCoinSuccessRsp, *LobbyBankClientGetCoinSuccessRspPtr;

//LOBBY_BANK_CLIENT_GET_COIN_ERROR_RSP
typedef struct tagLobbyBankClientGetCoinErrorRsp
{
	HEADER;

	LOBBY_BANK_CLIENT_ERROR			lError;
}LobbyBankClientGetCoinErrorRsp, *LobbyBankClientGetCoinErrorRspPtr;

//LOBBY_BANK_CLIENT_SAVE_COIN_REQ
typedef struct tagLobbyBankClientSaveCoinReq
{
	LobbyBankClientAuthenReq		authen;
	int								iGameID;						//游戏ID
	int								iServerID;						//房间ID
	long long						llSaveNum;
}LobbyBankClientSaveCoinReq, *LobbyBankClientSaveCoinReqPtr;

//LOBBY_BANK_CLIENT_SAVE_COIN_SUCCESS_RSP
typedef struct tagLobbyBankClientSaveCoinSuccessRsp
{
	HEADER;

	long long						llSaveNum;
}LobbyBankClientSaveCoinSuccessRsp, *LobbyBankClientSaveCoinSuccessRspPtr;

//LOBBY_BANK_CLIENT_SAVE_COIN_ERROR_RSP
typedef struct tagLobbyBankClientSaveCoinErrorRsp
{
	HEADER;
	LOBBY_BANK_CLIENT_ERROR			lError;
}LobbyBankClientSaveCoinErrorRsp, *LobbyBankClientSaveCoinErrorRspPtr;

//LOBBY_BANK_CLIENT_MODIFY_PASSWORD_REQ
typedef struct tagLobbyBankClientModifyPasswordReq
{
	LobbyBankClientAuthenReq		authen;
	char							szNewPasswd[PASSWD_LEN];
}LobbyBankClientModifyPasswordReq, *LobbyBankClientModifyPasswordReqPtr;

//LOBBY_BANK_CLIENT_MODIFY_PASSWORD_SUCCESS_RSP
typedef struct tagLobbyBankClientModifyPasswordSuccessRsp
{
	HEADER;
}LobbyBankClientModifyPasswordSuccessRsp, *LobbyBankClientModifyPasswordSuccessRspPtr;

//LOBBY_BANK_CLIENT_MODIFY_PASSWORD_ERROR_RSP
typedef struct tagLobbyBankClientModifyPasswordErrorRsp
{
	HEADER;
	LOBBY_BANK_CLIENT_ERROR			lError;
}LobbyBankClientModifyPasswordErrorRsp, *LobbyBankClientModifyPasswordErrorRspPtr;

#pragma pack(pop)

#endif //__LOBBY_BANK_CLIENT_MSG_H__
