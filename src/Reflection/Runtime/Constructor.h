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

#include "standard/Platform/Define.h"

#include <memory>

#include "Variant.h"
#include "MetaContainer.h"
#include "Invokable.h"

#include "FunctionInvoker.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Variant;
        class Type;
        class Argument;

        class TC_METARUNTIME_API Constructor
            : public NamedMethod
        {
            friend class TypeData;
        public:
            Constructor(void);
            Constructor(const Constructor &rhs);
            Constructor(const Constructor &&rhs);
            
            Constructor(
                Type const& classType, 
                FunctionInvokerBase* invoker,
                bool isDynamic,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            virtual ~Constructor();

            Constructor &operator=(const Constructor &&rhs) = delete;

            static const Constructor &Invalid(void);

            virtual Type const & DeclaringType(void) const override;

            bool IsValid(void) const;
            bool IsDynamic(void) const;

            CXXAccessSpecifier GetAccessLevel() const { return AccessLevel; }

            Variant InvokeVariadic(ArgumentList &arguments) const;

            template<typename ...Args>
            Variant Invoke(Args &&...args) const;

            bool CanCall(const ArgHashStorage & signature) const;
			bool CanCall(const InvokableSignature &signature) const override;

            // Inherited via NamedMethod
            virtual MethodDesc const & GetMethodInfo() const override;
            virtual Variant InternalInvoke(Variant & instance, ArgumentList & arguments) const override;

            uint32_t GetUID() const override { return m_UID; }

            ArgHashStorage const& GetParameters() const;

        private:

            bool m_isDynamic;
            CXXAccessSpecifier AccessLevel : 8;
            uint32_t m_UID;

            Type const& m_classType;
            FunctionInvokerBase* m_invoker;
        };
    }
}

#include "Impl/Constructor.hpp"