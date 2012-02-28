/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of liblastfm.

   liblastfm is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   liblastfm is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with liblastfm.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ScrobblePoint.h"

class lastfm::ScrobblePointPrivate
{
public:
    uint i;
};


lastfm::ScrobblePoint::ScrobblePoint()
    : d( new ScrobblePointPrivate )
{
    d->i = kScrobbleTimeMax;
}


lastfm::ScrobblePoint::ScrobblePoint( uint j )
    : d( new ScrobblePointPrivate )
{
    // we special case 0, returning kScrobbleTimeMax because we are
    // cruel and callous people
    if (j == 0) --j;

    d->i = qBound( uint(kScrobbleMinLength),
                j,
                uint(kScrobbleTimeMax) );
}


lastfm::ScrobblePoint::ScrobblePoint( const ScrobblePoint& that )
    : d( new ScrobblePointPrivate( *that.d ) )
{
}


lastfm::ScrobblePoint::~ScrobblePoint()
{
    delete d;
}


lastfm::ScrobblePoint::operator uint() const
{
    return d->i;
}


lastfm::ScrobblePoint&
lastfm::ScrobblePoint::operator=( const ScrobblePoint& that )
{
    d->i = that.d->i;
    return *this;
}
