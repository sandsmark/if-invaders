#ifndef INVADER_H
#define INVADER_H
 
 
#undef INVADERINCLUDE
#define INVADERINCLUDE
 
#include "allinclude.h"





class Invader;
class SillyInvader;
class CleverInvader;
class FatInvader;
class BigBoss;





/*
 * 1) Aliens are of different size ( espesially width )
 * 2) Aliens have different representation ( pixmaps )
 * 3) The enclosing rectangle around each & every alien is equally wide 
 *    FOR ALL... It simplifies talking to GameBoard
 * 4) All aliens paste their picture centered in the middle of the enclosing
 *    rectangle
 * 5) All aliens can shoot
 */





// base class for all types of invaders
class Invader
{

protected:
    // frame around invader in parent widget's coordinates
    // NB! enclosing.width() == widest alien FOR EVERYBODY
    QRect            enclosing;

    // do we need this flag? Jepp - bullet collision detection
    bool             alive;

public:
    Invader();

    // deleting a subclass through a base class' pointer invokes
    // undefined behaviour if the base class dtor isn't virtual
    virtual ~Invader(){}

    // position invader - topLeft coord.
    void            place( int x, int y, int w, int h );

    // check whether invader has intersection with argument QRect
    virtual bool    contains( const QRect &area );

    // how we draw ourselves. 2 devices for waving hands
    void            paint( QPaintDevice *buf1, QPaintDevice *buf2 );
  
    // maybe it should be protected?
    virtual QPixmap* getPix( int type );

    // is it still viable?
    bool            isAlive()            {  return alive; }
    void            setAlive( bool a )   {  alive = a; }

    // how wide are we, ( on pixmap )
    virtual int     getWidth();
    // how tall are we, ( on pixmap )
    virtual int     getHeight();

    // points
    virtual unsigned int getPoints();  

    int             getX()               {  return enclosing.x(); }
    int             getY()               {  return enclosing.y(); }
    int             getShotX()           {  return enclosing.center().x(); }
    int             getShotY()           {  return enclosing.center().y(); }
    
    QRect&          getEnclose()         {  return enclosing; }
};





// do we leak these statically alloc'ed pix?
class SillyInvader : public Invader
{

    // static, because there is 1 picture for everybody
    static QPixmap  *pix1, *pix2;
  
public:
    SillyInvader();
    ~SillyInvader();

    // static members are best init'ed by static functions
    static void   initPicture( const char *file = SILLYPIX1,
			     const char *file2 = SILLYPIX2 );

    bool          contains( const QRect& );
    
    QPixmap*      getPix( int );
    
    int           getWidth()             {  return pix1->width(); }
    int           getHeight()            {  return pix1->height(); }
    
    unsigned int  getPoints()            {  return 20; }

};





class CleverInvader : public Invader
{
  
    static QPixmap   *pix1, *pix2;

public:
    CleverInvader();
    ~CleverInvader();

    static void    initPicture( const char *file = CLEVERPIX1,
			     const char *file2 = CLEVERPIX2 );

    bool           contains( const QRect& );

    QPixmap*       getPix( int );

    int            getWidth()            {  return pix1->width(); }
    int            getHeight()           {  return pix1->height(); }
  
    unsigned int   getPoints()           {  return 30; }

};





class FatInvader : public Invader
{
  
    static QPixmap *pix1, *pix2;

public:
    FatInvader();
    ~FatInvader();

    static void   initPicture( const char *file = FATPIX1,
			       const char *file2 = FATPIX2 );

    bool          contains( const QRect& );
    
    QPixmap*      getPix( int );

    int           getWidth()             {  return pix1->width(); }
    int           getHeight()            {  return pix1->height(); }
  
    unsigned int  getPoints()            {  return 10; }

};





// that fat thing flying hyperfast above the invaders array
class BigBoss : public Invader  
{

    QPixmap       *pix;
    int           speed;
    int           height;
public:
    BigBoss( int );
    ~BigBoss();

    void          initPicture( const char *file = BIGBOSSPIX );
    
    bool          contains( const QRect& );

    bool          move( QPaintDevice*, int );

    int           getWidth()            {  return pix->width(); }
    int           getHeight()           {  return pix->height(); }
    
    void          reset();
    unsigned int  getPoints()           {  return 500; }

};


#endif
