#include "gameboard.h"





// OBS! shuttleShot speed is related to alienShotSpeed - the ratio
// must not be too hi - otherwise shuttleShot/alienShots will "jump
// over" one another. 
GameBoard :: GameBoard( QWidget *p, const char *n )
          :  QWidget( p, n ), alien_rows( 5 ), alien_columns( 18 ), 
	     max_aliens( alien_rows*alien_columns ),
	     alien_speed( 2 ), bossDelay( 30 ), alienShootDelay( 50 ),
	     shuttleShootDelay( 10 ), shuttleDelay( 25 ),
	     offsetX( 40 ), offsetY( 40 ), max_lives( 3 )
{

    setFixedSize( 640, 400 );

    setFocusPolicy( StrongFocus );

    bossTimer = alienTimer = alienShootTimer = shuttleShootTimer 
	= shuttleTimer = 0;

    column_status = std::vector< uchar >( alien_columns, 0 );
    row_status = std::vector< uchar >( alien_rows, 0 );
    pauseFlag = false;
    points = 0U;

    initDecorations();

    initTimers();

    initAliens();

    initShuttle();
  
    initShield();

    makeConnections();
}


GameBoard :: ~GameBoard()
{

    // enable back 'autorepeat' on exit
    autoRepeat( 1 );

    bossTimer->stop();
    alienTimer->stop();
    alienShootTimer->stop();
    shuttleShootTimer->stop();
    shuttleTimer->stop();

    delete bossTimer;
    delete alienTimer;
    delete alienShootTimer;
    delete shuttleShootTimer;
    delete shuttleTimer;

    flushAliens();     // dump all the alien objects

    alienShots.clear();

    delete boss;
    delete ship;
    delete buffer;
    delete buffer2;
}
  
  
void
GameBoard :: initDecorations()
{
  
    setBackgroundColor( QColor( black ) );
    shuttleShotPix = new QPixmap( SHUTTLESHOTPIX );
    alienShotPix1 = new QPixmap( ALIENSHOT1 );
    alienShotPix2 = new QPixmap( ALIENSHOT2 );
    currAlienShotPix = alienShotPix1;

    if ( alienShotPix1->isNull() || alienShotPix2->isNull() ) 
	fatal( "failed to init shots" );
}


void 
GameBoard :: initShuttle()
{

    ship = new Shuttle( alien_speed, width()/2, height() - 20 );

    if ( shuttleShotPix->isNull() ) 
	fatal ( "failed to init shots" );

    shuttleShot = shuttleShotPix->rect();
}


void
GameBoard :: initShield()
{

    shield = new Barricade( this, ship->getY() - 40, width() );
    shield->refresh( this );
}


// aliens are init'ed only ONCE - however, their status is reset to
// 'alive' every gameround
void
GameBoard :: initAliens()
{

    // ok, lets resize right now, malloc() gives a hi penalty
    aliens = std::vector< Invader* >( max_aliens, 0 );
    
    boss = 0;

    // Now, alien size is known
    SillyInvader :: initPicture();
    CleverInvader :: initPicture();
    FatInvader :: initPicture();

    
    // These loops won't look pretty with iterators...
    for ( unsigned int i = 0; i < alien_columns*2; ++i )
	aliens[i] = new CleverInvader();

    for ( unsigned int i = alien_columns*2; i < 3*alien_columns; ++i )
	aliens[i] = new SillyInvader();

    for ( unsigned int i = 3*alien_columns; i < max_aliens; ++i )
	aliens[i] = new FatInvader();

    boss = new BigBoss( 10 );

    // Code below must be run only once!
    // these values must of course be calculated, unless there is a
    // convention as to which aliens are the biggest
    max_alien_width = aliens[max_aliens-1]->getWidth() + alien_speed;
    max_alien_height = aliens[max_aliens-1]->getHeight() + 4*alien_speed;

    // now, what the hell was this for?
    alien_Xindent = max_alien_width;
    alien_Yindent = max_alien_height;

    buffer = new QPixmap( alien_Xindent*alien_columns, 
			  alien_Yindent*alien_rows );
    buffer2 = new QPixmap( alien_Xindent*alien_columns, 
			   alien_Yindent*alien_rows );
}


