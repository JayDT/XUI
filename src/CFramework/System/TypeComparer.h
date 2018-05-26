#ifndef __SYSTEM_TYPE_COMPARER_H__
#define __SYSTEM_TYPE_COMPARER_H__

namespace System
{
    struct TC_CFRAMEWORK_API TypeCompare
    {
        static bool IsGreater(System::Reflection::Variant const& lhs, System::Reflection::Variant const& rhs);
        static bool IsLesser(System::Reflection::Variant const& lhs, System::Reflection::Variant const& rhs);
        static bool IsEqual(System::Reflection::Variant const& lhs, System::Reflection::Variant const& rhs);
    };

    struct Meta(Enable) IComparable
    {
        virtual int CompareTo(System::Reflection::Variant const& obj) = 0;
    };
}

#endif //! __SYSTEM_TYPE_COMPARER_H__