#ifndef SHUTTLE_H
#define SHUTTLE_H


#undef SHUTTLEINCLUDE
#define SHUTTLEINCLUDE

#include "allinclude.h"





// As opposed to aliens, this one operates on true, GameBoard's
// coords. Hmm do we leak here?
class Shuttle
{

    QRect    enclosing;
    QPixmap  *picture;

    // shuttle speed ( sidewise )
    int      speed;

public:
  
    Shuttle( int sp, int x, int y, const char *file = SHUTTLEPIX );
    ~Shuttle(){}

    void move( QPaintDevice*, int lo, int hi, int dir );
    void redraw( QPaintDevice* );

    // we need to tell where we are
    QRect& getPosition() {  return enclosing;  }
    int getY()           {  return enclosing.y(); } 
    int getBulletX();
    int getBulletY();
};


#endif