void
GameBoard :: flushAliens()
{

    for ( VIPI tmp = aliens.begin(); tmp != aliens.end(); ++tmp ) {
	delete *tmp;
	*tmp = 0;
    }

    // index of alive aliens.
    aliveBunch.clear();

    delete boss;
    boss = 0;
}


// this one 'restarts' aliens
void
GameBoard :: fillAliens()
{
  
    buffer->fill( this, bufferX, bufferY );
    buffer2->fill( this, bufferX, bufferY );

    // we can affoard spending little xtra time @ the beginning of every
    // round, therefore, the multiplication in index is not all too
    // expensive
    for ( unsigned int i = 0; i < alien_rows; ++i ) 
	for ( unsigned int j = 0; j < alien_columns; ++j ) {
	    aliens[ i*alien_columns+j ]->place( j*alien_Xindent, 
						i*alien_Yindent,
						max_alien_width, 
						max_alien_height );
	    aliens[ i*alien_columns+j ]->setAlive( true );
	    aliens[ i*alien_columns+j ]->paint( buffer, buffer2 );

	    aliveBunch.insert( i*alien_columns+j );
	}

    for ( VUI i = column_status.begin(); i != column_status.end(); ++i )
	*i = alien_rows;

    for ( VUI i = row_status.begin(); i != row_status.end(); ++i )
	*i = alien_columns;

    move_types = ( rand() % 2 ) ? RIGHT : LEFT;
}
    

void 
GameBoard :: initTimers()
{

    // on timeOut() boss moves
    bossTimer = new QTimer( this );
    // on timeOut() all aliens ( except boss ) move
    alienTimer = new QTimer( this );
    // on timeOut() alien shots move
    alienShootTimer = new QTimer( this );
    // on timeOut() shuttle shot moves
    shuttleShootTimer = new QTimer( this );
    // on timeOut() shuttle moves
    shuttleTimer = new QTimer( this );
}


// don't ask - you don't want to know. This function tries to reward
// killing aliens very early at the same time as it makes it more
// difficult with fewer aliens.
int
GameBoard :: getAlienDelay()
{

    return std::max( (int)( 0.8*shuttleDelay ), 
		     (int)( height() - bufferY - level*level - 
			    (int)pow((5.0/((float)no_of_aliens))*20, 2) ) ); 
}


void
GameBoard :: makeConnections()
{

    connect( bossTimer, SIGNAL( timeout() ), SLOT( moveBoss() ) );
    connect( alienTimer, SIGNAL( timeout() ), SLOT( moveAliens() ) );
    connect( shuttleShootTimer, SIGNAL( timeout() ), SLOT( moveShot() ) );
    connect( shuttleTimer, SIGNAL( timeout() ), SLOT( moveShuttle() ) );
    connect( alienShootTimer, SIGNAL( timeout() ), SLOT( moveShots() ) );

    connect( this, SIGNAL( bossDead() ), SLOT( handleDeadBoss() ) );
    connect( this, SIGNAL( alienDead(int, int) ), 
	     SLOT( handleDeadInvader(int, int) ) );
    connect( this, SIGNAL( gameOver() ), SLOT( handleLostGame() ) );
    connect( this, SIGNAL( allDead() ), SLOT( handleWonGame() ) );
    connect( this, SIGNAL( shotCollision( QRect& ) ), 
	     SLOT( handleShotCollision( QRect& ) ) );
    connect( this, SIGNAL( pointsChanged(int) ), 
	     SLOT( handlePointsChanged(int) ) );
}


// called every time an event resulting in point change occurs.
void
GameBoard :: handlePointsChanged( int ptChange )
{
    static unsigned int next_life_up = 5000;

    points += ptChange*level;

    if ( points > next_life_up ) {
	emit newLives( ++lives );

	// it gets increasingly more difficult to get spare lives.
	next_life_up += next_life_up;
    }
      
    // comm to other widgets
    emit newPoints( points );  
}


