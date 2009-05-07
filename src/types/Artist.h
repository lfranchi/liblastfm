/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef LASTFM_ARTIST_H
#define LASTFM_ARTIST_H

#include <lastfm/global.h>
#include <QMap>
#include <QString>
#include <QUrl>


namespace lastfm
{
    class LASTFM_DLLEXPORT Artist
    {
        QString m_name;
        QList<QUrl> m_images;

    public:
        Artist()
        {}

        Artist( const QString& name ) : m_name( name )
        {}

        /** will be QUrl() unless you got this back from a getInfo or something call */
        QUrl imageUrl( ImageSize size = Large ) const { return m_images.value( size ); }

        bool isNull() const { return m_name.isEmpty(); }
        
        /** the url for this artist's page at www.last.fm */
        QUrl www() const;
    
        bool operator==( const Artist& that ) const { return m_name == that.m_name; }
        bool operator!=( const Artist& that ) const { return m_name != that.m_name; }
    
        operator QString() const 
        {
            /** if no artist name is set, return the musicbrainz unknown identifier
              * in case some part of the GUI tries to display it anyway. Note isNull
              * returns false still. So you should have queried that! */
            return m_name.isEmpty() ? "[unknown]" : m_name;
        }
        QString name() const { return QString(*this); } 
    
        QNetworkReply* share( const class User& recipient, const QString& message = "" );

        QNetworkReply* getInfo() const;
        static Artist getInfo( QNetworkReply* );
    
        QNetworkReply* getSimilar() const;
        /** The match percentage is returned from last.fm as a 4 significant 
          * figure floating point value. So we multply it by 100 to make an 
          * integer in the range of 0 to 10,000. This is possible confusing 
          * for you, but I felt it best not to lose any precision, and floats 
          * aren't much fun. */
        static QMap<int, QString> getSimilar( QNetworkReply* );
    
        /** use Tag::list to get the tag list out of the finished reply */
        QNetworkReply* getTags() const;
        QNetworkReply* getTopTags() const;
    
        /** Last.fm dictates that you may submit at most 10 of these */
        QNetworkReply* addTags( const QStringList& ) const;
    
        QNetworkReply* search( int limit = -1 ) const;
        static QList<Artist> list( QNetworkReply* );
        
        QMap<QString, QString> params( const QString& method ) const;
    };
}

#endif
