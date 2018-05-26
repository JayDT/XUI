#ifndef __XUI_DROPTARGET_H__
#define __XUI_DROPTARGET_H__

#include "DropTargetBase.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorableFloatingWindow.h"
#include "UI/LayoutDock/Layouts/LayoutDocumentFloatingWindow.h"
#include "UI/LayoutDock/Layouts/LayoutRoot.h"
#include "Core/GlobalInitializerScheduler.h"
#include "Core/Media/Rect_t.h"

namespace XUI::UI
{
    template<typename T>
    class DropTarget : public DropTargetBase
    {
        static_assert(std::is_base_of<Controls::Control, T>::value, "T must inherit from FrameworkElement");

    private:
        std::vector<Core::Media::Rect> _detectionRect;
        std::shared_ptr<T> _targetElement;
        DropTargetType _type = DropTargetType::DockingManagerDockLeft;

    public:
        PROPERTY_GET_ONLY(std::vector<Core::Media::Rect> , DetectionRects);
        std::vector<Core::Media::Rect> const& GetDetectionRects() const
        {
            return _detectionRect;
        }

        PROPERTY_GET_ONLY(T* , TargetElement);
        T* GetTargetElement() const
        {
            return _targetElement.get();
        }

        PROPERTY_GET_ONLY(DropTargetType , DropType);
        DropTargetType GetDropType() const
        {
            return _type;
        }

        virtual void Drop(LayoutFloatingWindow *floatingWindow)
        {
            LayoutRoot* root = floatingWindow->Root;
            auto currentActiveContent = root->ActiveContent;
            auto fwAsAnchorable = floatingWindow->Static_As<LayoutAnchorableFloatingWindow>();

            if (fwAsAnchorable != nullptr)
            {
                this->Drop(fwAsAnchorable);
            }
            else
            {
                auto fwAsDocument = floatingWindow->Static_As<LayoutDocumentFloatingWindow>();
                this->Drop(fwAsDocument);
            }

            if (currentActiveContent)
            {
                Core::UIDispatcher::OnRender += [content = currentActiveContent->weak_from_base_static<LayoutContent>()]()
                {
                    if (!content.expired())
                    {
                        auto currentActiveContent = content.lock().get();
                        currentActiveContent->IsSelected = false;
                        currentActiveContent->IsActive = false;
                        currentActiveContent->IsActive = true;
                    }
                };
            }
        }

        bool HitTest(Core::Media::Point const& dragPoint) override
        {
            for (auto const& dr : _detectionRect)
            {
                if (dr.contains(dragPoint))
                    return true;
            }

            return false;
        }

        void DragEnter()
        {
            SetIsDraggingOver(TargetElement, true);
        }

        void DragLeave()
        {
            SetIsDraggingOver(TargetElement, false);
        }

    protected:
        DropTarget(std::shared_ptr<T> targetElement, Core::Media::Rect const& detectionRect, DropTargetType type)
        {
            _targetElement = targetElement;
            _detectionRect = { detectionRect };
            _type = type;
        }

        DropTarget(std::shared_ptr<T> targetElement, std::vector<Core::Media::Rect> &detectionRects, DropTargetType type)
        {
            _targetElement = targetElement;
            _detectionRect = detectionRects.ToArray();
            _type = type;
        }

        virtual void Drop(LayoutAnchorableFloatingWindow *floatingWindow)
        {
        }

        virtual void Drop(LayoutDocumentFloatingWindow *floatingWindow)
        {
        }
    };
}


#endif	//#ifndef __XUI_DROPTARGET_H__
