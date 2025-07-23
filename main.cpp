#include "AuthenServer.h"
#include "ServerDef.h"

void handler(int sigal)
{
}

int main(int argc, char *argv[])
{
    InitializeLog(CONFIG_FILE, LOG_FILE);

    __log(_DEBUG, "main", "**************** staring %s v1.0.0.0 ****************\r\n", SERVER_NAME);

    IGameProvider *lpGameProvider = new AuthenServer;

    if (0 == lpGameProvider->Start())
    {
        signal(SIGTERM, handler);

        pause();
    }

    delete lpGameProvider;

    __log(_DEBUG, "main", "%s has been exit !\r\n", argv[0]);

    return 0;
}
