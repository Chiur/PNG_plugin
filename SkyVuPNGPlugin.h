#include "SkyVuPNGPrerequisites.h"
#include <Ogre.h>
#include <OgrePlugin.h>

using namespace Ogre;

namespace PNG
{
	class SkyVuPNGPlugin : public Plugin
	{
		public:
		SkyVuPNGPlugin();

		void install();
		void initialise();
		void shutdown();
		void uninstall();

		const String& getName() const;
	};
}