int
GameBoard :: getFirstActiveRow()
{

    for ( int i = alien_rows-1; i >= 0; --i )
	if ( row_status[i] > 0 )
	    return i;

    // Return any int negative value < 0.
    warning( "illegal return value(row)" );
    return -1;
}


int
GameBoard :: getFirstActiveColumn( int direct )
{

    switch ( direct ) {
    case LEFT:
	for ( unsigned int i = 0; i < alien_columns; ++i )
	    if ( column_status[i] > 0 ) 
		return i;
    case RIGHT:
	for ( int i = alien_columns-1; i >= 0; --i )
	    if ( column_status[i] > 0 ) 
		return i;
    }

    // should NEVER happen!
    warning( "illegal return value(col)" );
    return -1;
}


// aliens get a timer reset on move_down. Only this func knows by how
// much they move down.
void
GameBoard :: moveAliens()
{
    static unsigned int drop_count = 0;

    switch ( move_types ) {
    case LEFT:
	if ( bufferX + aliens[ getFirstActiveColumn( LEFT ) ]->getX() <= 
	     static_cast< int >( alien_speed ) ) 
	    move_types = DOWN_LEFT;

	bufferX -= alien_speed;
	break;
    case RIGHT:
	if ( bufferX + aliens[ getFirstActiveColumn( RIGHT ) ]->getX() >= 
	     width() - static_cast< int >( alien_Xindent + alien_speed ) )
	    move_types = DOWN_RIGHT;

	bufferX += alien_speed;
	break;
    case DOWN_LEFT:
	if ( drop_count++ < max_alien_height/2 ) 
	    bufferY += alien_speed;
	else {
	    alienTimer->changeInterval( getAlienDelay() );
	    drop_count = 0;
	    move_types = RIGHT;
	    aliensLanded();
	}
	break;
    case DOWN_RIGHT:
	if ( drop_count++ < max_alien_height/2 ) 
	    bufferY += alien_speed;
	else {
	    alienTimer->changeInterval( getAlienDelay() );
	    drop_count = 0;
	    move_types = LEFT;
	    aliensLanded();
	}
    }

    redraw();
}


void
GameBoard :: redraw( bool all )
{

    // When someone is killed, want to update without flip. 
    if ( all ) 
	activeBuffer = !activeBuffer;

    bitBlt( this, bufferX, bufferY, (( activeBuffer ) ? buffer : buffer2 ) );

    ship->redraw( this );

    nextShooter();
    repaintShots();

    // FIXME: Must find a better way of doing this. Then again, since
    // few aliens move very fast, player won't have time to shoot boss.
    if ( !bossTimer->isActive() && ( (abs(rand()) % 71) == 0 ) )
	bossTimer->start( bossDelay );
}


int
GameBoard :: pickRandom()
{
    int times = (int)((double)rand() / ((double)RAND_MAX + 1) * aliveBunch.size() );
    SII tmp = aliveBunch.begin(); 

    for ( int i = 1; i < times; ++i, ++tmp )
	;

    return *tmp;
}


void
GameBoard :: nextShooter()
{
    static const int probability = 10;
    static const int maxShots = 15;

    if ( (int)((double)rand() / ((double)RAND_MAX + 1) * probability )
	 || alienShots.size() > static_cast<unsigned int>( maxShots ) ) 
	return ;

    int index = pickRandom();
    alienShots.push_back(  QRect( aliens[index]->getShotX()+bufferX, 
				  aliens[index]->getShotY()+bufferY,
				  alienShotPix1->width(), 
				  alienShotPix1->height() ) );

    // First time thru
    if ( !alienShootTimer->isActive() ) 
	alienShootTimer->start( alienShootDelay );
}


void
GameBoard :: repaintShots()
{

    for ( LI iter = alienShots.begin(); iter != alienShots.end(); ++iter )
	bitBlt( this, (*iter).topLeft(), currAlienShotPix );
}


