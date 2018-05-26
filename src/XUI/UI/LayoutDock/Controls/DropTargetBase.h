#ifndef __XUI_DROPTARGETBASE_H__
#define __XUI_DROPTARGETBASE_H__

#include "Core/Dependencies/DependencyObject.h"
#include "UI/LayoutDock/DockingDefines.h"

namespace XUI::UI
{
    class OverlayWindow;
    class LayoutFloatingWindow;

    class DropTargetBase : public Core::Dependency::DependencyObject
    {
    public:
        /// <summary>
        /// IsDraggingOver Attached Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsDraggingOverProperty;

        /// <summary>
        /// Gets the IsDraggingOver property.  This dependency property 
        /// indicates if user is dragging a window over the target element.
        /// </summary>
        static bool GetIsDraggingOver(DependencyObject *d);

        /// <summary>
        /// Sets the IsDraggingOver property.  This dependency property 
        /// indicates if user is dragging away a window from the target element.
        /// </summary>
        static void SetIsDraggingOver(DependencyObject *d, bool value);

        static void StaticClassInitializer();

        virtual std::shared_ptr<Core::Media::Geometry> GetPreviewPath(OverlayWindow *overlayWindow, LayoutFloatingWindow *floatingWindow) = 0;

        virtual bool HitTest(Core::Media::Point const& dragPoint) = 0;

        PROPERTY_GET_ONLY(DropTargetType, DropType);
        virtual DropTargetType GetDropType() const = 0;

        virtual void Drop(LayoutFloatingWindow *floatingWindow) = 0;

        virtual void DragEnter() = 0;

        virtual void DragLeave() = 0;
    };
}


#endif	//#ifndef __XUI_DROPTARGETBASE_H__
