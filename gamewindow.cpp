#include "gamewindow.h"





GameWindow :: GameWindow( uid_t player, QWidget *p, const char *name ) 
           :  QWidget( p, name ), vspace( 20 )
{
  
    gbrd = new GameBoard( this );
    gbrd->lower();
    init_welcome = new Intro( player, this );
    init_welcome->setEnabled( true );
    init_welcome->setFocus();
    init_welcome->show(); 

    // mouse ptr gets in the way.
    gbrd->setCursor( QCursor( blankCursor ) );

    setFixedSize( gbrd->width() + 100, gbrd->height() );
    
    setUpButtons();
    makeConnections();

    // this one comes AFTER the connections/init, or we'll get SIGSEGV
    init_welcome->readHiscore();
}


GameWindow :: ~GameWindow()
{

    init_welcome->updateScore( gbrd->getPoints() );
    
    delete gbrd;
    delete init_welcome;
    delete newgame_bt;
    delete pause_bt;
    delete quit_bt;
    delete score;
    delete lives;
    delete level;
    delete best;
}


void
GameWindow :: setUpButtons()
{
  
    newgame_bt = new QPushButton( "new", this );
    QToolTip :: add( newgame_bt, "New game" );
    
    pause_bt = new QPushButton( "freeze", this );
    quit_bt = new QPushButton( "die", this );
    QToolTip :: add( quit_bt, "Quit game" );
    
    score = new QLCDNumber( 6, this, 0 );
    QToolTip :: add( score, "Current score" );

    lives = new QLCDNumber( 3, this, 0 );
    QToolTip :: add( lives, "Lives remaining" );
    
    level = new QLCDNumber( 3, this, 0 );
    QToolTip :: add( level, "Current level" );
    
    best  = new QLCDNumber( 6, this, 0 );
    QToolTip :: add( best, "Hiscore" );
    
    newgame_bt->setGeometry( gbrd->width() + 20, gbrd->y(),
			     50, 30 );
    pause_bt->setGeometry( newgame_bt->x(), 
			   newgame_bt->y() + newgame_bt->height() + vspace,
			   50, 30 );
    quit_bt->setGeometry( pause_bt->x(), 
			  pause_bt->y() + pause_bt->height() + vspace,
			  50, 30 );
    

    best->setGeometry( quit_bt->x(), quit_bt->y() + quit_bt->height() + vspace,
		       70, 30 );
    best->setLineWidth( 1 );
    best->setMidLineWidth( 2 );
    best->setFrameStyle( QFrame::Box | QFrame::Raised );
    best->setBackgroundColor( QColor( "#000000" ) );
    
    score->setGeometry( best->x(), best->y() + best->height() + vspace,
			70, 30 );
    score->setLineWidth( 1 );
    score->setMidLineWidth( 2 );
    score->setFrameStyle( QFrame::Box | QFrame::Raised );
    score->setBackgroundColor( QColor( "#000000" ) );
    
    lives->setGeometry( score->x(), score->y() + score->height() + vspace,
			36, 30 );
    lives->setLineWidth( 1 );
    lives->setMidLineWidth( 2 );
    lives->setFrameStyle( QFrame::Box | QFrame::Raised );
    lives->setBackgroundColor( QColor( "#000000" ) );
		      
    level->setGeometry( lives->x(), lives->y() + lives->height() + vspace,
			36, 30 );
    level->setLineWidth( 1 );
    level->setMidLineWidth( 2 );
    level->setFrameStyle( QFrame::Box | QFrame::Raised );
    level->setBackgroundColor( QColor( "#000000" ) );
		      

    QPalette qp = score->palette();
    // boy, this is really fucked!
    QColorGroup qcg = QColorGroup( QColor( "yellow" ),  // foreground
				   QColor( "#000000" ), // background
				   QColor( "LightYellow" ),
				   QColor( "yellow4" ),
				   QColor( "yellow2" ),
				   QColor( "yellow3" ), // text
				   QColor( "grey20" ));  
    qp.setNormal( qcg );
    score->setPalette( qp );
    lives->setPalette( qp );
    level->setPalette( qp );

    newgame_bt->show();
    pause_bt->show();
    quit_bt->show();
    score->display( 0 );

}
  

void 
GameWindow :: makeConnections()
{

    connect( newgame_bt, SIGNAL( clicked() ), gbrd, SLOT( newGame() ) );
    connect( init_welcome, SIGNAL( introDone() ), gbrd, SLOT( newGame() ) );
    connect( quit_bt, SIGNAL( clicked() ), this, SLOT( die() ) );
    connect( pause_bt, SIGNAL( clicked() ), gbrd, SLOT( setPause() ) );
    connect( gbrd, SIGNAL( newPoints( int ) ), 
	     score, SLOT( display( int ) ) );
    connect( gbrd, SIGNAL( newLives( int ) ),
	     lives, SLOT( display( int ) ) );
    connect( gbrd, SIGNAL( newLevel( int ) ), 
	     level, SLOT( display( int ) ) );
    connect( gbrd, SIGNAL( terminateGame( ulong ) ), 
	     init_welcome, SLOT( updateScore( ulong ) ) );
    connect( this, SIGNAL( pause() ), gbrd, SLOT( setPause() ) );
    connect( init_welcome, SIGNAL( bestScore( int ) ),
	     best, SLOT( display( int ) ) );
}


