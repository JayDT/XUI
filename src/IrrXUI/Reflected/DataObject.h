#ifndef __WORLDCLIENT_DATA_OBJECT_H__
#define __WORLDCLIENT_DATA_OBJECT_H__

#include "Reflection/Runtime/MetaProperty.h"
#include "Reflection/Runtime/Object.h"
#include "standard/BasicPrimities.h"
#include "XamlCPP/Core/NotifyPropertyChanged.h"

namespace WorldClient
{
    struct Meta(Enable) UnitTestDataContext : public XamlCPP::Core::NotifyPropertyChanged
    {
        std::wstring _text;

        UnitTestDataContext();
        virtual ~UnitTestDataContext() {}

        PROPERTY_T(std::wstring, StringValue)
        PROPERTY_T_GET(StringValue, const&) { return _text; }
        PROPERTY_T_SET(StringValue) 
        {
            if (_text != value)
            {
                _text = value;
                RaisePropertyChanged(nameof(StringValue));
            }
        }

        std::string ToString() const override
        {
            return System::String(_text);
        }
    };
};

#endif //!__WORLDCLIENT_DATA_OBJECT_H__