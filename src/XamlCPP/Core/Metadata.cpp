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

#include "Metadata.h"
#include "DependencyRegistration.h"

XamlCPP::Core::Metadata::Metadata()
{
}

bool XamlCPP::Core::Metadata::IsEmpty()
{
    return PropertyDependencies.empty() &&
        RuntimePropertyName.empty() &&
        ContentProperty.empty();
}

void XamlCPP::Core::Metadata::SetMemberDependency(std::string const & property, std::string const & dependsOn)
{
    PropertyDependencies.push_back(ReflEx::DependencyRegistration(property, dependsOn));
}

std::vector<std::string> XamlCPP::Core::Metadata::GetMemberDependencies(std::string const & name)
{
    std::vector<std::string> result;

    for (auto const& dep : PropertyDependencies)
    {
        if (dep.getProperty() == name)
            result.push_back(dep.getDependsOn());
    }

    return result;
}

bool XamlCPP::Core::Metadata::operator==(Metadata const& other) const
{
    if (RuntimePropertyName == RuntimePropertyName && PropertyDependencies.size() == other.PropertyDependencies.size())
    {
        auto iSelf = PropertyDependencies.begin();
        auto iOther = other.PropertyDependencies.begin();

        for (auto const& rhs : other.PropertyDependencies)
        {
            if (std::find(PropertyDependencies.begin(), PropertyDependencies.end(), rhs) == PropertyDependencies.end())
                return false;
        }

        return true;
    }
    return false;
}
