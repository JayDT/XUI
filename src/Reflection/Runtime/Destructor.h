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

#include "Invokable.h"

#include "MethodInvoker.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class TC_METARUNTIME_API Destructor
            : public NamedMethod
        {
            friend class TypeData;
        public:
            Destructor(void);
            Destructor(Type const& classType, MethodInvokerBase* invoker);
            virtual ~Destructor();

            static const Destructor &Invalid(void);

            bool IsValid(void) const;

            void Invoke(Variant &instance) const;

            // Inherited via NamedMethod
            virtual Type const & DeclaringType(void) const override;
            virtual CXXAccessSpecifier GetAccessLevel() const override;
            virtual bool CanCall(const ArgHashStorage & signature) const override;
            virtual MethodDesc const & GetMethodInfo() const override;
            virtual Variant InternalInvoke(Variant & instance, ArgumentList & arguments) const override;

            uint32_t GetUID() const override { return m_UID; }

        private:
            Destructor(Destructor const&) = delete;
            Destructor& operator=(Destructor const&) = delete;

            Type const& m_classType;
            uint32_t m_UID;

            MethodInvokerBase* m_invoker;
        };
    }
}