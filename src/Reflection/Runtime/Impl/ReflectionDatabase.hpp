/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
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

namespace System::Reflection
{
    inline namespace meta
    {
        template<typename GlobalType, typename GetterType, typename SetterType>
        void ReflectionDatabase::AddGlobal(const std::string &name, GetterType getter, SetterType setter, const MetaManager::Initializer &meta)
        {
            typedef GlobalGetter<GlobalType, true> GlobalGetterType;
            typedef GlobalSetter<GlobalType, true> GlobalSetterType;

            Global* global = new Global({
                name,
                typeof( GlobalType ),
                !getter ? nullptr : new GlobalGetterType(
                    reinterpret_cast<typename GlobalGetterType::Signature>( getter )
                ),
                !setter ? nullptr : new GlobalSetterType(
                    reinterpret_cast<typename GlobalSetterType::Signature>( setter )
                )
            });

            global->m_meta = meta;

            globals.emplace( name, global );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType, typename GetterType>
        void ReflectionDatabase::AddGlobal(const std::string &name, GetterType getter, GlobalType *globalSetter, const MetaManager::Initializer &meta)
        {
            typedef GlobalGetter<GlobalType, true> GlobalGetterType;
            typedef GlobalSetter<GlobalType, false> GlobalSetterType;

            Global* global = new Global({
                name,
                typeof( GlobalType ),
                !getter ? nullptr : new GlobalGetterType(
                    reinterpret_cast<typename GlobalGetterType::Signature>( getter )
                ),
                !globalSetter ? nullptr : new GlobalSetterType( globalSetter )
            });

            global->m_meta = meta;

            globals.emplace( name, global );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType, typename SetterType>
        void ReflectionDatabase::AddGlobal(const std::string &name, GlobalType *globalGetter, SetterType setter, const MetaManager::Initializer &meta)
        {
            typedef GlobalGetter<GlobalType, false> GlobalGetterType;
            typedef GlobalSetter<GlobalType, true> GlobalSetterType;

            Global* global = new Global({
                name,
                typeof( GlobalType ),
                !globalGetter ? nullptr : new GlobalGetterType( globalGetter ),
                !setter ? nullptr : new GlobalSetterType(
                    reinterpret_cast<typename GlobalSetterType::Signature>( setter )
                )
            });

            global.m_meta = meta;

            globals.emplace( name, global );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        void ReflectionDatabase::AddGlobal(const std::string &name, GlobalType *globalGetter, GlobalType *globalSetter, const MetaManager::Initializer &meta)
        {
            typedef GlobalGetter<GlobalType, false> GlobalGetterType;
            typedef GlobalSetter<GlobalType, false> GlobalSetterType;

            Global* global = new Global({
                name,
                typeof( GlobalType ),
                !globalGetter ? nullptr : new GlobalGetterType( globalGetter ),
                !globalSetter ? nullptr : new GlobalSetterType( globalSetter )
            });

            global.m_meta = meta;

            globals.emplace( name, global );
        }
    }
}
