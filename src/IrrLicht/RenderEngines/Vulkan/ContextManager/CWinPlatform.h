#ifndef __C_WGL_MANAGER_H_INCLUDED__
#define __C_WGL_MANAGER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "CVulkanPlatform.h"

namespace irr
{
namespace video
{
    // WGL manager.
    class CWinVulkanPlatform : public CVulkanPlatform
    {
    public:
        //! Constructor.
        CWinVulkanPlatform(CVulkanDriver* driver)
            : CVulkanPlatform(driver)
        {}

		//! Destructor
        virtual ~CWinVulkanPlatform();

        // Inherited via CVulkanPlatform
        virtual void initialize() override;
        virtual void resizeSwapBuffers() override;

    protected:

        bool mIsFullScreen = false;

    };
}
}

#endif

#endif
