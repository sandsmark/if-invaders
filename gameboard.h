#ifndef GAMEBOARD_H
#define GAMEBOARD_H


#undef GAMEBOARDINCLUDE
#define GAMEBOARDINCLUDE
#include "allinclude.h"





class GameBoard;
class ShuttleDeadException{};





class GameBoard : public QWidget
{

private:

    // update timers for shots and movements
    QTimer             *bossTimer, *alienTimer, *alienShootTimer, 
	               *shuttleShootTimer, *shuttleTimer;

    const unsigned int alien_rows, alien_columns, max_aliens, alien_speed;
    unsigned int       no_of_aliens, max_alien_width, max_alien_height, 
	               alien_Xindent, alien_Yindent;
  
    const unsigned int bossDelay, alienShootDelay, shuttleShootDelay, 
	               shuttleDelay;
    int                getAlienDelay();

    // initial buffer offset
    const unsigned int offsetX, offsetY;

    // current position of topLeft of alien buffer
    int                bufferX, bufferY;

    enum { LEFT = 1, RIGHT, DOWN_LEFT, DOWN_RIGHT } move_types, shuttle_moves;

    // help vectors for # of aliens per column & per row
    typedef unsigned char uchar;
    typedef std::vector< uchar > :: iterator VUI;
    std::vector< uchar >    column_status, row_status;

    // aliens -> aliens[0]... aliens[max_aliens-1] are pointers to
    // actual aliens out in the field. Must be pointer to be able to
    // handle inheritance
    std::vector< Invader* > aliens;
    typedef std::vector< Invader* > :: iterator VIPI;

    BigBoss            *boss;   
    Shuttle            *ship;    // human

    // buffer for aliens. 
    QPixmap            *buffer, *buffer2;
    // flag for alien buffer choice
    bool               activeBuffer;


    QRect              shuttleShot;
    QPixmap            *shuttleShotPix;
    QPixmap            *alienShotPix1, *alienShotPix2, *currAlienShotPix;

    std::list< QRect >      alienShots;
    typedef std::list< QRect > :: iterator LI;

    std::set< int >         aliveBunch;
    typedef std::set< int > :: iterator SII;
    // pick out the index of the nest shooter.
    int                pickRandom();
  
  
    unsigned int       level;
    unsigned long int  points;
    int                lives;
    const unsigned long int max_lives;
    bool               pauseFlag;


    // protection wall
    Barricade          *shield;

    // initialize all timers; don't start them
    void               initTimers();
    // setup matrix
    void               initAliens();
    // put up the barricade
    void               initShield();
    // setup spiffy things
    void               initDecorations();
    // setup the Bean-a-Bacon rocket. Anyone played Keen4? :)
    void               initShuttle();

    // connect proper signals to proper slots
    void               makeConnections();

    // refresh. Flag is for switching alien buffers (leg-waving :))
    void               redraw( bool all = true );

    // called to remove flickering when aliens warp too fast.
    void               repaintShots();
  
    // give bad guys a chance to shoot
    void               nextShooter();
    
    void               flushAliens();
    void               fillAliens();
    
    // these are called to determine which alien row/col is the first
    // one that has aliens
    int                getFirstActiveRow();
    int                getFirstActiveColumn( int direction );


    // did WE hit anything? Anything at all?
    bool               continueShooting();
    // are we invaded? Called every time aliens move down
    void               aliensLanded();
    bool               hitAliens();
    bool               hitBoss();

    // did some alienShot hit anything?
    bool               alienShotCollision( QRect &area );
    
    // prepare for next level. Arg is for *real* level increment
    void               nextLevel( bool advace = true );

    // S.Meyers - "always provide copy ctor for classes allocating memory"
    GameBoard( const GameBoard& ) ;

    // nor do we allow assign
    GameBoard& operator=( const GameBoard& ) ; 

    Q_OBJECT

public:

    GameBoard( QWidget *p = 0, const char *n = 0 );


    ~GameBoard();
    int                getPoints();

signals:

    // where the hell is the typedef of ulong?
    void               terminateGame( ulong );
    // we lose
    void               gameOver();
    // we killed someone
    void               alienDead( int, int );
    // we win
    void               allDead();
    // we killed the BOSS
    void               bossDead();
    // we hit an alienShot
    void               shotCollision( QRect& );
    // someone dies... this one is internal.
    void               pointsChanged( int );

    // comm about new smth. to someone.
    void               newPoints( int );
    void               newLives( int );
    void               newLevel( int );

private slots:

    // displacement functions
    void               moveBoss();
    void               moveAliens();
    void               moveShot(); 
    void               moveShuttle();
    void               moveShots();
    
    // let's bury them
    void               handleDeadInvader( int xcoord, int ycoord );
    void               handleDeadBoss();
    void               handleShotCollision( QRect &area );

    // kill all timers, start some spiffy animation?
    void               handleLostGame();
    void               handleWonGame();
    // update local vars
    void               handlePointsChanged( int newScore );
  

public slots:

    void               newGame();
    void               setPause();

protected:
  
    // shuttle control
    void               keyPressEvent( QKeyEvent *qke );
    void               keyReleaseEvent( QKeyEvent *qke );
  
    // need it for refresh after partial cover
    void               paintEvent( QPaintEvent *qpe ); 

    // ugly X11 hack to disable continuous keyPressEvents - is this
    // portable?
    void               autoRepeat( int );
    void               focusInEvent( QFocusEvent *qfe );
    void               focusOutEvent( QFocusEvent *qfe );

};


#endif
