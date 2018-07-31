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

#ifndef __SYSTEM_BASETYPES_H__
#define __SYSTEM_BASETYPES_H__

#include <Runtime/Object.h>
#include <Runtime/VariantBase.h>
#include <locale.h>

namespace System
{
    namespace Reflection::Assembly
    {
        class AssemblyModulCFramework;
    }

    class TC_CFRAMEWORK_API Int32 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        int value;

    public:

        Int32() : value(0) {}
        Int32(int value)
            : value(value)
        {}

        operator int()
        {
            return value;
        }

        static int Parse(const std::string& value, std::locale const& loc = std::locale());
        static bool TryParse(const std::string& value, int& result, std::locale const& loc = std::locale());
        std::string ToString() const override;

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(int))
                return false;

            return value == *(int*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API UInt32 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        unsigned int value;

    public:

        UInt32() : value(0) {}
        UInt32(unsigned int value)
            : value(value)
        {}

        operator unsigned int()
        {
            return value;
        }

        static uint Parse(const std::string& value, std::locale const& loc = std::locale());
        static bool TryParse(const std::string& value, uint& result, std::locale const& loc = std::locale());
        std::string ToString() const override
        {
            return std::to_string(value);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(unsigned int))
                return false;

            return value == *(unsigned int*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API Int64 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        long long value;

    public:

        Int64() : value(0) {}
        Int64(long long value)
            : value(value)
        {}

        operator long long()
        {
            return value;
        }

        static long long Parse(const std::string& value, std::locale const& loc = std::locale());
        static bool TryParse(const std::string& value, long long& result, std::locale const& loc = std::locale());
        std::string ToString() const override;

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(long long))
                return false;

            return value == *(long long*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API UInt64 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        unsigned long long value;

    public:

        UInt64() : value(0) {}
        UInt64(unsigned long long value)
            : value(value)
        {}

        operator unsigned long long()
        {
            return value;
        }

        std::string ToString() const override
        {
            return std::to_string(value);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(unsigned long long))
                return false;

            return value == *(unsigned long long*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API Int16 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        short value;

    public:

        Int16() : value(0) {}
        Int16(short value)
            : value(value)
        {}

        operator short()
        {
            return value;
        }

        std::string ToString() const override
        {
            return std::to_string(value);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(short))
                return false;

            return value == *(short*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API UInt16 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        unsigned short value;

    public:

        UInt16() : value(0) {}
        UInt16(unsigned short value)
            : value(value)
        {}

        operator unsigned short()
        {
            return value;
        }

        std::string ToString() const override
        {
            return std::to_string(value);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(unsigned short))
                return false;

            return value == *(unsigned short*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API Int8 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        char value;

    public:

        Int8() : value(0) {}
        Int8(char value)
            : value(value)
        {}

        operator char()
        {
            return value;
        }

        std::string ToString() const override
        {
            return std::to_string(value);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(char))
                return false;

            return value == *(char*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API UInt8 : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        unsigned char value;

    public:

        UInt8() : value(0) {}
        UInt8(unsigned char value)
            : value(value)
        {}

        operator unsigned char()
        {
            return value;
        }

        std::string ToString() const override
        {
            return std::to_string(value);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(unsigned char))
                return false;

            return value == *(unsigned char*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API Float : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        float value;

    public:

        Float() : value(0) {}
        Float(float value)
            : value(value)
        {}

        operator float()
        {
            return value;
        }

        std::string ToString() const override
        {
            return std::to_string(value);
        }

        static float Parse(const std::string& value, std::locale const& loc = std::locale());
        static bool TryParse(const std::string& value, float& result, std::locale const& loc = std::locale());
        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(float))
                return false;

            return value == *(float*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API Double : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        double value;

    public:

        Double() : value(0) {}
        Double(double value)
            : value(value)
        {}

        operator double()
        {
            return value;
        }

        static double Parse(const std::string& value, std::locale const& loc = std::locale());
        static bool TryParse(const std::string& value, double& result, std::locale const& loc = std::locale());
        std::string ToString() const override
        {
            return std::to_string(value);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(double))
                return false;

            return value == *(double*)(variant.ToPointer());
        }
    };

    class TC_CFRAMEWORK_API Bool : public Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulCFramework;

        bool value;

    public:

        Bool() : value(0) {}
        Bool(bool value)
            : value(value)
        {}

        operator bool()
        {
            return value;
        }

        static bool Parse(const std::string& value, std::locale const& loc = std::locale());
        static bool TryParse(const std::string& value, bool& result, std::locale const& loc = std::locale());
        std::string ToString() const override;

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(bool))
                return false;

            return value == *(bool*)(variant.ToPointer());
        }
    };
}

#endif // !__SYSTEM_BASETYPES_H__
