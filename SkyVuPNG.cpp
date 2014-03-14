#include "SkyVuPNGPlugin.h"
#include <OgreRoot.h>

namespace PNG
{
	SkyVuPNGPlugin* plugin;

	extern "C" void _SkyVuPNGExport dllStartPlugin(void)
	{
		plugin = new SkyVuPNGPlugin();

		Ogre::Root::getSingleton().installPlugin(plugin);
	}

	extern "C" void _SkyVuPNGExport dllStopPlugin(void)
	{
		Ogre::Root::getSingleton().uninstallPlugin(plugin);
		delete plugin;
	}
}