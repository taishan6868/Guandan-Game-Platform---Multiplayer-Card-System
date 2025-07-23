#ifndef __AUTHEN_SERVER_MSG_H__
#define __AUTHEN_SERVER_MSG_H__

#include "lobby_client_msg.h"

//************************************************************
//************       登录服务器 <-> 验证服务器         **********
//************************************************************

//登录服务器转发验证请求给验证服务器; 登录服务器 -> 验证服务器
typedef struct tagLobbyAuthenReq_Trans : public tagLobbyAuthenReq
{
    unsigned long ulPlayerNode;     //转发给验证服务器, 加上大厅客户端连接对象
    unsigned long ulPlayerNodeId;   //客户端连接对象ID
}LobbyAuthenReq_Trans, *PLobbyAuthenReq_Trans;


//不知道站点ID的情况下请求登录
typedef struct tagLobbyAuthenWithNoSiteIdReq_Trans : public tagLobbyAuthenWithNoSiteIdReq
{
	unsigned long ulPlayerNode;     //转发给验证服务器, 加上大厅客户端连接对象
	unsigned long ulPlayerNodeId;   //客户端连接对象ID

	char		  szClientIP[16];
}LobbyAuthenWithNoSiteIdReq_Trans, *PLobbyAuthenWithNoSiteIdReq_Trans;

//修改玩家昵称请求
typedef struct tagLobbySetNicknameReq_Trans : public tagLobbySetNicknameReq
{
	unsigned long ulPlayerNode;     //转发给验证服务器, 加上大厅客户端连接对象
	unsigned long ulPlayerNodeId;   //客户端连接对象ID
}LobbySetNicknameReq_Trans, *PLobbySetNicknameReq_Trans;

//修改玩家昵称回应
typedef struct tagLobbySetNicknameRsp_Trans : public tagLobbySetNicknameRsp
{
	unsigned long ulPlayerNode;     //转发给验证服务器, 加上大厅客户端连接对象
	unsigned long ulPlayerNodeId;   //客户端连接对象ID
}LobbySetNicknameRsp_Trans, *PLobbySetNicknameRsp_Trans;

//客户端登陆发送自己的设备号以及
typedef struct tagLobbySendSelfLocationReq_Trans : public tagLobbySendSelfLocationReq
{
	unsigned long ulPlayerNode;     //转发给验证服务器, 加上大厅客户端连接对象
	unsigned long ulPlayerNodeId;   //客户端连接对象ID
}LobbySendSelfLocationReq_Trans, *PLobbySendSelfLocationReq_Trans;

typedef struct tagLobbyHeChengReq_Trans : public LobbyHeCheng
{
	unsigned long   ulPlayerNode;     //转发给验证服务器, 加上大厅客户端连接对象
	unsigned long   ulPlayerNodeId;   //客户端连接对象ID
}LobbyHeChengReq_Trans, *PLobbyHeChengReq_Trans;


//验证服务器返回验证结果给登录服务器; 验证服务器 -> 登录服务器
typedef struct tagLobbyAuthenRes_Trans : public tagLobbyAuthenRes
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LobbyAuthenRes_Trans, *PLobbyAuthenRes_Trans;

//请求的时候，不知道站点ID
typedef struct tagLobbyAuthenWithNoSiteIdRes_Trans : public tagLobbyAuthenWithNoSiteIdRes
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyAuthenWithNoSiteIdRes_Trans, *PLobbyAuthenWithNoSiteIdRes_Trans;

typedef struct tagLobbyHeCheng_Trans : public tagLobbyHeChengRes
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyHeCheng_Trans, *PLobbyHeCheng_Trans;



//验证服务器返回验证结果给登录服务器; 验证服务器 -> 登录服务器
typedef struct tagLobbyAuthenRes_2_Trans : public tagLobbyAuthenRes_2
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyAuthenRes_2_Trans, *PLobbyAuthenRes_2_Trans;


typedef struct tagLobbyHeCheng_2_Trans : public tagLobbyHeChengRes_2
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyHeCheng_2_Trans, *PLobbyHeCheng_2_Trans;


typedef struct tagLobbyGetGamePrizeReq_Trans : public tagLobbyGetGamePrizeReq
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LobbyGetGamePrizeReq_Trans, *PLobbyGetGamePrizeReq_Trans;

typedef struct tagLobbyGetGamePrizeRes_Trans : public tagLobbyGetGamePrizeRes
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LobbyGetGamePrizeRes_Trans, *PLobbyGetGamePrizeRes_Trans;

typedef struct tagLobbyGetHongbaoInfoReq_Trans : public tagLobbyGetHongbaoInfoReq
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LobbyGetHongbaoInfoReq_Trans, *PLobbyGetHongbaoInfoReq_Trans;

typedef struct tagLobbyGetHongbaoInfoRes_Trans : public tagLobbyGetHongbaoInfoRes
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LobbyGetHongbaoInfoRes_Trans, *PLobbyGetHongbaoInfoRes_Trans;

typedef struct tagLobbyGetHongbaoContentReq_Trans : public tagLobbyGetHongbaoContentReq
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LobbyGetHongbaoContentReq_Trans, *PLobbyGetHongbaoContentReq_Trans;

typedef struct tagLobbyGetHongbaoContentRes_Trans : public LobbyGetHongbaoContentRes
{
    unsigned long   ulPlayerNode;
    unsigned long   ulPlayerNodeId;
}LobbyGetHongbaoContentRes_Trans, *PLobbyGetHongbaoContentRes_Trans;

typedef struct tagLobbyModifyLoginPasswordReq_Trans : public LobbyModifyLoginPasswordReqDef
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyModifyLoginPasswordReq_Trans, *PLobbyModifyLoginPasswordReq_Trans;

typedef struct tagLobbyModifyLoginPasswordRes_Trans : public LobbyModifyLoginPasswordResDef
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyModifyLoginPasswordRes_Trans, *PLobbyModifyLoginPasswordRes_Trans;

typedef struct tagLobbyGetCaiPiaoInfoReq_Trans : public tagLobbyGetCaiPiaoInfoReq
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyGetCaiPiaoInfoReq_Trans, *PLobbyGetCaiPiaoInfoReq_Trans;

typedef struct tagLobbyGetCaiPiaoInfoRes_Trans : public tagLobbyGetCaiPiaoInfoRes
{
	unsigned long   ulPlayerNode;
	unsigned long   ulPlayerNodeId;
}LobbyGetCaiPiaoInfoRes_Trans, *PLobbyGetCaiPiaoInfoRes_Trans;

#endif //__AUTHEN_SERVER_MSG_H__
