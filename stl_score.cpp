#include "stl_score.h"





Hiscore :: Hiscore()
{

    logname = std::string( "" );
    score = ident = 0;
}


Hiscore :: Hiscore( uid_t i, ulong s )
{

    ident = i;
    score = s;
    getUserName();
}


bool
Hiscore :: getUserName()
{

    if ( getpwuid( ident ) == 0 ) {
	warning( "cannot get info about user %d", 
		 static_cast< int >( ident ) );
	return false;
    }

    logname = std::string( getpwuid( ident )->pw_name );
    return true;
}


const char*
Hiscore :: getName()
{

    return ( logname.length() ) ? logname.c_str() : "unknown";
}


ulong
Hiscore :: getScore()
{

    return score;
}


uid_t
Hiscore :: getUid()
{
  
    return ident;
}


Hiscore*
Hiscore :: readFromFile( FILE *f )
{

    if ( fscanf( f, "%lu %lu", &ident, &score ) < 2 )
	fatal( "hiscore file's corrupt. Giving up..." );
  
    getUserName();
    return this;
}


void 
Hiscore :: writeToFile( FILE *f )
{

    if ( fprintf( f, "%lu %lu\n", ident, score ) < 0 )
	fatal( "writing hiscore failed. Giving up..." );
}


void
Hiscore :: updateScore( ulong nscore )
{

    score = nscore;
}





HallOfFame :: HallOfFame( const std::string &s, const uint m )
           :  max_entries( m )
{

    fname = s;
    scores = std::vector< Hiscore* >();
    scores.reserve( max_entries );
}


HallOfFame :: ~HallOfFame()
{

    for_each( scores.begin(), scores.end(), FlushOne() );
    scores.clear();
}


void
HallOfFame :: setLock( FILE *f, int type )
{
    struct flock fl;

    fl.l_type = type;
    fl.l_start = SEEK_SET;   // from the beginning
    fl.l_whence = 0;
    fl.l_len = 0;            // ... until eof

    // I had some trouble with F_SETLKW last time. Faking it... Does
    // this thing work over NFS?
    while ( fcntl( fileno( f ), F_SETLK, &fl ) == -1 ) {
	warning( "File's busy. Trying harder..." );
	sleep( 2 );
    }
}


ulong
HallOfFame :: getMaxScore()
{

    return ( scores.size() ? scores[0]->getScore() : 0 );
}


VHI
HallOfFame :: userPosition( uid_t ident )
{

    return find_if( scores.begin(), scores.end(), LookUpUid( ident ) );
}


bool
HallOfFame :: hasUser( uid_t ident )
{

    return userPosition( ident ) != scores.end();
}


VHI
HallOfFame :: scorePosition( ulong sc )
{

    return find_if( scores.begin(), scores.end(), LookUpScore( sc ) );
}


void
HallOfFame :: removeScore( VHI pos )
{

    delete *pos;
    scores.erase( pos );
}


void
HallOfFame :: removeAll()
{

    for_each( scores.begin(), scores.end(), FlushOne() );
    scores.clear();
}


ulong
HallOfFame :: getScore( uid_t id )
{
    VHI tmp = find_if( scores.begin(), scores.end(), LookUpUid( id ) );
  
    if ( tmp != scores.end() ) 
	return (*tmp)->getScore();
    else 
	return 0;
}

		     
bool
HallOfFame :: insertNewScore( uid_t id, ulong sc )
{
    VHI pos = scorePosition( sc );

    if ( scores.size()+1 < scores.capacity() ) {
	scores.insert( pos, new Hiscore( id, sc ) );
	return true;
    }

    if ( pos < scores.end() ) {
	removeScore( scores.end()-1 );
	// is this one really necessary?
	pos = scorePosition( sc );
	scores.insert( pos, new Hiscore( id, sc ) );
	return true;
    }
    
    return false;
}


bool
HallOfFame :: updateUser( uid_t id, ulong newScore )
{
    VHI oldP = userPosition( id ), newP = scorePosition( newScore );

    if ( newP < oldP || newP == oldP ) {
	(*oldP)->updateScore( newScore );
	stable_sort( scores.begin(), scores.end(), Hiscore_gt() );
	return true;
    }
    else if ( newP == scores.end() )
	return true;

    return false;
}


// when a NULL arg is given, we need a genuine read, not re-read for update
// hmm - the file'd better be correct - no check is done on fscanf()
void
HallOfFame :: readFromFile( FILE *f )
{
    uint i;
    FILE *fptr;
  
    startOwner();

    if ( f != 0 ) {
        fptr = f;
	rewind( fptr );
    }
    else {
        if ( (fptr = fopen( fname.c_str(), "r" )) == 0 )
	    fatal( "failed to open hiscore file" );

        setLock( fptr, F_RDLCK );
    }

    fscanf( fptr, "%u", &i );
    i = ( i > scores.capacity() ) ? scores.capacity() : i;
    for ( uint j = 0; j < i; ++j )
	scores.push_back( (new Hiscore())->readFromFile( fptr ) );

    if ( f == 0 ) {
        setLock( fptr, F_UNLCK );
        fclose( fptr );
    }

    startUser();
}
 

// ok, this one is now atomic
void
HallOfFame :: updateFile( uid_t who, ulong lastScore )
{
    FILE *fptr;

    removeAll();

    startOwner();

    // Opening for "append" - do not want to flush content unless a lock
    // can be placed
    if ( (fptr = fopen( fname.c_str(), "r+" )) == 0 ) 
	fatal( "open hiscore file failed" );
 
    setLock( fptr, F_WRLCK );

    readFromFile( fptr );

    if ( hasUser( who ) && updateUser( who, lastScore ) )
	writeToFile( fptr ); 
    else if ( !hasUser( who ) && insertNewScore( who, lastScore ) )
	writeToFile( fptr );

    setLock( fptr, F_UNLCK );
    fclose( fptr );
  
    startUser();
}


void
HallOfFame :: writeToFile( FILE *fptr )
{

    rewind( fptr );
    fprintf( fptr, "%d\n", scores.size() );
    for_each( scores.begin(), scores.end(), WriteEntry( fptr ) );
}
