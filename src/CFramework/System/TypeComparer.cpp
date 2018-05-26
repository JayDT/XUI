#include "StdAfx.h"
#include "TypeComparer.h"

bool System::TypeCompare::IsGreater(System::Reflection::Variant const & lhs, System::Reflection::Variant const & rhs)
{
    if (lhs.GetType()->IsPrimitive() && rhs.GetType()->IsPrimitive())
    {
        if (lhs.GetType()->IsFloatingPoint())
            return lhs.ToDouble() > rhs.ToDouble();
        else if (*lhs.GetType() == typeid(unsigned long long))
            return *(unsigned long long*)(lhs.toPointer()) > *(unsigned long long*)(rhs.toPointer());
        else
            return lhs.ToLong() > rhs.ToLong();
    }
    else if (lhs.GetType() == rhs.GetType())
    {
        return lhs.ToString() > rhs.ToString();
    }

    //ToDo: need implement more cases (and Object part)
    throw System::NotImplementedException();
}

bool System::TypeCompare::IsLesser(System::Reflection::Variant const & lhs, System::Reflection::Variant const & rhs)
{
    if (lhs.GetType()->IsPrimitive() && rhs.GetType()->IsPrimitive())
    {
        if (lhs.GetType()->IsFloatingPoint())
            return lhs.ToDouble() < rhs.ToDouble();
        else if (*lhs.GetType() == typeid(unsigned long long))
            return *(unsigned long long*)(lhs.toPointer()) < *(unsigned long long*)(rhs.toPointer());
        else
            return lhs.ToLong() < rhs.ToLong();
    }
    else if (lhs.GetType() == rhs.GetType())
    {
        return lhs.ToString() < rhs.ToString();
    }

    //ToDo: need implement more cases (and Object part)
    throw System::NotImplementedException();
}

bool System::TypeCompare::IsEqual(System::Reflection::Variant const & lhs, System::Reflection::Variant const & rhs)
{
    if (lhs.GetType()->IsPrimitive() && rhs.GetType()->IsPrimitive())
    {
        if (lhs.GetType()->IsFloatingPoint())
            return lhs.ToDouble() == rhs.ToDouble();
        else if (*lhs.GetType() == typeid(unsigned long long))
            return *(unsigned long long*)(lhs.toPointer()) == *(unsigned long long*)(rhs.toPointer());
        else
            return lhs.ToLong() == rhs.ToLong();
    }

    return lhs.IsFastEqual(rhs);
}
