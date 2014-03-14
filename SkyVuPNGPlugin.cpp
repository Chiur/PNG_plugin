#include "SkyVuPNGPlugin.h"
#include "SkyVuPNGCodec.h"
#include <OgreCodec.h>

namespace PNG
{
	const String sPluginName = "SkyVuPNGPlugin";
	PNGCodec* codec;

	//---------------------------------------------------------------------
	SkyVuPNGPlugin::SkyVuPNGPlugin()
	{
	}
	//---------------------------------------------------------------------
	const String& SkyVuPNGPlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void SkyVuPNGPlugin::install()
	{
		codec = OGRE_NEW PNGCodec();
		Codec::registerCodec(codec);
	}
	//---------------------------------------------------------------------
	void SkyVuPNGPlugin::initialise()
	{
	}
	//---------------------------------------------------------------------
	void SkyVuPNGPlugin::shutdown()
	{
		Codec::unregisterCodec(codec);
	}
	//---------------------------------------------------------------------
	void SkyVuPNGPlugin::uninstall()
	{
		OGRE_DELETE codec;
	}
}