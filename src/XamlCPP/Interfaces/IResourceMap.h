/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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

#pragma once

#include <System/System.h>
#include <standard/collection.h>
#include <standard/events.h>

namespace XamlCPP::Core::Dependency
{
    class IDependencyObject;
}

namespace XamlCPP::Interfaces
{
    struct ResourceMap;

	struct ResourceChangedEventArgs : System::Events::EventArg
	{
		std::string ResourceKey;
		bool Removed = false;
	};

    struct Meta(Enable) IResourceNode
    {
        System::Events::EventHandler<ResourceChangedEventArgs> ResourcesChanged;

        virtual ~IResourceNode() {}
        virtual IResourceNode* GetResourceParent() const = 0;
        virtual XamlCPP::Interfaces::ResourceMap const& GetResources() const = 0;
        virtual XamlCPP::Core::Dependency::IDependencyObject* ToIDependencyObject2() = 0;
        virtual System::Reflection::Variant const& FindResource(const std::string &name) const = 0;

        IResourceNode* ToIResourceNode() { return this; } // instead dynamic cast
    };

    struct ResourceMap : public System::Collection::HashMap<std::string, System::Reflection::Variant>
    {
        ResourceMap()
        { }

        ResourceMap(ResourceMap const& from)
            : System::Collection::HashMap<std::string, System::Reflection::Variant>(from)
        { }

        ResourceMap(ResourceMap const&& from)
            : System::Collection::HashMap<std::string, System::Reflection::Variant>(std::move(from))
        { }

        void operator=(ResourceMap const& from)
        {
            Clear();
            for (auto const& v : from.GetNativeEnumerator())
            {
                Add(v.first, v.second);
            }
        }
    };
}
