#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#undef GAMEWINDOWINCLUDE
#define GAMEWINDOWINCLUDE

#include "allinclude.h"





class GameWindow;
class Intro;




// I ought to implement copy ctors for all the classes.
class GameWindow : public QWidget
{

    GameBoard          *gbrd;
  
    QPushButton        *newgame_bt, *pause_bt, *quit_bt;
    QLCDNumber         *score, *lives, *level, *best;

    Intro              *init_welcome;


    Q_OBJECT

    void               setUpButtons();
    void               makeConnections();

    // vert. space between the widgets
    const int          vspace;

public:

    GameWindow( uid_t player, QWidget *p = 0, const char *name = 0 );
    ~GameWindow();

public slots:
    void               die();


signals:
    void               newGame();
    void               pause();

protected:
    void               paintEvent( QPaintEvent* );
    void               keyPressedEvent( QKeyEvent* );

};





class Intro : public QWidget, public HallOfFame
{

    QListBox           *scoreLst;
    QLabel             *title;
    QLabel             *ack;

    void               initWidgets();

    uid_t              player;

    Q_OBJECT

public:
    Intro( uid_t pl, QWidget *p = 0, const char *name = 0 );
    ~Intro();
    void               readHiscore();
    void               displayScores();

public slots:
    void               updateScore( ulong lastScore ); 

signals:
    void               introDone();
    void               bestScore( int );

protected:
    void               paintEvent( QPaintEvent* );
    void               keyPressEvent( QKeyEvent* );
};





class HiscoreEntry : public QListBoxItem
{
    QColor             internalColor;

public:

    HiscoreEntry( const QColor &c, const char *s );

protected:
    void               paint( QPainter* );
    int                height( const QListBox* ) const;
    int                width( const QListBox* ) const;
    
};





#endif
