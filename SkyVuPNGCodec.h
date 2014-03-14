#include "SkyVuPNGPrerequisites.h"
#include <OgrePrerequisites.h>
#include <OgreImageCodec.h>
#include <OgreDataStream.h>
#include <OgreSharedPtr.h>
#include <png.h>

namespace PNG
{
	using namespace Ogre;

	class _SkyVuPNGExport PNGCodec : public Ogre::ImageCodec
	{
	private:
		Ogre::String mType;
	public:
		PNGCodec();
		virtual ~PNGCodec() {};

		Ogre::DataStreamPtr encode(MemoryDataStreamPtr& input, CodecDataPtr& pData) const;
		void encodeToFile(MemoryDataStreamPtr& input, const Ogre::String& outFileName, CodecDataPtr& pData) const;
		DecodeResult decode(DataStreamPtr& stream) const;
		Ogre::String magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const;
		virtual Ogre::String getType() const;

		static void Read(png_structp png_ptr, png_bytep data, png_uint_32 length);
	};
}