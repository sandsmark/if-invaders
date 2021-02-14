#ifndef STL_SCORE_H
#define STL_SCORE_H


#undef HISCOREINCLUDE
#define HISCOREINCLUDE

#include "allinclude.h"





class Hiscore;
class HallOfFame;

// this should not be in the global namespace
typedef std::vector< Hiscore* > :: iterator VHI;
typedef unsigned long ulong;
typedef unsigned int uint;





class Hiscore
{
  std::string    logname;
  ulong     score;
  uid_t     ident;

  bool      getUserName();

public:
  Hiscore();
  Hiscore( uid_t, ulong );
  ~Hiscore(){}

  const char*    getName();
  ulong     getScore();
  uid_t     getUid();
  Hiscore*  readFromFile( FILE* );
  void      writeToFile( FILE* );
  void      updateScore( ulong nscore );
};





class HallOfFame
{
protected:
    const unsigned int max_entries;
    std::vector< Hiscore* > scores;

    std::string fname;
    void   setLock( FILE*, int lock_type );
    void   writeToFile( FILE* );
 
public:
    HallOfFame( const std::string &s = HISCOREFILE , const uint m = MAXENTRIES );
    ~HallOfFame();

    void   readFromFile( FILE *f = 0 );
    void   updateFile( uid_t ident, ulong score );

    ulong  getMaxScore();

    // returns position in score table where result 'score' might be placed
    VHI    scorePosition( ulong score );
    VHI    userPosition( uid_t ident );
    // whether user was recorded in hiscore structure
    bool   hasUser( uid_t ident );  
    // get 'ident' s best score in score table
    ulong  getScore( uid_t ident );

    void   removeScore( VHI pos );
    void   removeAll();

    bool   updateUser( uid_t ident, ulong nscore );
    bool   insertNewScore( uid_t ident, ulong score );
};





// Predicates - find_if(), for_each(), etc. STL
struct LookUpUid
{
  uid_t  key;
  
  explicit LookUpUid( const uid_t k ) : key( k ) {}
  bool   operator()( Hiscore *h ){  return h->getUid() == key; }
};





struct LookUpScore
{
  ulong   key;
  
  explicit LookUpScore( const ulong k ) : key( k ){}
  bool   operator()( Hiscore *h ){  return key >= h->getScore(); }
};





struct Hiscore_gt
{
  bool   operator()( Hiscore *x, Hiscore *y )
  {  
    return  x->getScore() > y->getScore();
  }
};





// Operators
struct WriteEntry
{
  FILE *fp;

  explicit WriteEntry( FILE *f ) : fp( f ){}
  void   operator()( Hiscore *h ){  h->writeToFile( fp ); }
};





struct FlushOne
{
  void   operator()( Hiscore *h ){  delete h; }
};


#endif
