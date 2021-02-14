#include "shuttle.h"





Shuttle :: Shuttle( int sp, int x, int y, const char *file )
        :  speed( sp )
{
  
    picture = new QPixmap( file );
    if ( picture->isNull() ) 
	fatal( "cannot initialize shuttle" );

    enclosing = QRect( x, y, picture->width(), picture->height() ); 
}


void
Shuttle :: move( QPaintDevice *qpd , int lo, int hi, int dir )
{

    switch ( dir ) {
    case 1:
	if ( enclosing.x() - speed > lo ) 
	    enclosing.moveBy( -speed, 0 );
	break ;
    case 2:
	if ( enclosing.right() + speed < hi ) 
	    enclosing.moveBy( speed, 0 );
	break ;
    }
  
    bitBlt( qpd, enclosing.x(), enclosing.y(), picture );
}


void
Shuttle :: redraw( QPaintDevice *qpd )
{

    bitBlt( qpd, enclosing.x(), enclosing.y(), picture );
}


int
Shuttle :: getBulletX()
{

    return enclosing.center().x();
}


int
Shuttle :: getBulletY()
{
  
    return enclosing.y();
}



