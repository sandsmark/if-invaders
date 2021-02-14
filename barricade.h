#ifndef BARRICADE_H
#define BARRICADE_H


#undef BARRICADEINCLUDE
#define BARRICADEINCLUDE
#include "allinclude.h"
#undef BARRICADEINCLUDE





class BarricadeSegment;
class Barricade;
typedef std::vector< BarricadeSegment* > :: iterator VBSI;





// coords are relative to Barricade
class BarricadeSegment
{

    typedef unsigned int uchar;
    QPixmap              *lump;
    std::vector< uchar >      top, bottom;
    QRect                enclosing;

    BarricadeSegment( const BarricadeSegment& ){  assert( 0 ); }
    BarricadeSegment& operator=( const Barricade& ){  assert( 0 ); }

public:

    // what the hell does 'no' do?
    BarricadeSegment( int no, const char *filename = WALLPIX );
    ~BarricadeSegment();
  
    bool   takeDamageFromBelow( const QRect &area );
    bool   takeDamageFromAbove( const QRect &area );

    QRect& getEnclosing(){  return enclosing; }
    int    getWidth()    {  return enclosing.width(); }
    int    getHeight()   {  return enclosing.height(); }
    QSize  getSize()     {  return enclosing.size(); }

    void   refresh( QPaintDevice *qpd );
    void   restart();

    void   wipe( const QRect &area );
    void   place( int rel_x );
};


// coords are RELATIVE TO gamewindow.
class Barricade
{

    std::vector< BarricadeSegment* > bricks;
    QPixmap                     *buffer;
    const unsigned int          maxBlocks;
    QRect                       enclosing;
    QSize                       wallSize;
    int                         innerWallSpace;
    QPainter                    *buffDrawer;
    bool                        disableFlag;

    // overloaded by a public function with an arg. too ugly?
    void   refresh();

    // nope, this class is neither "copyable" nor "assignable"
    Barricade( const Barricade& ) ; 
    Barricade& operator=( const Barricade& ) ;

public:

    Barricade( QWidget*, int top, int width, int maxpieces = 4 );
    ~Barricade();


    void   refresh( QPaintDevice *qpd );
    bool   takeDamageFromAbove( const QRect &area );
    bool   takeDamageFromBelow( const QRect &area );
    bool   intersects( const QRect &area );

    void   restart( QPaintDevice *qpd );
    void   disable( QWidget *qw );

    int    getY();
    QRect& getEnclosing(){  return enclosing; }
};


#endif

