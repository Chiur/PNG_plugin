#include "SkyVuPNGCodec.h"
#include <OgreImage.h>
#include <png.h>
#include <intrin.h>

const unsigned int PNG_MAGIC = 0x89504e47;

namespace PNG
{
	PNGCodec::PNGCodec() :
		mType("png")
	{
	}

	Ogre::String PNGCodec::getType() const
	{
		return mType;
	}

	DataStreamPtr PNGCodec::encode(MemoryDataStreamPtr& input, Codec::CodecDataPtr& pData) const
    {        
        OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
            "PNG encoding not supported",
            "PNGCodec::encode" ) ;
    }

	void PNGCodec::encodeToFile(MemoryDataStreamPtr& input, const Ogre::String& outFileName, Codec::CodecDataPtr& pData) const
    {
		OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
            "PNG encoding to file not supported",
            "PNGCodec::encodeToFile" ) ;
	}

	Codec::DecodeResult PNGCodec::decode(DataStreamPtr& stream) const
	{
		unsigned __int64 signature; // 64-bits/8 bytes

		// make sure it's a PNG first
		stream->read(&signature, 8);

#if OGRE_ENDIAN == OGRE_ENDIAN_LITTLE
		signature = _byteswap_uint64(signature);
#endif

		if(!png_sig_cmp((png_bytep)&signature, (png_size_t)0, (png_size_t)8))
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
				"Error: not a PNG",
				"PNGCodec::decode");
		}

		png_structp png_ptr;
		png_infop info_ptr;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
										 NULL, NULL, NULL);

		if(png_ptr == NULL)
		{
			stream->close();
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
				"Error: PNG Load Issue",
				"PNGCodec::decode");
		}

		info_ptr = png_create_info_struct(png_ptr);
		
		if(info_ptr == NULL)
		{
			stream->close();
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
				"Error: PNG Load Issue",
				"PNGCodec::decode");
		}

		// error handler
		if(setjmp(png_jmpbuf(png_ptr)))
		{
			stream->close();
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
				"Error: PNG Load Issue",
				"PNGCodec::decode");
		}

		// define a custom read function
		png_set_read_fn(png_ptr, stream.getPointer(), Read);

		// tell it we've already read the signature
		png_set_sig_bytes(png_ptr, 8);

		// read the rest of the PNG data

#if OGRE_ENDIAN == OGRE_ENDIAN_LITTLE
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | // expand 1/2/4 bit colors to 8-bit
										PNG_TRANSFORM_PACKING | // expand palettized images to RGB, grayscale to 8-bit
										PNG_TRANSFORM_SHIFT | // normalize pixels depth to sBIT depth (decompress)
										PNG_TRANSFORM_BGR | // RGB->BGR, RGBA->BGRA
										PNG_TRANSFORM_SWAP_ALPHA | // -> // RGBA->ARGB, GA->AG
										PNG_TRANSFORM_SWAP_ENDIAN, // swap 16-bit color channels endian
										NULL);
#else // big-endian, PNG's native format
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND |
										PNG_TRANSFORM_PACKING |
										PNG_TRANSFORM_SHIFT,
										NULL);
#endif

		// PNG should be entirely read by now through the "hi-level" route.
		// Alternatively, libPNG provides a lengthy low-level of reading
		// components one by one.

		// set imageData to read info for return result
		ImageData* imgData = OGRE_NEW ImageData();

		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
			&interlace_type, NULL, NULL);

		imgData->height = height;
		imgData->width = width;

		// handle PixelFormat here
		PixelFormat format = PF_UNKNOWN;
		imgData->flags = 0;

		// set flags from color type
		switch(color_type)
		{
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				imgData->flags |= PFF_HASALPHA;
			case PNG_COLOR_TYPE_GRAY:
				imgData->flags |= PFF_LUMINANCE;
				break;
			case PNG_COLOR_TYPE_RGB_ALPHA:
				imgData->flags |= PFF_HASALPHA;
			case PNG_COLOR_TYPE_PALETTE: // palette images expanded to RGB
			case PNG_COLOR_TYPE_RGB:
				// no flags needed
				break;
		}
		
		switch(bit_depth)
		{
			// since palettes aren't Ogre-supported, depths 1/2/4 are only grayscale
			case 1: case 2: case 4: // 8-bit
				imgData->format = PF_L8;
				break;
			case 8:
				if(imgData->flags & PFF_LUMINANCE) // grayscale
				{
					if(imgData->flags & PFF_HASALPHA)
					{
						imgData->format = PF_BYTE_LA; // 16-bit Gr/A
					}
					else
					{
						imgData->format = PF_L8; // 8-bit Gr
					}

				}
				else // rgb
				{
					if(imgData->flags & PFF_HASALPHA)
					{
						imgData->format = PF_R8G8B8A8; // 32-bit RGBA
					}
					else
					{
						imgData->format = PF_R8G8B8; // 24-bit RGB
					}
				}
				break;
			case 16:
				if(imgData->flags & PFF_LUMINANCE) // gray scale
				{
					if(imgData->flags & PFF_HASALPHA) // 16/16 Gr/A not supported
					{
						stream->close();
						png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
						OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
						"Error: 32-bit Grayscale + Alpha not supported.",
						"PNGCodec::decode");
					}
					else
					{
						imgData->format = PF_L16; // 16-bit Gr
					}
				}
				else // rgb
				{
					if(imgData->flags & PFF_HASALPHA)
					{
						imgData->format = PF_SHORT_RGBA; // 64-bit RGBA
					}
					else
					{
						imgData->format = PF_SHORT_RGB; // 48-bit RGB
					}
				}
		}

		imgData->depth = 1; // png is only a single depth
		imgData->num_mipmaps = 0; // png mipmaps not supported, although a custom read function could handle several files and combine them.

		imgData->size = Image::calculateSize(imgData->num_mipmaps, 1, 
            imgData->width, imgData->height, imgData->depth, imgData->format);

		// Bind output buffer
		MemoryDataStreamPtr output;
        output.bind(OGRE_NEW MemoryDataStream(imgData->size));

        void* destPtr = output->getPtr();
		png_bytepp rowPtrs = png_get_rows(png_ptr, info_ptr);
	
		// read raw pixel data from image to destPtr
		png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);

		int size = PixelUtil::getMemorySize(width, height, 1, imgData->format);
		int pitch = size / height;
		for(unsigned int i = 0; i < height; i++)
		{
			memcpy(destPtr, rowPtrs[i], pitch);
			destPtr = static_cast<void*>(static_cast<uchar*>(destPtr) + pitch);
		}

		// return pointer to pixel data and the image data
		DecodeResult result;
		result.first = output;
		result.second = CodecDataPtr(imgData);
		return result;
	}

	void PNGCodec::Read(png_structp png_ptr, png_bytep data, png_uint_32 length)
	{
		DataStream* stream = (DataStream*)png_get_io_ptr(png_ptr);
		unsigned n = stream->read(data, length);
	}

	Ogre::String PNGCodec::magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const
	{
		if(maxbytes >= sizeof(uint32))
		{
			uint32 fileType;
			memcpy(&fileType, magicNumberPtr, sizeof(uint32));
#if OGRE_ENDIAN == OGRE_ENDIAN_LITTLE
			fileType = _byteswap_ulong(fileType);
#endif
			if(PNG_MAGIC == fileType)
			{
				return String("png");
			}
		}

		return StringUtil::BLANK;
	}
}