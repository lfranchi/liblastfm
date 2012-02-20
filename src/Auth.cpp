
#include "ws.h"

#include "Auth.h"

lastfm::Auth::Auth()
{
}


QNetworkReply*
lastfm::Auth::getSessionInfo()
{
    QMap<QString, QString> map;
    map["method"] = "Auth.getSessionInfo";
    if (!lastfm::ws::Username.isEmpty()) map["username"] = lastfm::ws::Username;
    if (!lastfm::ws::SessionKey.isEmpty()) map["sk"] = lastfm::ws::SessionKey;
    return nam()->get( QNetworkRequest( lastfm::ws::url( map ) ) );
}
