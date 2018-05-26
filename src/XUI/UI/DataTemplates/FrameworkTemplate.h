/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#ifndef __XUI_UI_DATATEMPLATES_FRAMEWORKTEMPLATE_H__
#define __XUI_UI_DATATEMPLATES_FRAMEWORKTEMPLATE_H__

#include "standard/misc.h"
#include "standard/Errors.h"
#include "UI/Controls/NameScope.h"
#include "Core/Dependencies/DependencyObject.h"
#include "XamlCPP/Interfaces/ITemplate.h"
#include "XamlCPP/Interfaces/IResourceMap.h"
#include "XamlCPP/Interfaces/ITriggerBase.h"
#include "XamlCPP/Interfaces/IDeferredLoader.h"
#include "XamlCPP/Interfaces/IInstanceLifeCycleListener.h"

#include <functional>

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/ContentControl.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class Control;
}

namespace XUI::UI::DataTemplates
{
    struct ContentTemplate : System::Reflection::Object
    {
        /// <summary>
        /// Gets a value indicating whether the data template supports recycling of the generated
        /// control.
        /// </summary>
        virtual bool SupportsRecycling() = 0;

        /// <summary>
        /// Creates the control.
        /// </summary>
        /// <param name="param">The parameter.</param>
        /// <returns>
        /// The created control.
        /// </returns>
        virtual System::Reflection::RefObject Build(System::Reflection::meta::Variant const& param) = 0;
    };

    struct TC_XUI_API FrameworkTemplate : Core::Dependency::DependencyObject, XamlCPP::Interfaces::ITemplate
    {
        FrameworkTemplate() {}
        virtual ~FrameworkTemplate() {}

        PROPERTY_AUTO_QT(bool, IsSealed)

        PROPERTY_QT(std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>, ContentTemplate)
        std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> GetContentTemplate() const { return m_ContentTemplate; }
        void SetContentTemplate(std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> const& value) { m_ContentTemplate = value; }

        /// <summary>
        /// Gets or sets a dictionary of style resources.
        /// </summary>
        PROPERTY_(XamlCPP::Interfaces::ResourceMap, Resources);
        XamlCPP::Interfaces::ResourceMap const& GetResources() const
        {
            return m_resources;
        }
        void SetResources(const XamlCPP::Interfaces::ResourceMap &value)
        {
            for (auto i : value.GetNativeEnumerator())
            {
                m_resources.Add(i.first, i.second);
            }
        }

        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader>, XamlTemplate);
        std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader> GetXamlTemplate() const { return m_xamlTemplate; }
        void SetXamlTemplate(std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader> const& value)
        {
            m_xamlTemplate = value;
            m_ContentTemplate = CreateContentTemplate();
        }

        bool HasContent()
        {
            return m_xamlTemplate && m_ContentTemplate;
        }

        // Inherited via INameScope

        /// <summary>
        /// Finds a named element in the name scope.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <returns>The element, or null if the name was not found.</returns>
        System::Reflection::RefObject FindName(std::string const& name, UI::Controls::Control* templatedParent);

        template<typename TType>
        std::shared_ptr<TType> FindNameByManagedType(std::string const& name, UI::Controls::Control* templatedParent)
        {
            System::Reflection::RefObject object = FindName(name, templatedParent);
            if (object && object->Is<TType>())
                return std::static_pointer_cast<TType>(object);
            return std::shared_ptr<TType>();
        }

    protected:
        virtual std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> CreateContentTemplate() = 0;
        
        std::map<UI::Controls::Control*, std::shared_ptr<UI::Controls::NameScope>> _namedScopes;

    private:

        std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader> m_xamlTemplate;
        XamlCPP::Interfaces::ResourceMap m_resources;
    };

    /// <summary>
    /// Creates a control from a <see cref="Func{TParam, TControl}"/>.
    /// </summary>
    /// <typeparam name="TParam">The type of the parameter.</typeparam>
    /// <typeparam name="TControl">The type of control.</typeparam>
    template<class TControl>
    class Meta(AbstractType) FuncTemplate : public XamlCPP::Interfaces::ITemplate
    {
    public:
        typedef std::function<TControl()> FuncHandler;
    private:
        Meta(Disable) FuncHandler _func;
    
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="FuncTemplate{TControl, TParam}"/> class.
        /// </summary>
        /// <param name="func">The function used to create the control.</param>
        FuncTemplate(FuncHandler const& func)
            : _func(std::move(func))
        {
            System::ThrowIfFailed<System::ArgumentNullException>(func != nullptr);
        }
    
        virtual ~FuncTemplate() {}
    
        /// <summary>
        /// Creates the control.
        /// </summary>
        /// <param name="param">The parameter.</param>
        /// <returns>
        /// The created control.
        /// </returns>
        TControl Create()
        {
            return _func();
        }

        System::Reflection::RefObject Build(XamlCPP::Interfaces::IInstanceLifeCycleListener*) override
        {
            return std::static_pointer_cast<System::Reflection::Object>(Create());
        }
    };
}
#endif // !__XUI_UI_DATATEMPLATES_FRAMEWORKTEMPLATE_H__

