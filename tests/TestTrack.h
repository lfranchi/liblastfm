#include <QtTest>
#include <lastfm/Track>

using lastfm::Track;


class TestTrack : public QObject
{
    Q_OBJECT
    
    Track example()
    {
        lastfm::MutableTrack t;
        t.setTitle( "Test Title" );
        t.setArtist( "Test Artist" );
        t.setAlbum( "Test Album" );
        return t;
    }
    
private slots:
    void testClone()
    {
        Track original = example();
        Track copy = original.clone();
        
        #define TEST( x ) QVERIFY( original.x == copy.x )
        TEST( title() );
        TEST( artist() );
        TEST( album() );
        #undef TEST
    }
};
