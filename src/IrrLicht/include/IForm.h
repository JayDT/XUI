#ifndef __IFORM_H__
#define __IFORM_H__

#include "IReferenceCounted.h"

namespace irr
{
    namespace io{
        class IReadFile;
    }

    namespace gui{
        class IGUIWindow;
    }

    class IrrlichtDevice;

    namespace Forms
    {
        class IForm : public virtual IReferenceCounted
        {
        public:
            explicit IForm() {}
            virtual ~IForm() {}

            virtual void InitComponents(irr::io::IReadFile* Settings) = 0;
            virtual irr::IrrlichtDevice * GetDevice() = 0;
            virtual irr::IrrlichtDevice const* GetDevice() const = 0;
            virtual unsigned int GetID() const = 0;
            virtual irr::gui::IGUIWindow* GetWindowHandle() = 0;
            virtual IEventReceiver* GetEventReceiver() { return nullptr; }

            virtual void OnRender() {}

            void SetFormWindow(gui::IGUIWindow* wnd)
            {
                wnd->SetParentForm(this);
            }
        private:
        };
    }
}
#endif //__IFORM_H__