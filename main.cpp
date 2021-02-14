#include "stl_score.h"
#include "gamewindow.h"
 

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
 
 



static uid_t uidAuthor, uidUser;


void 
startOwner()
{
 
    if ( seteuid( uidAuthor ) < 0 ) 
	fatal( "failed privileged action" );
}


void 
startUser()
{
 
    if ( seteuid( uidUser ) < 0 )
	fatal( "failed privileged action" );
}


int 
main( int argc, char *argv[] )
{

    if ( getpwuid( getuid() ) == 0 ) 
	fatal( "You don't exist here - go away" );
  
    uidAuthor = geteuid();
    uidUser   = getuid();
    // open connection to display
    startUser();

    // courtesy of Dag Langmyhr
    srand( time(0) ^ getpid() );

    QApplication a( argc, argv );
    
    GameWindow *gw = new GameWindow( uidUser ); 
    
    a.setMainWidget( gw );
    gw->show();
  
    return a.exec();
}

