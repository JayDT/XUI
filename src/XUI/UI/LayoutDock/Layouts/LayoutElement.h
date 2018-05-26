/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ELEMENT_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ELEMENT_H__

#include "Core/Dependencies/DependencyObject.h"
#include "UI/LayoutDock/DockingEventArgs.h"
#include "standard/LinkedReference/Reference.h"

namespace XUI::UI
{
    class LayoutContainer;
    class LayoutRoot;
    class LayoutDocumentPaneGroup;

    class TC_XUI_API LayoutElement : public Core::Dependency::DependencyObject
    {
        friend class LayoutRoot;

    protected:

        std::weak_ptr<LayoutContainer> m_parent;
        mutable std::weak_ptr<LayoutRoot> m_root;

        class LayoutElementReference : public System::Collection::Reference<LayoutElement, LayoutRoot>
        {
        protected:
            void targetObjectBuildLink() override;
            void targetObjectDestroyLink() override {}
            void sourceObjectDestroyLink() override {}

        public:
            explicit LayoutElementReference() : System::Collection::Reference<LayoutElement, LayoutRoot>() {}
            virtual ~LayoutElementReference() { unlink(); }

        } m_layoutElementReference;

    public:

        LayoutElement();
        virtual ~LayoutElement();

        PROPERTY_(LayoutContainer *, Parent);
        LayoutContainer * GetParent() const { return m_parent.lock().get(); }
        void SetParent(LayoutContainer* value);

        PROPERTY_GET_ONLY(LayoutRoot *, Root);
        LayoutRoot* GetRoot() const;

        virtual void ComputeVisibility()
        {
        }

        virtual bool GetIsVisible() const { return true; }
        virtual LayoutContainer* GetPreviousContainer() const { return nullptr; }
        virtual void SetPreviousContainer(LayoutContainer* value) {}
        virtual int  GetPreviousContainerIndex() const { return -1; }
        virtual void SetPreviousContainerIndex(int value) {}

        template<class T>
        static T* FindParent(LayoutElement* element)
        {
            auto parent = element->GetParent();
            while (parent && !parent->Is<T>())
                parent = parent->GetParent();

            return parent ? parent->Static_As<T>() : nullptr;
        }

        static LayoutRoot* GetRoot(LayoutElement* element);

        static AnchorSide GetSide(LayoutElement* element);

    protected:

        /// <summary>
        /// Provides derived classes an opportunity to handle execute code before to the Parent property changes.
        /// </summary>
        virtual void OnParentChanging(LayoutContainer* oldValue, LayoutContainer* newValue)
        {
        }

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Parent property.
        /// </summary>
        virtual void OnParentChanged(LayoutContainer* oldValue, LayoutContainer* newValue)
        {
        }

        virtual void OnRootChanged(LayoutRoot* oldRoot, LayoutRoot* newRoot);
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ELEMENT_H__