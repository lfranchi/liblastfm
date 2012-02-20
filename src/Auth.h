#ifndef LASTFM_AUTH_H
#define LASTFM_AUTH_H

#include "global.h"

namespace lastfm
{
    class LASTFM_DLLEXPORT Auth
    {
    private:
        Auth();

    public:
        static QNetworkReply* getSessionInfo();
    };
}

#endif // TASTEOMETER_H
