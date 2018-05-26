#include "Precompiled.h"
#include "FieldGetterBase.h"
#include "Field.h"
#include "Argument.h"

using namespace System::Reflection;
using namespace System::Reflection::meta;

Type const & System::Reflection::meta::FieldGetterBase::DeclaringType(void) const
{
    return m_declaringField ? m_declaringField->DeclaringType() : Type::Invalid();
}

const char * System::Reflection::meta::FieldGetterBase::GetName(void) const
{
    return m_declaringField ? m_declaringField->GetName() : "";
}

CXXAccessSpecifier System::Reflection::meta::FieldGetterBase::GetAccessLevel() const
{
    return m_declaringField ? m_declaringField->GetAccessLevel() : CXXAccessSpecifier::Public;
}

bool System::Reflection::meta::FieldGetterBase::CanCall(const ArgHashStorage & signature) const
{
    return signature.size() == 1;
}

bool System::Reflection::meta::FieldGetterBase::IsValid(void) const
{
    return true;
}

MethodDesc const & System::Reflection::meta::FieldGetterBase::GetMethodInfo() const
{
    static MethodDesc default(
        0,
        DeclaringType(),
        m_declaringField ? *m_declaringField->GetFieldType() : Type::Invalid(),
        MethodDesc::MethodType::NONE,
        {});
    return default;
}

Variant System::Reflection::meta::FieldGetterBase::InternalInvoke(Variant & instance, ArgumentList & arguments) const
{
    return const_cast<FieldGetterBase*>(this)->GetValue(m_declaringField->GetUID(), instance);
}

uint32_t System::Reflection::meta::FieldGetterBase::GetUID() const
{
    return m_declaringField ? m_declaringField->GetUID() : 0;
}
