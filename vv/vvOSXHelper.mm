#include <Cocoa/Cocoa.h>
#include "vvOSXHelper.h"

// For retina displays, see
// http://public.kitware.com/pipermail/vtkusers/2015-February/090117.html
void disableGLHiDPI( long a_id ){
 NSView *view = reinterpret_cast<NSView*>( a_id );
 [view setWantsBestResolutionOpenGLSurface:NO];
}

