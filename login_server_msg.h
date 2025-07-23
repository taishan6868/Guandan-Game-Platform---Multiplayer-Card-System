#ifndef __LOGIN_SERVER_MSG_H__
#define __LOGIN_SERVER_MSG_H__

#include "Game/NetMessage.h"

//************************************************************
//************* 登录服务器 -> 验证服务器, 管理服务器 *************
//************************************************************

enum
{
    LOGIN_SERVER_ID_NOTICE = 0xA000
};

//登录服务器通知自己的ID给验证服务器和管理服务器
typedef struct tagLoginServerIdNotice
{
    HEADER;
    unsigned long ulLoginServerID;
}LoginServerIdNotice, *PLoginServerIdNotice;

#endif //__LOGIN_SERVER_MSG_H__
