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
        ///////////////////////////////////////////////////////////////////////
        // Setter from Method
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalSetter<GlobalType, true> : public GlobalSetterBase
        {
        public:
            typedef GlobalType(*Signature)(const GlobalType &);

            GlobalSetter(Signature function)
                : m_function( function ) { }

            void SetValue(const Argument &value) override
            {
                m_function( value.GetValue<GlobalType>( ) );
            }

        private:
            Signature m_function;
        };

        ///////////////////////////////////////////////////////////////////////
        // Setter from Direct Field
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalSetter<GlobalType, false> : public GlobalSetterBase
        {
        public:
            GlobalSetter(GlobalType *field)
                : m_field( field ) { }

            void SetValue(const Argument &value) override
            {
                const_cast<GlobalType&>( *m_field ) = value.GetValue<GlobalType>( );
            }

        private:
            GlobalType *m_field;
        };
    }
}