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

#include "Precompiled.h"

#include "Variant.h"

#include "Type.h"
#include "Object.h"
#include "VariantContainer.h"
#include "ArrayWrapper.h"
#include "ArrayVariantContainer.h"

namespace System::Reflection
{
    using Json = json11::Json;
}

namespace System::Reflection
{
    inline namespace meta
    {
        Variant Variant::Empty = Variant();

        Variant::Variant(void)
            : m_isConst( true )
            , m_base( nullptr ) { }

        Variant::Variant(Object const& instance, variant_policy::NoCopy*)
            : m_isConst(true)
            , m_base(new VariantRefObject(*instance.GetType(), (Object*)std::addressof(instance), true, false))
        {
        }

        Variant::Variant(Object & instance, variant_policy::NoCopy*)
            : m_isConst(false)
            , m_base(new VariantRefObject(*instance.GetType(), std::addressof(instance), true, false))
        {
        }

        Variant::Variant(Object*& instance, variant_policy::NoCopy*)
            : m_isConst(false)
            , m_base(new VariantRefObject(instance ? *instance->GetType() : Type::Invalid(), std::addressof(instance), true, true))
        {
        }

        Variant::Variant(Object const* const& instance, variant_policy::NoCopy*)
            : m_isConst(false)
            , m_base(new VariantRefObject(instance ? *instance->GetType() : Type::Invalid(), (void*)std::addressof(instance), true, true))
        {
        }

        Variant::Variant(Object * const & instance, variant_policy::Copy*)
            : m_isConst(true)
            , m_base(new VariantCopyObject<Object *>(instance ? *instance->GetType() : Type::Invalid(), instance, sizeof(Object*), true, true))
        {
        }

        Variant::Variant(Object const* const & instance, variant_policy::Copy*)
            : m_isConst(true)
            , m_base(new VariantCopyObject<Object const*>(instance ? *instance->GetType() : Type::Invalid(), instance, sizeof(Object*), true, true))
        {
        }

        Variant::Variant(ArrayWrapper const& _array)
            : m_isConst(true)
            , m_base(new ArrayVariantContainer(_array.GetType(), _array))
        {
        }

        Variant::Variant(const Variant &rhs)
            : m_isConst( rhs.m_isConst )
            , m_nativetypeHash( rhs.m_nativetypeHash)
            , m_base( rhs.m_base ? rhs.m_base->Clone( ) : nullptr )
        {
        }

        Variant::Variant(Variant &&rhs)
            : m_isConst( rhs.m_isConst )
            , m_nativetypeHash( rhs.m_nativetypeHash )
            , m_base( rhs.m_base )
        {
            rhs.m_isConst = true;
            rhs.m_nativetypeHash = 0;
            rhs.m_base = nullptr;
        }

        Variant::~Variant(void)
        {
            if (m_base)
                delete m_base;
        }

        Variant &Variant::operator=(const Variant &rhs)
        {
            //Note: Make Clone from box
            Variant( rhs ).Swap( *this );

            return *this;
        }

        Variant &Variant::operator=(Variant &&rhs)
        {
            rhs.Swap( *this );

            Variant( ).Swap( rhs );

            return *this;
        }

        bool Variant::operator==(const Variant &rhs) const
        {
            return m_base == rhs.m_base || (m_base && rhs.m_base && m_base->Equal(*rhs.m_base));
        }

        bool Variant::operator!=(const Variant &rhs) const
        {
            if ((m_base != nullptr) != (rhs.m_base != nullptr))
                return true;

            return (m_base && rhs.m_base && !m_base->Equal(*rhs.m_base));
        }

        Type const* Variant::GetType(void) const
        {
            return m_base ? &m_base->GetType( ) : &Type::Invalid( );
        }

        //ArrayWrapper Variant::GetArray(void) const
        //{
        //    return m_base ? m_base->GetArray( ) : ArrayWrapper( );
        //}

        Object * Variant::ToObject(void) const
        {
            return m_base ? m_base->ToObject() : nullptr;
        }

        std::shared_ptr<void> Variant::ToSharedPointer(void) const
        {
            return m_base ? m_base->ToSharedPointer() : nullptr;
        }

        std::shared_ptr<Object> Variant::ToSharedObject(void) const
        {
            return m_base ? m_base->ToSharedObject() : nullptr;
        }

        int Variant::ToInt(void) const
        {
            return m_base ? m_base->ToInt( ) : int( );
        }

        bool Variant::ToBool(void) const
        {
            return m_base ? m_base->ToBool( ) : bool( );
        }

        float Variant::ToFloat(void) const
        {
            return m_base ? m_base->ToFloat( ) : float( );
        }

        double Variant::ToDouble(void) const
        {
            return m_base ? m_base->ToDouble( ) : double( );
        }

        long long Variant::ToLong(void) const
        {
            return m_base ? m_base->ToLong() : double();
        }

        std::string Variant::ToString(void) const
        {
            return m_base ? m_base->ToString( ) : std::string( );
        }

        void* Variant::ToPtr(void) const
        {
            return m_base->GetPtr( );
        }

        ArrayWrapper* Variant::GetArray(void) const
        {
            return m_base ? m_base->GetArray() : nullptr;
        }

        Json Variant::SerializeJson(void) const
        {
            return GetType() ? GetType( )->SerializeJson( *this ) : Json();
        }

        void Variant::Swap(Variant &other)
        {
            std::swap( m_isConst, other.m_isConst );
            std::swap( m_nativetypeHash, other.m_nativetypeHash );
            std::swap( m_base, other.m_base );
        }

        bool Variant::IsValid(void) const
        {
            return m_base != nullptr;
        }

        bool Variant::IsConst(void) const
        {
            return m_isConst;
        }

        bool Variant::IsArray(void) const
        {
            return m_base ? m_base->IsArray( ) : false;
        }

        bool Variant::IsPointer(void) const
        {
            return m_base ? m_base->IsPointer() : false;
        }

        bool Variant::IsObject(void) const
        {
            return m_base ? m_base->IsObject() : false;
        }

        void *Variant::getPtr(void) const
        {
            return m_base ? m_base->GetPtr( ) : nullptr;
        }

        void * Variant::toPointer(void) const
        {
            return m_base ? m_base->ToPointer() : nullptr;
        }

        bool Variant::IsFastEqual(Variant const & rhs) const
        {
            return *this == rhs;
        }

        std::size_t Variant::GetNativeTypeHash() const
        {
            return GetType() && GetType()->IsValid() ? GetType()->GetHashCode() : m_nativetypeHash;
        }

        bool Variant::HasType(void) const
        {
            return Valid() && GetType()->IsValid();
        }
    }
}
