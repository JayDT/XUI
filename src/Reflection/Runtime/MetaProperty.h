#pragma once

#ifndef __REFL_META_PROPERTY_H__
#define __REFL_META_PROPERTY_H__

#include "standard/Platform/Define.h"
#include "System/rtString.h"

#include "Macros.h"
#include "Object.h"

namespace System::Reflection 
{ 
    inline namespace meta
    {
        class TC_METARUNTIME_API MetaProperty : public ISerializable
        {
        public:
            MetaProperty();
            virtual ~MetaProperty();
        };

        template<typename PropertyType, typename ...Args>
        MetaProperty *MetaPropertyInitializer(Args&&... args);
    } 
}

#include "System/BuildInMetaAttributes.h"
#include "Impl/MetaProperty.hpp"
#endif //__REFL_META_PROPERTY_H__