#include "FieldBase.h"
#include "MethodBase.h"
#include "clang-c/Index.h"
#include "LanguageTypes/Class.h"
#include "LanguageTypes/Field.h"
#include "Runtime/MetaProperty.h"
#include "MetaDataConfig.h"
#include "MetaUtils.h"
#include "ReflectionParser.h"

extern unsigned long internalID;

FieldBase::FieldBase(const Cursor & cursor, const Namespace & currentNamespace, Class * parent)
    : LanguageType(cursor, currentNamespace)
    , m_isConst(cursor.GetType().IsConst())
    , m_isConstArray(false)
    , m_isConstSubType(false)
    , m_isTemplate(false)
    , m_isReference(false)
    , m_isPointer(false)
    , m_isStatic(cursor.IsStatic())
    , m_parent(parent)
    , m_name(cursor.GetSpelling())
    , m_fieldType(cursor.GetType().GetCanonicalType())
    , m_fieldAstType(AstType::Create(cursor.GetType().GetCanonicalType()))
    , m_type(::utils::GetQualifiedName(m_fieldType))
{
}

void FieldBase::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    LanguageType::MetaInit(cursor, currentNamespace);

    auto displayName = m_metaData.GetNativeString(native_attribute::DisplayName);

	//if (m_name == "Model")
	//{
	//	m_type = (::utils::GetQualifiedName(m_fieldType));
	//	printf("");
	//}

    switch (m_fieldType.GetKind())
    {
        case CXType_Pointer:
            m_isPointer = true;
            break;
        case CXType_LValueReference:
        case CXType_RValueReference:
            m_isReference = true;
            break;
    }

    size_t pos = m_type.find('<');
    if (pos != std::string::npos)
    {
        //std::string type = m_fieldType.GetElaboratedType().GetDisplayName();

        if (m_parent && m_parent->IsTemplate())
        {
            m_isTemplate = true;
        }

        if (m_fieldType.GetDeclaration().GetKind() == CXCursorKind::CXCursor_NoDeclFound)
        {
            if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(m_fieldAstType->GetDeclarationHash()))
                klass->DeclareTemplate(*m_fieldAstType, &cursor);
        }
        else if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(m_fieldType.GetDeclaration()))
            klass->DeclareTemplate(*m_fieldAstType, &cursor);
    }

    m_constArraySize = 0;
    if (cursor.GetType().GetKind() == CXType_ConstantArray)
    {
        for (auto& child : cursor.GetChildren())
        {
            if (child.GetKind() < CXType_LastBuiltin)
            {
                m_subType = utils::GetQualifiedName(child.GetType());
                m_isConstSubType = child.GetType().IsConst();
            }
        }

        m_type.resize(m_type.size() - 1);
        System::String type = m_type;
        std::vector<std::string> tokens = type.Split('[');

        m_isConstArray = true;
        if (m_parent)
            m_type = "std::remove_all_extents<decltype(" + m_parent->m_qualifiedName + "::" + m_name + ")>::type";
        else
            m_type = "std::remove_all_extents<decltype(" + m_name + ")>::type";
        m_constArraySize = atoi(tokens[1].c_str());

        m_metaData.AddProperty("System::ConstArrayType", System::String::format("typeid(%s).hash_code(), %d", m_type.c_str(), m_constArraySize));
    }

    if (displayName.empty())
        m_displayName = m_name;
    else
        m_displayName = displayName;

    m_isProperty = m_metaData.GetFlag(native_attribute::PropertyType);
    m_Getter = m_metaData.GetNativeString(native_attribute::PropertyGetter);
    m_ExplicitGetter = m_metaData.GetNativeString(native_attribute::ExplicitGetter);
    m_hasExplicitGetter = !m_Getter.empty() || !m_ExplicitGetter.empty();

    m_Setter = m_metaData.GetNativeString(native_attribute::PropertySetter);
    m_ExplicitSetter = m_metaData.GetNativeString(native_attribute::ExplicitSetter);
    m_hasExplicitSetter = !m_Setter.empty() || !m_ExplicitSetter.empty();

	if (IsProperty())
	{
		m_parent->ResolvePropertyMethod(m_Getter, static_cast<Field*>(this));
		m_parent->ResolvePropertyMethod(m_Setter, static_cast<Field*>(this));
	}

    //unsigned line, column, offset;
    //cursor.GetSourceFileInfo(line, column, offset);
}