void
GameWindow :: paintEvent( QPaintEvent* )
{

    // EMPTY... why?
}


void
GameWindow :: keyPressedEvent( QKeyEvent *qke )
{

    switch ( qke->key() ) {
    case Key_N:
	init_welcome->hide();
	gbrd->show();
	break ;
    case Key_Q:
	if ( qke->state() == AltButton )
	    close();
	break ;
    case Key_P:
	if ( qke->state() == AltButton ) 
	    emit pause();
	break ;
    }
}


void
GameWindow :: die()
{

    close( true );
}





Intro :: Intro( uid_t play, QWidget *p, const char *name )
      :  QWidget( p, name ), HallOfFame(), player( play )
{

    if ( p ) 
	setGeometry( 0, 0, p->width(), p->height() );
    else 
	setGeometry( 0, 0, 640, 480 );

    setBackgroundColor( QColor( "#3366CC" ) );
    scoreLst = new QListBox( this, 0, WStyle_NoBorder );
    title = new QLabel( this );
    ack = new QLabel( this );
    
    setEnabled( true );
    setFocusPolicy( StrongFocus );
    initWidgets();
}
  

Intro :: ~Intro()
{

    delete scoreLst;
    delete title;
    delete ack;
}


void
Intro :: initWidgets()
{

    // we don't want any input
    //  scoreLst->setEnabled( false );
    scoreLst->setFocusPolicy( NoFocus );
    scoreLst->setScrollBar( true );
    scoreLst->resize( 200, 200 );
    
    
    title->setGeometry( scoreLst->width() + scoreLst->x() + 20,
			scoreLst->y(),
			100, 60 );
    title->setBackgroundColor( backgroundColor() );
    title->setAutoResize( true );
    title->setFont( QFont( "helvetica", 36, QFont::Bold, false ) );
    title->setText( "IfiInvaders" );
    
    QPalette qp = title->palette();
    // boy, this is really fucked! #DD2222
    QColorGroup qcg = QColorGroup( QColor( "#999999" ),  // foreground
				   QColor( "#000000" ),  // background
				   QColor( "#222222" ),
				   QColor( "#111111" ),
				   QColor( "#333333" ),
				   QColor( "grey10" ),   // text
				   QColor( "grey30" ));  
    qp.setNormal( qcg );
    title->setPalette( qp );
    
    
    ack->setGeometry( 0, height() - ack->font().pointSize()*4, 100, 40 );
    ack->setBackgroundColor( backgroundColor() );
    ack->setAutoResize( true );
    ack->setPalette( qp );
  
    ack->setText( "© Copyright by igorR\n"
		  "Thanx alot to: Knut Erik Borgen (lazy advisor), "
		  "Andrew Richards (X11 hacks) and Jonny Goldman (pix)" );
}


void
Intro :: readHiscore()
{

    readFromFile();
    displayScores();

    if ( scores.size() )
	emit bestScore( scores[0]->getScore() );
    else
	emit bestScore( 0 );
}
  

void
Intro :: paintEvent( QPaintEvent *qpe )
{
    
    if ( qpe->rect().intersects( scoreLst->rect() ) )
	scoreLst->repaint();
}


void
Intro :: keyPressEvent( QKeyEvent *qke )
{
    
    qke->ignore();
    lower();
    emit introDone();
}
  

void
Intro :: updateScore( ulong lastScore )
{

    updateFile( player, lastScore );
    
    scoreLst->clear();
    displayScores();
    emit bestScore( scores[0]->getScore() );
}


// fuck - egcs-2.91.60 hasn't implemented sstream yet. At least not in
// any standard header. ARGH!  btw, there is no leak assoc'ed with
// 'new' here - QListBox assumes ownership over QListBoxItems inserted
// into it, and it will delete them, as soon as they are not needed
void
Intro :: displayScores()
{
    char buffer[60];
  
    scoreLst->setAutoUpdate( false );
    for ( unsigned int i = 0; i < scores.size(); ++i ) {
	sprintf( buffer, "%d. %-9.9s : %d", i+1, scores[i]->getName(),
		 (int)scores[i]->getScore() );
	if ( scores[i]->getUid() == player )
	    scoreLst->insertItem( new HiscoreEntry( QColor( "blue" ), buffer ) );
	else
	    scoreLst->insertItem( new HiscoreEntry( QColor( "black" ), buffer ) );
    }
    scoreLst->setAutoUpdate( true );
}





// a wrapper to do fancy stuff with hiscore entries.
HiscoreEntry :: HiscoreEntry( const QColor &c, const char *s )
             :  QListBoxItem(), internalColor( c )
{

    setText( s );
}


void
HiscoreEntry :: paint( QPainter *p )
{
    QPen oldP = p->pen(); 
    
    p->setPen( QPen( internalColor ) );
    // damn, this is ugly.
    p->drawText( 0, 10, text() );
    p->setPen( oldP );
}


int 
HiscoreEntry :: height( const QListBox *qlb ) const 
{

    return qlb->fontMetrics().lineSpacing() + 1;
}


int
HiscoreEntry :: width( const QListBox *qlb ) const
{
    
    return qlb->fontMetrics().width( text() ) + 6;
}




