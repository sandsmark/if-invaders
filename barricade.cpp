#include "barricade.h"

#include "pixmaps/barricade2.xpm"




BarricadeSegment :: BarricadeSegment( int off )
{

    lump = new QPixmap( wall3 );
    if ( lump->isNull() ) 
	fatal( "no segment pix" );


    top = std::vector< uchar >( lump->width() );     // offset from above
    bottom = std::vector< uchar >( lump->width() );  // offset from below

    restart();

    enclosing = QRect( 0, 0, lump->width(), lump->height() );
}


BarricadeSegment :: ~BarricadeSegment()
{

    delete lump;
}


void
BarricadeSegment :: restart()
{
    QImage qim = lump->convertToImage();

    // don't ask why - u don't want to know. Remember, this O(width^2)
    // thing isn't called very often
    for ( int j = 0; j < qim.width(); ++j ) {
	for ( int i = 0; i < qim.height(); ++i ) {
	    if ( qRed( qim.pixel(j,i) ) != 0 ||
		 qBlue( qim.pixel(j,i) ) != 0 ||
		 qGreen( qim.pixel(j,i) ) != 0 ) {
		top[j] = i;
		break;
	    }
	}
    }
      
    for ( int j = 0; j < qim.width(); ++j ) {
	for ( int i = qim.height()-1; i; --i ) {
	    if ( qRed( qim.pixel(j,i) ) != 0 ||
		 qBlue( qim.pixel(j,i) ) != 0 ||
		 qGreen( qim.pixel(j,i) ) != 0 ) {
		bottom[j] = i;
		break;
	    }
	}
    }
}


void
BarricadeSegment :: place( int rel_x )
{

    enclosing.moveTopLeft( QPoint( rel_x, 0 ) );
}


// WARNING: ugly code
bool
BarricadeSegment :: takeDamageFromBelow( const QRect &qr )
{
    bool flag = false;
    int hi = qr.top(),
	lf = qr.left() - enclosing.left(), ri = lf + qr.width();

    // we are on the side of the segment. The last test should NEVER
    // happen... now why the heck did I include it...?
    if ( lf < 0 || hi < 0 ) 
	return false;  

    if ( (int)bottom[ lf + qr.width()/2 ] > hi ) {
	for ( int i = lf; i <= ri; ++i ) {
	    bottom[i] = hi;
	    if ( bottom[i] <= top[i] ) {
		bottom[i] = 0;
		top[i] = UCHAR_MAX;
	    }
	}
	flag = true;
    }
    
    return flag;
}


// WARNING: ugly code
bool
BarricadeSegment :: takeDamageFromAbove( const QRect &qr )
{
    bool flag = false;
    int lo = qr.bottom(),
	lf = qr.left() - enclosing.left(), ri = lf + qr.width();

    if ( lf < 0 ) 
	return false;

    if ( static_cast<int>( top[ lf + qr.width()/2 ] ) < lo ) {
	for ( int i = lf; i <= ri; i++ ) {
	    top[i] = lo;

	    // once there is a hole, shots must get thru in both directions
	    if ( top[i] >= bottom[i] ) {
		bottom[i] = 0;           // guaranteed to be too small
		top[i] = UCHAR_MAX;      // this col is free
	    }

	    flag = true;
	}
    }

    return flag;
}


void
BarricadeSegment :: refresh( QPaintDevice *dst )
{

    bitBlt( dst, enclosing.topLeft(), lump );
}





Barricade :: Barricade( QWidget *qpd, int top, int width, int maxp )
          :  maxBlocks( (unsigned int)maxp )
{

    disableFlag = false;
    bricks = std::vector< BarricadeSegment* >( maxBlocks );

    for ( unsigned int i = 0; i < maxBlocks; ++i )
	bricks[i] = new BarricadeSegment( i );

    wallSize = QSize( bricks[0]->getSize() );
    innerWallSpace = ( width - maxBlocks*wallSize.width() ) / (maxBlocks+1);

    buffer = new QPixmap( wallSize.width()*maxBlocks +
			  innerWallSpace*(maxBlocks+1), wallSize.height() );
    buffer->fill( qpd, 0, top );

    buffDrawer = new QPainter( buffer );
    buffDrawer->setBackgroundColor( qpd->backgroundColor() ); 

    enclosing = QRect( 0, top, buffer->width(), buffer->height() );

    for ( unsigned int i = 0; i < maxBlocks; ++i )
	bricks[i]->place( innerWallSpace*(i+1) + i*wallSize.width() );

    // draw basic blocks
    refresh();
}  


Barricade :: ~Barricade()
{

    delete buffDrawer;
    delete buffer;
}


void
Barricade :: refresh( QPaintDevice *dstWindow )
{

    if ( disableFlag ) 
	return ;

    bitBlt( dstWindow, enclosing.topLeft(), buffer );
}


void
Barricade :: refresh()
{

    for ( VBSI iter = bricks.begin(); iter < bricks.end(); ++iter )
	(*iter)->refresh( buffer );
}

  
bool
Barricade :: takeDamageFromAbove( const QRect &qr )
{

    if ( disableFlag ) 
	return false;

    // convert to local coords
    QRect r = QRect( qr );
    r.moveBy( 0, -enclosing.top() );
  
    int index = r.x() / ( innerWallSpace + bricks[0]->getWidth() );

    if ( 0 <= index && index < (int)maxBlocks && 
	 bricks[index]->takeDamageFromAbove( r ) ) {
	buffDrawer->eraseRect( r );
	return true;
    }

    return false;
}


bool
Barricade :: takeDamageFromBelow( const QRect &qr )
{

    if ( disableFlag ) 
	return false;

    QRect r = QRect( qr );
    r.moveBy( 0, -enclosing.top() );

    // inconsistency as compared to above func - it's easier to check
    // this locally. Actually, it should be topMiddle()
    if ( !enclosing.contains( qr.topLeft() ) ) 
	return false;

    int index = r.x() / ( innerWallSpace + bricks[0]->getWidth() );

    if ( 0 <= index && index < (int)maxBlocks &&
	 bricks[index]->takeDamageFromBelow( r ) ) {
	buffDrawer->eraseRect( r );
	return true;
    }

    return false;
}


bool
Barricade :: intersects( const QRect &area )
{

    return enclosing.intersects( area );
}  
  
    
void
Barricade :: restart( QPaintDevice *qpd )
{

    disableFlag = false;

    for ( VBSI iter = bricks.begin(); iter < bricks.end(); ++iter )
	(*iter)->restart();

    refresh();
    refresh( qpd );
}


void
Barricade :: disable( QWidget *qpd )
{

    disableFlag = true;
    buffer->fill( qpd, enclosing.topLeft() );
    qpd->erase( enclosing );
}


int
Barricade :: getY()
{  

    return enclosing.y(); 
}