Mustache::Data<std::string> FieldBase::CompileStandardTemplate(Mustache::Data<std::string>& data, const ReflectionParser *context) const
{
    data["internalID"] = std::to_string(++internalID);
    data["name"] = m_name;
    data["displayName"] = m_displayName;
    data["type"] = m_type;
    data["AccessLevel"] = std::to_string(int(m_accessModifier));

    data["isTypePointer"] = utils::TemplateBool(m_isPointer);

    bool unmanagedClassOrNonSharedPtr = true;
    //auto declAstType = m_fieldAstType->GetDeclarationType();
    //if (declAstType->GetKind() == CXTypeKind::CXType_Invalid)
    //    declAstType = m_fieldAstType->GetElaboratedType();
    auto FieldTypeClass = m_fieldAstType->FindClassLangType(); // declAstType->GetClassLangType();
    if (FieldTypeClass && FieldTypeClass->IsTemplate() && FieldTypeClass->GetDisplayName() == "shared_ptr")
    {
        data["isTypeReflPointer"] = utils::TemplateBool(true);
        for (auto defClass : FieldTypeClass->m_definations)
        {
            if (m_fieldAstType->GetQualifiedName() == defClass->m_qualifiedName)
            {
                // Special case for std::shared_ptr
                if (!defClass->m_templateTypes.empty())
                {
                    auto templateArgClass = defClass->m_templateTypes[0]->FindClassLangType().get(); //GetClassLangType().get();

                    if (!templateArgClass)
                    {
                        auto hash = defClass->m_templateTypes[0]->GetDeclarationHash();
                        if (defClass->m_templateTypes[0]->GetElaboratedType())
                            hash = defClass->m_templateTypes[0]->GetElaboratedType()->GetDeclarationHash();

                        templateArgClass = ReflectionModulParser::Instance->GetClassDesc(hash);
                    }

                    if (templateArgClass)
                    {
                        // Note: This class not part of assembly but required init for correct typeinfo
                        if (!templateArgClass->IsPreparedForAssembly())
                            templateArgClass->PrepareForAssembly();
                        data["isTypeReflObject"] = utils::TemplateBool(templateArgClass->IsReflObject());
                        data["isTypeDynamicCast"] = utils::TemplateBool(templateArgClass->HasVirtualBase());

                        unmanagedClassOrNonSharedPtr = !templateArgClass->IsReflObject();
                        break;
                    }
                }
            }
        }
    }

    if (unmanagedClassOrNonSharedPtr)
    {
        data["isTypeReflPointer"] = utils::TemplateBool(false);
        data["isTypeReflObject"] = utils::TemplateBool(FieldTypeClass ? FieldTypeClass->IsReflObject() : false);
        data["isTypeDynamicCast"] = utils::TemplateBool(FieldTypeClass ? FieldTypeClass->HasVirtualBase() : false);
    }

    data["hasParent"] = utils::TemplateBool(!!m_parent);
    data["isReferenceValue"] = utils::TemplateBool(m_isReference);
    data["isReflObject"] = utils::TemplateBool(m_parent && m_parent->IsReflObject());
    data["isDynamicCast"] = utils::TemplateBool(m_parent && m_parent->HasVirtualBase());

    if (m_parent)
        data["parentQualifiedName"] = m_parent->m_qualifiedName;

    // getter
    data["isGetterAccessible"] = utils::TemplateBool(isGetterAccessible());
    data["hasExplicitGetter"] = utils::TemplateBool(m_hasExplicitGetter);

    if (isGetterAccessible())
    {
        std::string explicitGetter;

        if (m_parent && m_ExplicitGetter.empty())
        {
            explicitGetter = m_Getter;
        }
        else
        {
            explicitGetter = m_ExplicitGetter;
        }

        data["explicitGetter"] = explicitGetter;

    }

	data["isStaticGetter"] = _getter_method && _getter_method->IsStatic() ? "true" : "false";
	data["getterBody"] = context->LoadTemplatePartial(kPartialFieldGetter);

    // setter

    data["isSetterAccessible"] = utils::TemplateBool(isSetterAccessible());
    data["hasExplicitSetter"] = utils::TemplateBool(m_hasExplicitSetter);

    if (isSetterAccessible())
    {
        std::string explicitSetter;

        if (m_parent && m_ExplicitSetter.empty())
        {
            explicitSetter = m_Setter;
        }
        else
        {
            explicitSetter = m_ExplicitSetter;
        }

        data["explicitSetter"] = explicitSetter;
	}

	data["isStaticSetter"] = _setter_method && _setter_method->IsStatic() ? "true" : "false";
	data["setterBody"] = context->LoadTemplatePartial(kPartialFieldSetter);

    data["instanceBody"] = context->LoadTemplatePartial(kPartialInstanceDefination);
    data["argumentBody"] = context->LoadTemplatePartial(kPartialArgumentDefination);


    m_metaData.CompileTemplateData(data, context);

    return data;
}

bool FieldBase::ShouldCompile(void) const
{
    return m_enabled;
}

bool FieldBase::isAccessible(void) const
{
    return !m_metaData.GetFlag(native_attribute::Disable);
}

bool FieldBase::isGetterAccessible(void) const
{
    return m_hasExplicitGetter || !m_isProperty;
}

bool FieldBase::isSetterAccessible(void) const
{
    return !m_isConstArray && (m_fieldType.IsPointer() || !m_NoDefaultClassCopyMethod) && (m_hasExplicitSetter || !m_isProperty);
}