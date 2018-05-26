#include "StdAfx.h"
#include "PngBitmapDecoder.h"
#include "libpng/png.h"
#include "standard/streamex.h"
#include <streambuf>

// PNG function for error handling
static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
    //longjmp(png_jmpbuf(png_ptr), 1);
}

// PNG function for warning handling
static void png_cpexcept_warn(png_structp png_ptr, png_const_charp msg)
{
}

// PNG function for file reading
void user_read_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length)
{
    png_size_t check;

    auto file = (std::istream*)png_get_io_ptr(png_ptr);
    check = (png_size_t)file->readsome((char*)data, (size_t)length);

    std::ios_base::eofbit;
    if (check != length && check != (png_size_t)-1)
        png_error(png_ptr, "Read Error");
}

System::Imaging::PngBitmapDecoder::PngBitmapDecoder()
{
}

System::Imaging::PngBitmapDecoder::~PngBitmapDecoder()
{
}

bool System::Imaging::PngBitmapDecoder::Decode(System::URI const & uri)
{
    return false;
}

bool System::Imaging::PngBitmapDecoder::Decode(std::vector<byte> const & stream)
{
    System::memorystreambuf memoryStream((std::vector<byte>*)&stream);
    std::istream _buf(&memoryStream);

    //Used to point to image rows
    byte** RowPointers = 0;

    if (stream.size() < 8)
        return false;

    png_byte* buffer = (png_byte*)stream.data();
    if (png_sig_cmp(buffer, 0, 8))
        return false;

    _buf.seekg(8);

    // Allocate the png read struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)png_cpexcept_error, (png_error_ptr)png_cpexcept_warn);
    if (!png_ptr)
        return false;

    // Allocate the png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return false;
    }

    png_set_read_fn(png_ptr, &_buf, user_read_data_fcn);
    png_set_sig_bytes(png_ptr, 8); // Tell png that we read the signature
    png_read_info(png_ptr, info_ptr); // Read the info section of the png file
    
    int32 BitDepth;
    int32 ColorType;
    {
        // Use temporary variables to avoid passing casted pointers
        png_uint_32 w, h;
        // Extract info
        png_get_IHDR(png_ptr, info_ptr,
            &w, &h,
            &BitDepth, &ColorType, NULL, NULL, NULL);
        Width = w;
        Height = h;
        PixelEntry.BitsPerPixel = BitDepth;
    }

    // Convert palette color to true color
    if (ColorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);
    
    // Convert low bit colors to 8 bit colors
    if (BitDepth < 8)
    {
        if (ColorType == PNG_COLOR_TYPE_GRAY || ColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_expand_gray_1_2_4_to_8(png_ptr);
        else
            png_set_packing(png_ptr);
    }
    
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);
    
    // Convert high bit colors to 8 bit colors
    if (BitDepth == 16)
        png_set_strip_16(png_ptr);
    
    // Convert gray color to true color
    if (ColorType == PNG_COLOR_TYPE_GRAY || ColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);
    
    int intent;
    const double screen_gamma = 2.2;
    
    if (png_get_sRGB(png_ptr, info_ptr, &intent))
        png_set_gamma(png_ptr, screen_gamma, 0.45455);
    else
    {
        double image_gamma;
        if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
            png_set_gamma(png_ptr, screen_gamma, image_gamma);
        else
            png_set_gamma(png_ptr, screen_gamma, 0.45455);
    }
    
    // Update the changes in between, as we need to get the new color type
    // for proper processing of the RGBA type
    png_read_update_info(png_ptr, info_ptr);
    {
        // Use temporary variables to avoid passing casted pointers
        png_uint_32 w, h;
        // Extract info
        png_get_IHDR(png_ptr, info_ptr,
            &w, &h,
            &BitDepth, &ColorType, NULL, NULL, NULL);
        Width = w;
        Height = h;
    }

    // Convert RGBA to BGRA
    if (ColorType == PNG_COLOR_TYPE_RGB_ALPHA)
    {
#ifdef __BIG_ENDIAN__
        png_set_swap_alpha(png_ptr);
#else
        png_set_bgr(png_ptr);
#endif
    }

    // Create the image structure to be filled by png data
    if (ColorType == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        PixelBitSize = 32;
        PixelEntry.Format = PixelFormat::Rgba8888;
    }
    else
    {
        PixelBitSize = 24;
        PixelEntry.Format = PixelFormat::Rgb888;
    }

    RowBytes = (PixelBitSize / 8.0) * Width;
    Data.resize(size_t(RowBytes * Height));

    if (Data.empty())
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }

    // Create array of pointers to rows in image data
    RowPointers = new png_bytep[size_t(Height)];
    if (!RowPointers)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        Data.clear();
        return false;
    }

    // Fill array of pointers to rows in image data
    unsigned char* data = (unsigned char*)Data.data();
    for (uint i = 0; i<Height; ++i)
    {
        RowPointers[i] = data;
        data += size_t(RowBytes);
    }

    // for proper error handling
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        delete[] RowPointers;
        Data.clear();
        return false;
    }

    // Read data using the library function that handles all transformations including interlacing
    png_read_image(png_ptr, RowPointers);

    png_read_end(png_ptr, NULL);
    delete[] RowPointers;
    png_destroy_read_struct(&png_ptr, &info_ptr, 0); // Clean up memory
    return true;
}
