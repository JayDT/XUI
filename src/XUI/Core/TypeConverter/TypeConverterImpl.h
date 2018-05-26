#ifndef __XUI_CORE_TYPE_CONVERTER_IMPL_H__
#define __XUI_CORE_TYPE_CONVERTER_IMPL_H__

#include "XamlCPP/Interfaces/ITypeConverter.h"
#include "XamlCPP/Interfaces/IValueContext.h"

namespace XUI::Core::TypeConversion
{
	struct BrushValueConverter : public XamlCPP::Interfaces::ITypeConverter
	{
		// Inherited via ITypeConverter
		virtual System::Reflection::Variant ConvertFrom(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
		virtual System::Reflection::Variant ConvertTo(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
		virtual bool CanConvertTo(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
		virtual bool CanConvertFrom(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
	};

    struct BitMapConverter : public XamlCPP::Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };
}

#endif //! __XUI_CORE_TYPE_CONVERTER_IMPL_H__