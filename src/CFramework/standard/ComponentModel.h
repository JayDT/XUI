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

#ifndef __COMPONENTMODEL_H__
#define __COMPONENTMODEL_H__

#include <standard/BasicPrimities.h>
#include <standard/events.h>

namespace System::ComponentModel
{
    class PropertyChangedEventArgs : public System::Events::EventArg
    {
        std::string _propertyName;
    public:
        PropertyChangedEventArgs(std::string propertyName) : _propertyName(propertyName)
        {

        }

        AUTO_PROPERTY(_propertyName, PropertyName, virtual);
    };

    struct INotifyPropertyChanged
    {
        typedef Events::EventHandler<PropertyChangedEventArgs> PropertyChangedEventHandler;
        PropertyChangedEventHandler PropertyChanged;
    };
}
#endif
