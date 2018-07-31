#pragma once

#include "CCommonDefines.h"
#include <standard/enum.h>

namespace irr
{
    namespace video
    {
        /** Flags that determine how is a resource being used by the GPU. */
        enum class VulkanUseFlag
        {
            None = 0,
            Read = 0x1,
            Write = 0x2
        };

        ENABLE_ENUM_CLASS_FLAG(VulkanUseFlag)
    }
}