// Are excepts performance bottleneck? Why the hell are SGIs so SLOW?
// The story with excepts is that whenever we detect that a shuttle
// has been hit by an alien shot, a signal is emmitted indicating
// it. Upon this emition, newGame is called which in turn flushes
// alienShots.  Since alienShotCollision() detects this event, there
// must be a way to communicate back the required
// information... Throwing an except is about the easiest.
void 
GameBoard :: moveShots()
{
    LI temp;

    for ( LI iter = alienShots.begin(); iter != alienShots.end(); ) {
	(*iter).moveBy( 0, alien_speed );
	bitBlt( this, (*iter).topLeft(), currAlienShotPix );

	try {
	    if ( alienShotCollision( *iter ) ) {
		temp = iter;
		++iter;
		alienShots.erase( temp );
	    }
	    else 
		++iter;
	}
	catch ( ShuttleDeadException &s ) {
	    emit gameOver();
	    return ;
	}
    }

    currAlienShotPix = ( currAlienShotPix == alienShotPix1 ) 
	             ? alienShotPix2 
                     : alienShotPix1;
}


bool
GameBoard :: alienShotCollision( QRect &ashot )
{

    // shot's outside the board
    if ( !rect().intersects( ashot ) )
	return true;

    // both shots are wiped by signal's SLOT/handler
    if ( ashot.intersects( shuttleShot ) ) {
	emit shotCollision( ashot );  
	return true;
    }

    // player's dead
    if ( ashot.intersects( ship->getPosition() ) )
	throw ShuttleDeadException();

    // shield updates the barricade segments itself
    if ( shield->intersects( ashot ) && 
	 shield->takeDamageFromAbove( ashot ) ) {
	erase( ashot );
	return true;
    }

    return false;
}


// NB! moveShots() removes ashot from shots list
void
GameBoard :: handleShotCollision( QRect &ashot )
{

    erase( ashot );
    erase( shuttleShot );
    emit pointsChanged( 5 );
    shuttleShootTimer->stop();  // stop shots  
}


void
GameBoard :: aliensLanded()
{
    // +1 because vector is indexed @ 0
    int level = getFirstActiveRow()+1;

    // we lose
    if ( bufferY + level*(static_cast<int>(alien_Yindent) ) 
	 >= ship->getY() ) {

	emit gameOver();
	return ;
    }

    // shield - out.
    if ( shield->getY() <= 
	 bufferY + level*(static_cast<int>(alien_Yindent) ) ) {
	shield->disable( this );
	erase( shield->getEnclosing() );
    }
}
  

void
GameBoard :: moveShuttle()
{

    ship->move( this, 0, width(), static_cast<int>(shuttle_moves) );
}


void 
GameBoard :: moveBoss()
{

    // are we too far to the right?
    if ( !boss->move( this, width() ) ) 
	bossTimer->stop();
}


// what's the story with the magic -3? Should be read in some way from
// the pic.
void 
GameBoard :: moveShot()
{
  
    erase( shuttleShot );

    if ( !continueShooting() ) 
	shuttleShootTimer->stop();
    else {
	shuttleShot.moveBy( 0, -3 );
	bitBlt( this, shuttleShot.topLeft(), shuttleShotPix );
    }
}


bool
GameBoard :: continueShooting()
{

    return rect().intersects( shuttleShot ) && !hitAliens() && 
	   !hitBoss() && !shield->takeDamageFromBelow( shuttleShot );
}


bool
GameBoard :: hitBoss()
{
  
    if ( bossTimer->isActive() && boss->contains( shuttleShot ) ) {
	emit bossDead();
	return true;
    }

    return false;
}
    
    
void
GameBoard :: handleDeadBoss()
{

    bossTimer->stop();
    erase( boss->getEnclose() );   // wipe the sucker
    boss->reset();                 // move to the init position
    emit pointsChanged( boss->getPoints() );
}


// WARNING! Very obscure code!
bool
GameBoard :: hitAliens()
{

    //         compensate for buffer offset        find index
    int indexX = (shuttleShot.x() - bufferX) / alien_Xindent;
    int indexY = (shuttleShot.y() - bufferY) / alien_Yindent;

    if ( 0 <= indexX && indexX <= static_cast< int >( alien_columns -1 ) && 
	 0 <= indexY && indexY <= static_cast< int >( alien_rows -1 ) ) {
	if ( aliens[ indexY*alien_columns + indexX ]->isAlive() ) {
	    // now we have a candidate for a kill. Compensate for buffer
	    // offset - aliens know where they are within BUFFER, not BOARD
	    shuttleShot.moveBy( -bufferX, -bufferY );
	    if ( aliens[ indexY*alien_columns + indexX ]->contains(shuttleShot) ) {
		// damn, easy to forget those!
		emit alienDead( indexX, indexY );
		return true;
	    }
	    else 
		shuttleShot.moveBy( bufferX, bufferY ); // restore coords
	}
    }

  return false;
}


