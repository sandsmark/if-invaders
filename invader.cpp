#include "invader.h"





// definition of static members
QPixmap* SillyInvader :: pix1 = 0;
QPixmap* SillyInvader :: pix2 = 0;

QPixmap* CleverInvader :: pix1 = 0;
QPixmap* CleverInvader :: pix2 = 0;

QPixmap* FatInvader :: pix1 = 0;
QPixmap* FatInvader :: pix2 = 0;





Invader :: Invader()
        :  enclosing( 0, 0, 0, 0 )
{

    alive = false;
}


void
Invader :: place( int x, int y, int w, int h )
{

    enclosing = QRect( x, y, w, h );
}


void
Invader :: paint( QPaintDevice *b1, QPaintDevice *b2 )
{
    QPixmap *pix = getPix( 1 );
    bitBlt( b1, enclosing.center().x() - pix->width()/2,
	    enclosing.center().y() - pix->height()/2, pix );

    pix = getPix( 2 );
    bitBlt( b2, enclosing.center().x() - pix->width()/2,
	    enclosing.center().y() - pix->height()/2, pix );
}


bool
Invader :: contains( const QRect& )
{

    fatal( "wrong contains()" ); 
    return false;
}


QPixmap*
Invader :: getPix( int )
{

    fatal( "wrong getPix()" ); 
    return 0;
}


int
Invader :: getWidth()
{

    fatal( "wrong getWidth()" );
    return 0;
}


int
Invader :: getHeight()
{

    fatal( "wrong getHeight()" );
    return 0;
}


unsigned int
Invader :: getPoints()
{
    
    fatal( "wrong getPoints" );
    return 0;
}





SillyInvader :: SillyInvader()
             :  Invader()
{
    // EMPTY
}


SillyInvader :: ~SillyInvader()
{
    // EMPTY
}


void
SillyInvader :: initPicture( const char *file, const char *file2 )
{

    pix1 = new QPixmap( file );
    pix2 = new QPixmap( file2 );

    if ( pix1->isNull() ) 
	fatal( "init failure" );
}


bool
SillyInvader :: contains( const QRect &t )
{
    QRect r = pix1->rect();
    
    r.moveCenter( QPoint( enclosing.center() ) );
    return ( r.contains( t ) );
}


// don't worry about these magic numbers - they are all internal
QPixmap*
SillyInvader :: getPix( int i )
{
  
    return ( i == 1 ) ? pix1 : pix2;
}





CleverInvader :: CleverInvader()
              :  Invader()
{
    // EMPTY
}


CleverInvader :: ~CleverInvader()
{
    // EMPTY
}


void
CleverInvader :: initPicture( const char *file, const char *file2 )
{

    pix1 = new QPixmap( file );
    pix2 = new QPixmap( file2 );

    if ( pix1->isNull() ) 
	fatal( "init failure\n ");
}


bool
CleverInvader :: contains( const QRect &t )
{
    QRect r = pix1->rect();

    r.moveCenter( QPoint( enclosing.center() ) );
    return ( r.contains( t ) );
}


QPixmap*
CleverInvader :: getPix( int i )
{
  
    return ( i == 1 ) ? pix1 : pix2;
}





FatInvader :: FatInvader()
            : Invader()
{
    // EMPTY
}


FatInvader :: ~FatInvader()
{
    // EMPTY
}


void
FatInvader :: initPicture( const char *file, const char *file2 )
{

    pix1 = new QPixmap( file );
    pix2 = new QPixmap( file2 );
  
    if ( pix1->isNull() ) 
	fatal( "init failure" );
}


bool
FatInvader :: contains( const QRect &t )
{
    QRect r = pix1->rect();

    r.moveCenter( QPoint( enclosing.center() ) );
    return ( r.contains( t ) );
}


QPixmap*
FatInvader :: getPix( int i )
{
  
    return ( i == 1 ) ? pix1 : pix2;
}





BigBoss :: BigBoss( int y )
{

    initPicture();
    height = y;
    enclosing = QRect( 0, height, pix->width(), pix->height() );
    speed = 4;
}


BigBoss :: ~BigBoss()
{

    delete pix;
}


void
BigBoss :: initPicture( const char *file )
{
  
    pix = new QPixmap( file );
    if ( pix->isNull() ) 
      fatal( "cannot init boss" );
}


bool
BigBoss :: contains( const QRect &qr )
{

    return qr.intersects( enclosing );
}


bool
BigBoss :: move( QPaintDevice *qpd, int maxRight )
{

    QPainter p( qpd );
    p.eraseRect( enclosing );
    p.end();
    
    if ( enclosing.x() < maxRight ) {
	enclosing.moveBy( speed, 0 );
	bitBlt( qpd, enclosing.x(), enclosing.y(), pix );
	return true;
    }

    reset();
    return false;
}

  
void
BigBoss :: reset()
{

    enclosing.moveTopLeft( QPoint( 0, height ) );
}





