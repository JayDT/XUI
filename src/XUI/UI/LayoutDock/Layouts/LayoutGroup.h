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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_GROUP_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_GROUP_H__

#include "UI/LayoutDock/Layouts/LayoutGroupBase.h"
#include "Reflection/Runtime/ReflectionModule.h"

namespace XUI::UI
{
    template<class TElement>
    class TC_XUI_API LayoutGroup : public LayoutGroupBase
    {
    protected:

        bool _isVisible = true;

    public:

        LayoutGroup();
        virtual ~LayoutGroup();

        PROPERTY_(bool, IsVisible);
        bool GetIsVisible() const override { return _isVisible; }
        void SetIsVisible(bool const& value)
        {
            if (_isVisible != value)
            {
                _isVisible = value;
                OnIsVisibleChanged();
                NotifyPropertyChanged::RaisePropertyChanged("IsVisible");
            }
        }

        void CollectionChanged_children(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);
        virtual void OnIsVisibleChanged();
        void UpdateParentVisibility();
        void ComputeVisibility() override;
        void AddChild(LayoutElement* child) override
        {
            if (!child->Is<TElement>())
                throw System::ArgumentException();
            LayoutGroupBase::AddChild(child);
        }
        void AddChild(std::shared_ptr<LayoutElement> const& child)
        {
            AddChild(child.get());
        }
        TElement* GetChild(int Index) const
        {
            return static_cast<TElement*>(m_children[Index].get());
        }
        LayoutElement* GetChildElement(int Index) const override
        {
            return m_children[Index].get();
        }
        void MoveChild(int oldIndex, int newIndex) override;
        void RemoveChildAt(int childIndex) override;
        int IndexOfChild(LayoutElement* element) override;
        void InsertChildAt(int index, LayoutElement* element) override;
        void InsertChildAt(int index, std::shared_ptr<LayoutElement> const& element)
        {
            InsertChildAt(index, element.get());
        }
        void RemoveChild(LayoutElement* element) override;
        void ReplaceChild(LayoutElement* oldElement, LayoutElement* newElement) override;
        size_t ChildrenCount();
        void ReplaceChildAt(int index, LayoutElement* element);
        void* GetSchema();

        
        virtual void ReadXml(void* reader);
        virtual void WriteXml(void* writer);

    protected:

        virtual void ChildMoved(int oldIndex, int newIndex);

        virtual bool GetVisibility() = 0;
        void OnParentChanged(LayoutContainer* oldValue, LayoutContainer* newValue) override;

    private:

        const System::Reflection::Type* FindType(std::string const& name);
    };

    template<class TElement>
    inline LayoutGroup<TElement>::LayoutGroup()
    {
        m_children.CollectionChanged += System::make_eventhandler(&LayoutGroup::CollectionChanged_children, this);
    }

    template<class TElement>
    inline LayoutGroup<TElement>::~LayoutGroup()
    {
    }

    template<class TElement>
    void LayoutGroup<TElement>::CollectionChanged_children(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
    {
        if (e.Action == System::Collection::NotifyCollectionChangedAction::Remove)
        {
            LayoutElement* element = static_cast<LayoutElement*>(m_children[e.OldStartingIndex].get());
            if (element != nullptr)
            {
                if (element->GetParent() == this)
                    element->SetParent(nullptr);
            }
        }

        if (e.Action == System::Collection::NotifyCollectionChangedAction::Add)
        {
            LayoutElement* element = static_cast<LayoutElement*>(m_children[e.NewStartingIndex].get());
            if (element != nullptr)
            {
                if (element->GetParent() && element->GetParent() != this)
                    element->GetParent()->RemoveChild(element);
                element->SetParent(this);
            }
        }

        ComputeVisibility();
        OnChildrenCollectionChanged();
        NotifyChildrenTreeChanged(ChildrenTreeChange::DirectChildrenChanged);
        NotifyPropertyChanged::RaisePropertyChanged("ChildrenCount");
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::OnIsVisibleChanged()
    {
        UpdateParentVisibility();
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::UpdateParentVisibility()
    {
        auto parentPane = GetParent();
        if (parentPane)
            parentPane->ComputeVisibility();
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::ComputeVisibility()
    {
        IsVisible = GetVisibility();
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::MoveChild(int oldIndex, int newIndex)
    {
        if (oldIndex == newIndex)
            return;

        m_children.Replace(oldIndex, newIndex);
        ChildMoved(oldIndex, newIndex);
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::RemoveChildAt(int childIndex)
    {
        m_children.RemoveAt(childIndex);
    }

    template<class TElement>
    inline int LayoutGroup<TElement>::IndexOfChild(LayoutElement * element)
    {
        return m_children.IndexOf(element->shared_from_base_static<TElement>());
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::InsertChildAt(int index, LayoutElement * element)
    {
        m_children.Insert(index, element->shared_from_base_static<TElement>());
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::RemoveChild(LayoutElement * element)
    {
        m_children.Remove(element->shared_from_base_static<TElement>());
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::ReplaceChild(LayoutElement * oldElement, LayoutElement * newElement)
    {
        int index = m_children.IndexOf(oldElement->shared_from_base_static<TElement>());
        m_children.Insert(index, newElement->shared_from_base_static<TElement>());
        m_children.RemoveAt(index + 1);
    }

    template<class TElement>
    inline size_t LayoutGroup<TElement>::ChildrenCount()
    {
        return m_children.size();
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::ReplaceChildAt(int index, LayoutElement * element)
    {
        m_children[index] = element->shared_from_base_static<TElement>();
    }

    template<class TElement>
    inline void * LayoutGroup<TElement>::GetSchema()
    {
        return nullptr;
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::ReadXml(void * reader)
    {
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::WriteXml(void * writer)
    {
    }

    template<class TElement>
    inline void LayoutGroup<TElement>::ChildMoved(int oldIndex, int newIndex)
    {

    }

    template<class TElement>
    inline void LayoutGroup<TElement>::OnParentChanged(LayoutContainer * oldValue, LayoutContainer * newValue)
    {
        LayoutGroupBase::OnParentChanged(oldValue, newValue);

        ComputeVisibility();
    }

    template<class TElement>
    inline const System::Reflection::Type * LayoutGroup<TElement>::FindType(std::string const & name)
    {
        for (auto a : System::Reflection::ReflectionModulManager::Instance().GetAssemblies())
        {
            for (auto t : a->GetTypes())
            {
                if (t->name == name)
                    return t;
            }
        }
        return &System::Reflection::Type::Invalid();
    }
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_GROUP_H__