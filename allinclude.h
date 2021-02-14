// This is the general include file for all of the game files. Each of
// them defines the proper vars and then includes this file.





#ifdef HISCOREINCLUDE

// These are not ANSI (actually POSIX)
#include <sys/types.h>  // for fcntl
#include <fcntl.h>      // file-locking
#include <pwd.h>        // user identification. 
#include <unistd.h>

// ADT
#include <vector>       // data structure
#include <algorithm>    // snacks for the above header
#include <functional>   // predicates...

// MISC
#include <string>
#include <cstdio>       // C-style files.

// QT
#include <qapp.h>

// these opts should be defined compile time - makefile.
#ifndef HISCOREFILE
#define HISCOREFILE "hiscore.dta"
#endif

#ifndef MAXENTRIES
#define MAXENTRIES  50
#endif

extern void startOwner();
extern void startUser();

#endif





#ifdef GAMEWINDOWINCLUDE

// QT
#include <qwidget.h>
#include <qpushbt.h>
#include <qlcdnum.h>
#include <qpalette.h>
#include <qcursor.h>
#include <qbitmap.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qtooltip.h>

// MISC
#include "stl_score.h"
#include "gameboard.h"

#endif





#ifdef SHUTTLEINCLUDE

// QT
#include <qrect.h>
#include <qpixmap.h>
#include <qpainter.h>

#ifndef SHUTTLEPIX
#define SHUTTLEPIX "pixmaps/ship2.xpm"
#endif

#endif





#ifdef GAMEBOARDINCLUDE
// ADT
#include <list>
#include <vector>
#include <set>

// QT
#include <qwidget.h>
#include <qtimer.h>
#include <qrect.h>
#include <qpainter.h>
#include <qevent.h>
#include <qkeycode.h>

// MISC 
#include <cmath>
#include <cstdlib>
#include <cassert>
#include "invader.h"
#include "shuttle.h"
#include "barricade.h"

#ifndef SHUTTLESHOTPIX
#define SHUTTLESHOTPIX "pixmaps/ship_bullet2.xpm"
#endif   

#ifndef ALIENSHOT1
#define ALIENSHOT1  "pixmaps/sperma2.xpm"
#define ALIENSHOT2  "pixmaps/spermb2.xpm"
#endif


// NON-PORTABLE. 
#include <X11/Xlib.h>

#endif





#ifdef BARRICADEINCLUDE

// ADT
#include <vector>

// MISC
#include <limits.h>

// QT
#include <qimage.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qwidget.h>

#ifndef WALLPIX
#define WALLPIX "pixmaps/barricade2.xpm"
#endif

#endif





#ifdef INVADERINCLUDE

// QT
#include <qrect.h>
#include <qpixmap.h>
#include <qpainter.h>

#ifndef SILLYPIX1
#define SILLYPIX1  "pixmaps/vader1a2.xpm"
#define SILLYPIX2  "pixmaps/vader1b2.xpm"
#endif

#ifndef CLEVERPIX1
#define CLEVERPIX1 "pixmaps/vader2a2.xpm"
#define CLEVERPIX2 "pixmaps/vader2b2.xpm"
#endif

#ifndef FATPIX1 
#define FATPIX1    "pixmaps/vader3a2.xpm"
#define FATPIX2    "pixmaps/vader3b2.xpm"
#endif

#ifndef BIGBOSSPIX
#define BIGBOSSPIX "pixmaps/spacer2.xpm"
#endif

#endif