void
GameBoard :: handleDeadInvader( int indexX, int indexY )
{
    Invader *target = aliens[ indexY*alien_columns + indexX ];

    // u r dead baby
    target->setAlive( false );
    --column_status[indexX];
    --row_status[indexY];

    // DO NOT CHANGE THE PLACE OF THIS FUNCTION CALL. getFirst...()
    // depends on column/row_status. Their update must happen *before*
    // we ask for new data.
    int currCol = getFirstActiveRow()+1;

    // shrink buffer to eliminate flickering. Do we leak here?
    if ( currCol > 0 && buffer->height() > (int)(currCol*max_alien_height) ) {
	erase( bufferX, bufferY, buffer->width(), buffer->height() );
	buffer->resize( buffer->width(), currCol*max_alien_height );
	buffer2->resize( buffer2->width(), currCol*max_alien_height ); 
    }

    emit pointsChanged( target->getPoints() );

    aliveBunch.erase( aliveBunch.find( indexY*alien_columns + indexX ) );
    
    if ( !(--no_of_aliens) ) {
	buffer->resize( buffer->width(), max_alien_height*alien_rows );
	buffer2->resize( buffer2->width(), max_alien_height*alien_rows );
	emit allDead();
    }
    else {
	alienTimer->changeInterval( getAlienDelay() );
	QPainter p( buffer ), p2( buffer2 );
	// wipe from buffers... Can we optimize this? eraseRect()
	p.fillRect( target->getEnclose(), QBrush( backgroundColor() ) );
	p2.fillRect( target->getEnclose(), QBrush( backgroundColor() ) );
	redraw( false );
    }
}
  

// FIXME: perhaps termination is rather brutal...
void
GameBoard :: handleLostGame()
{

    if ( alienTimer->isActive() ) alienTimer->stop();
    if ( shuttleTimer->isActive() ) shuttleTimer->stop();
    if ( alienShootTimer->isActive() ) alienShootTimer->stop();
    if ( shuttleShootTimer->isActive() ) shuttleShootTimer->stop();
    if ( bossTimer->isActive() ) bossTimer->stop();
    
    //  setEnabled( false );

    // FIXME: we had a SIGBUS around here. Last msg was 'Lost game'
    if ( --lives >= 0 )  {
	emit newLives( lives );
	nextLevel( false );
    }
    else  {
	setPause();
	hide();
	emit terminateGame( points );
    }
}


void
GameBoard :: handleWonGame()
{

    alienTimer->stop();
    shuttleTimer->stop();
    alienShootTimer->stop();
    shuttleShootTimer->stop();
    bossTimer->stop();
    //  setEnabled( false );  // ?? what happens here
  
    erase();

    nextLevel();
}


void 
GameBoard :: newGame()
{

    // one of these is redundant
    erase();
    raise();
    show();

    pauseFlag = false;
    no_of_aliens = 0;
    level = 0;
    points = 0;
    lives = max_lives;

    emit pointsChanged( points );
    emit newLives( lives );
    nextLevel();
}


void
GameBoard :: nextLevel( bool advance )
{
    static const int vert_alien_offset = 20;

    erase();

    setEnabled( true );
    setFocus();    

    if ( advance ) 
	emit newLevel( ++level );

    activeBuffer = true;
    bufferX = offsetX;
    bufferY = offsetY + vert_alien_offset*level;


    // There is no guarantee about buffer size due to my opt. of code
    if ( (unsigned int)buffer->height() < alien_columns*max_alien_height ) {
	buffer->resize( buffer->width(), max_alien_height*alien_rows );
	buffer2->resize( buffer2->width(), max_alien_height*alien_rows );
    }
    
    // cannot be called before buffer offset is correct
    fillAliens();  
    boss->reset();
    shield->restart( this );

    no_of_aliens = max_aliens;

    alienShots.clear();
    // at level 10 we start way too lo.
    aliensLanded();
  
    alienTimer->start( getAlienDelay() );
}


