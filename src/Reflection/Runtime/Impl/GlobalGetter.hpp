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
        // Getter from Method
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalGetter<GlobalType, true> : public GlobalGetterBase
        {
        public:
            typedef GlobalType (*Signature)(void);

            GlobalGetter(Signature function)
                : m_function( function ) { }

            Variant GetValue(void) override
            {
                return m_function( );
            }

        private:
            Signature m_function;
        };

        ///////////////////////////////////////////////////////////////////////
        // Getter from Direct Global
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalGetter<GlobalType, false> : public GlobalGetterBase
        {
        public:
            GlobalGetter(GlobalType *global)
                : m_global( global ) { }

            Variant GetValue(void) override
            {
                return *m_global;
            }

        private:
            GlobalType *m_global;
        };
    }
}