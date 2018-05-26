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

#include "Common/Logging.h"

namespace System::Reflection
{
    inline namespace meta
    {
        struct InvalidOperationFieldSetter
        {

        };

        template<typename FieldType>
        class FieldValue<FieldType, true> : public FieldSetterBase, public FieldGetterBase
        {
        public:
            FieldValue(uint32 _offset)
                : offset(_offset), m_fieldType(System::Reflection::type_of(typeid(FieldType))) { }

            void SetValue(uint32 uid, Variant &obj, const Variant &value) override
            {
                UAssert(value.IsValid(), "Setting invalid value.");

                *(std::remove_const<FieldType>::type*)((char*)(obj.getPtr()) + offset) = *static_cast<FieldType*>(value.getPtr());
            }

            Variant GetValue(uint32 uid, const Variant &obj) override
            {
                return Variant(m_fieldType, (void*)((char*)(obj.getPtr()) + offset), (variant_policy::NoCopy*)nullptr);
            }

            Variant GetValueReference(uint32 uid, const Variant &obj) override
            {
                return Variant(m_fieldType, (void*)((char*)(obj.getPtr()) + offset), (variant_policy::NoCopy*)nullptr);
            }

        private:
            uint32 offset;
            Type const& m_fieldType;
        };

        template<typename FieldType>
        class FieldValue<FieldType, false> : public FieldSetterBase, public FieldGetterBase
        {
        public:
            FieldValue(uint32 _offset)
                : offset(_offset), m_fieldType(System::Reflection::type_of(typeid(FieldType))) { }

            void SetValue(uint32 uid, Variant &obj, const Variant &value) override
            {
            }

            Variant GetValue(uint32 uid, const Variant &obj) override
            {
                return Variant(m_fieldType, (void*)((char*)(obj.getPtr()) + offset), (variant_policy::NoCopy*)nullptr);
            }

            Variant GetValueReference(uint32 uid, const Variant &obj) override
            {
                return Variant(m_fieldType, (void*)((char*)(obj.getPtr()) + offset), (variant_policy::NoCopy*)nullptr);
            }

        private:
            uint32 offset;
            Type const& m_fieldType;
        };

        ///////////////////////////////////////////////////////////////////////
        // Setter from Method
        ///////////////////////////////////////////////////////////////////////

        class PropertySetter : public FieldSetterBase
        {
        public:
            typedef void invoker_impl(size_t methodId, Variant&& obj, Variant&& value);

            invoker_impl* invoker = nullptr;
			bool isStatic = false;

            PropertySetter(invoker_impl method)
                : invoker( method ) { }

            void SetValue(uint32 uid, Variant &obj, const Variant &value) override
            {
                UAssert( value.IsValid( ), "Setting invalid value." );
                invoker(uid, std::move(const_cast<Variant &>(obj)), std::move(const_cast<Variant &>(value)));
            }

			bool IsStatic() const override { return isStatic; }
        };


        ///////////////////////////////////////////////////////////////////////
        // Setter from Method
        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ArgumentType, typename ReturnType>
        class PropertyValue<ClassType, ArgumentType, ReturnType, false> : public FieldSetterBase, public FieldGetterBase
        {
        public:
            typedef typename std::remove_reference<ArgumentType>::type NonReferenceArgType;
            typedef void (ClassType::*Signature)(ArgumentType);
            typedef void (ClassType::*SignatureConst)(ArgumentType) const;
            typedef ReturnType(ClassType::*Get_Signature)(void);
            typedef ReturnType(ClassType::*Get_SignatureConst)(void) const;

            PropertyValue(Signature method, Get_Signature getmethod)
                : m_method(method), m_get_method(getmethod) { }

            PropertyValue(SignatureConst method, Get_SignatureConst getmethod)
                : m_method(reinterpret_cast<Signature>(method)), m_get_method(reinterpret_cast<Get_Signature>(getmethod)) { }

            void SetValue(uint32 uid, Variant &obj, const Variant &value) override
            {
                UAssert(value.IsValid(), "Setting invalid value.");

                throw InvalidOperationFieldSetter();
            }

            Variant GetValue(uint32 uid, const Variant &obj) override
            {
                auto &instance = *(ClassType*)(obj.getPtr()); // .GetValue<ClassType>();

                auto __return_value = (instance.*m_get_method)();

                static m::Type const& returnType = System::Reflection::type_of(typeid(__return_value));
                return m::Variant(returnType, &__return_value, sizeof(__return_value));
            }

            Variant GetValueReference(uint32 uid, const Variant &obj) override
            {
                return getValueReference(obj);
            }

        private:
            template<typename T = ReturnType>
            Variant getValueReference(
                const Variant &obj,
                typename std::enable_if<
                std::is_lvalue_reference<T>::value
                >::type* = nullptr
            )
            {
                auto &instance = *(ClassType*)(obj.getPtr()); //obj.GetValue<ClassType>( );

                auto& __return_value = (instance.*m_get_method)();

                static m::Type const& returnType = System::Reflection::type_of(typeid(__return_value));
                return m::Variant(returnType, (void*)&__return_value, (m::variant_policy::NoCopy*)nullptr);
            }

            template<typename T = ReturnType>
            Variant getValueReference(
                const Variant &obj,
                typename std::enable_if<
                !std::is_lvalue_reference<T>::value
                >::type* = nullptr
            )
            {
                // non l-value reference return types must return by value
                return GetValue(obj);
            }

        private:
            Signature m_method;
            Get_Signature m_get_method;
        };
    }
}