void
GameBoard :: keyPressEvent( QKeyEvent *qke )
{

    switch ( qke->key() ) {
    case Key_Left:   
	if ( pauseFlag ) 
	    break ;
	shuttleTimer->stop();
	shuttle_moves = LEFT;
	shuttleTimer->start( shuttleDelay );
	break ;

    case Key_Right:  
	if ( pauseFlag ) 
	    break ;
	shuttleTimer->stop();
	shuttle_moves = RIGHT;
	shuttleTimer->start( shuttleDelay );
	break ;

    case Key_Space:
	if ( pauseFlag ) 
	    break ;
	if ( !shuttleShootTimer->isActive() ) {
	    shuttleShot.moveTopLeft( QPoint( 
		        ship->getBulletX(), 
			ship->getBulletY()-shuttleShot.height() ) );
	    shuttleShootTimer->start( shuttleShootDelay );
	}
	break ;

    case Key_Q:
	setPause();
	hide();
	emit terminateGame( points );
	break ;

    case Key_P:
	setPause();
	break ;

    // These are debug opts. remove this code later.
    case Key_D:
	if ( qke->state() == ShiftButton ) {
	    debug( "buffer size (w, h) == (%d, %d)", buffer->width(), buffer->height() );
	    debug( "controlled height == (%d", (getFirstActiveRow()+1)*max_alien_height );
	}
	break ;

    case Key_L:
	if ( qke->state() == ShiftButton )
	    nextLevel();
	break ;
    }
}


void
GameBoard :: keyReleaseEvent( QKeyEvent *qke )
{

    switch ( qke->key() ) {
    case Key_Left:   // FALLTHRU
    case Key_Right:
	if ( shuttleTimer->isActive() ) 
	    shuttleTimer->stop();
	break ;
    }
}


void
GameBoard :: setPause()
{
    static bool bTimer = false, aTimer = false, aSTimer = false, 
	        sSTimer = false;

    pauseFlag = !pauseFlag;
    
    if ( pauseFlag ) {
	bTimer = bossTimer->isActive();  bossTimer->stop();
	aTimer = alienTimer->isActive();  alienTimer->stop();
	aSTimer = alienShootTimer->isActive();  alienShootTimer->stop();
	sSTimer = shuttleShootTimer->isActive();  shuttleShootTimer->stop();
	shuttleTimer->stop();
	return ;
    }
  
    if ( bTimer ) bossTimer->start( bossDelay );
    if ( aTimer ) alienTimer->start( getAlienDelay() );
    if ( aSTimer ) alienShootTimer->start( alienShootDelay );
    if ( sSTimer ) shuttleShootTimer->start( shuttleShootDelay );
}


// courtesy of Andrew Richards
// Work around continuous keyPressEvents
void
GameBoard :: autoRepeat( int state )
{
    if ( state < 0 || state > 1 )
	fatal( "autoRepeat: Illegal autorepeat setting" );
 
    XKeyboardControl values;
    values.auto_repeat_mode = state;
    values.key = -1;         // ALL keys
    XChangeKeyboardControl( qt_xdisplay(), KBAutoRepeatMode, &values );
}


void
GameBoard :: focusInEvent( QFocusEvent* )
{

    autoRepeat( 0 );
    
    if ( pauseFlag ) 
	setPause(); 
}


void
GameBoard :: focusOutEvent( QFocusEvent* )
{

//     autoRepeat( 1 );
//     QPoint p = mapFromGlobal( QCursor::pos() );
    
//     if ( !parentWidget()->rect().contains( p ) ) {
// 	if ( !pauseFlag ) 
// 	    setPause();
//     }
}


void
GameBoard :: paintEvent( QPaintEvent *qpe )
{

    if ( shield->intersects( qpe->rect() ) )
	shield->refresh( this );
}


int
GameBoard :: getPoints()
{

    return points;
}
