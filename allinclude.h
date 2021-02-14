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

#endif





#ifdef INVADERINCLUDE

// QT
#include <qrect.h>
#include <qpixmap.h>
#include <qpainter.h>

#endif
