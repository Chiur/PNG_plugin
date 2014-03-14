#ifndef __SkyVuPNGPrerequisites_H__
#define __SkyVuPNGPrerequisites_H__
 
#include <OgrePrerequisites.h>
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#   ifdef SKYVU_PNG_EXPORTS
#       define _SkyVuPNGExport __declspec(dllexport) 
#   else 
#       define _SkyVuPNGExport __declspec(dllimport) 
#   endif 
#else 
#   define _SkyVuPNGExport 
#endif
#endif // __SkyVuPNGPrerequisites_H__