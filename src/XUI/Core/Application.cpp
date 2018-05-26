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

#include "Application.h"

#include "Platform/UIService.h"

#include <System/Resource/ResourceManager.h>

#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/Assembler/Settings.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"

XamlCPP::Interfaces::ResourceMap  XUI::Core::Application::m_Resources;
XamlCPP::Interfaces::ResourceMap  XUI::Core::Application::m_DataTemplates;

void LoadXamlData(std::string const& data)
{
    XamlCPP::Core::Assembler::Settings settings;
    std::ostringstream stream;
    stream << data;

    std::shared_ptr<XamlCPP::Interfaces::ILoader> xamlLoader = XamlCPP::Xaml::DefaultLoader(XUI::Platform::UIService::Instance());
    xamlLoader->Load(stream, &settings);
}

void LoadXamlResource(System::URI const& uri)
{
    System::Resource::ResourceObject* resource = System::Resource::ResourceManager::Instance()->FindResource(uri);
    if (resource)
        LoadXamlData(resource->ToString());
}

void XUI::Core::Application::InitializeDefaultResoures()
{
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultDataTemplates.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultButton.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultCheckbox.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultListbox.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultMenu.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultPopupRoot.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultRadioButton.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultScrollviewer.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultTabControl.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultTextbox.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultTreeview.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultWindow.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultContextMenu.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultSeparator.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultDropDown.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultExpander.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultCarousel.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultProgressBar.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultSlider.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultToolTip.xaml", true));
    LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultDataGrid.xaml", true));
	LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultLayoutDock_Theme.xaml", true));
	LoadXamlResource(System::URI("pack://application:,,,/XUI;/Default/DefaultLayoutDock.xaml", true));

}

System::Reflection::Variant const & XUI::Core::Application::FindResource(const std::string & name) const
{
    auto iResource = Resources.GetNativeEnumerator().find(name);
    if (iResource != Resources.GetNativeEnumerator().end())
        return iResource->second;
    
    Core::Dependency::DependencyObject* object = GetInheritanceParent();
    while (object)
    {
        auto const& value = object->FindResource(name);
        if (value.GetBase())
            return value;
    
        object = object->GetInheritanceParent();
    }
    
    return System::Reflection::Variant::Empty;
}
