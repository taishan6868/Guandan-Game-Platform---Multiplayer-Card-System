#ifndef __AUTHENPLAYER_H__
#define __AUTHENPLAYER_H__

#include "Game/PlayerNode.h"

class AuthenPlayer : public PlayerNode
{
public:
    unsigned long m_ulServerID;

protected:
    virtual void Initialize()
    {
        m_ulServerID = 0;
    }
};

#endif //__LOGINPLAYER_H__
