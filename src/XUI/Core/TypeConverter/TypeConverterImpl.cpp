#include "TypeConverterImpl.h"
#include "Core/Media/Brush.h"
#include "Core/Media/Image_t.h"
#include "System/Image/PngBitmapDecoder.h"
#include "System/Uri.h"
#include <System/Resource/ResourceManager.h>
#include "Platform/UIRenderer.h"
#include "Platform/UIService.h"

#include <boost/filesystem.hpp>

System::Reflection::Variant XUI::Core::TypeConversion::BrushValueConverter::ConvertFrom(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
	if (*value.GetType() == typeid(std::string))
	{
		System::Reflection::Variant result = std::static_pointer_cast<XUI::Interfaces::IBrush>(XUI::Core::Media::Brush::Parse(value.ToString()));
		return result;
	}
	else if (*value.GetType() == typeid(Core::Media::Color))
	{
		return std::make_shared<Core::Media::SolidColorBrush>(*static_cast<Core::Media::Color*>(value.ToObject()));
	}
	//
	return System::Reflection::Variant(nullptr);
}

System::Reflection::Variant XUI::Core::TypeConversion::BrushValueConverter::ConvertTo(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
	return value.GetValue<std::shared_ptr<XUI::Interfaces::IBrush>>().get()->ToString();
}

bool XUI::Core::TypeConversion::BrushValueConverter::CanConvertTo(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
	return *destinationType == typeid(std::string);
}

bool XUI::Core::TypeConversion::BrushValueConverter::CanConvertFrom(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
	return *sourceType == typeid(std::string) || *sourceType == typeid(Core::Media::Color);
}

System::Reflection::Variant XUI::Core::TypeConversion::BitMapConverter::ConvertFrom(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    if (*value.GetType() == typeid(std::string))
    {
        std::string uriStr = "pack://application:,,,";
        uriStr += value.ToString();
        System::URI uri(uriStr, true);
        System::Resource::ResourceObject* resource = System::Resource::ResourceManager::Instance()->FindResource(uri);
        if (resource)
        {
            boost::filesystem::path filename(resource->Name());
            std::string ext = System::String(filename.extension().string()).ToLower();
            if (ext == ".png")
            {
                System::Imaging::PngBitmapDecoder png;
                if (png.Decode(resource->ToBytes()))
                {
                    return Core::Media::Image_t::CreateBitmapFromImageSource(&png);
                }
            }
            //platformService->CreateWritableBitmap();
            //System::Reflection::Variant result = std::static_pointer_cast<XUI::Interfaces::IBrush>(XUI::Core::Media::Brush::Parse(value.ToString()));
            //return result;
        }
    }
    //else if (*value.GetType() == typeid(System::URI))
    //{
    //}
    //
    return System::Reflection::Variant(nullptr);
}

System::Reflection::Variant XUI::Core::TypeConversion::BitMapConverter::ConvertTo(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    return System::Reflection::Variant();
}

bool XUI::Core::TypeConversion::BitMapConverter::CanConvertTo(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return *destinationType == typeid(std::string);
}

bool XUI::Core::TypeConversion::BitMapConverter::CanConvertFrom(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string) || *sourceType == typeid(System::URI);
}
