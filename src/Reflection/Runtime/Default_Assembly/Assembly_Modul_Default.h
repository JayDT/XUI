/* ----------------------------------------------------------------------------
** GENERATED HEADER FILE
** Do not modify the contents of this file.
**
** System::Reflection Meta Generator v1.0
** --------------------------------------------------------------------------*/

#pragma once

#include "Reflection/Runtime/ReflectionDatabase.h"
#include "Reflection/Runtime/ReflectionModule.h"

namespace System::Reflection::Assembly 
{
    inline namespace meta
    {
        class ReflectionDatabase;
        class Variant;
    }

    class AssemblyModulDefault : public System::Reflection::meta::ReflectionModule
    {
        ///////////////////////////////////////////////////////////////////////////
        // Module Files
        ///////////////////////////////////////////////////////////////////////////
        
        void AllocateModuleFileObject( System::Reflection::meta::ReflectionDatabase &db );
        void AllocateModuleFileMetaProperty( System::Reflection::meta::ReflectionDatabase &db );
        
        void DefineModuleFileObject( System::Reflection::meta::ReflectionDatabase &db );
        static intptr_t offset_adjustment_Object_impl(size_t classId, System::Reflection::meta::Variant& from);
        static void field_Setter_Object_impl(size_t methodId, System::Reflection::meta::Variant&& obj, System::Reflection::meta::Variant&& value);
        static System::Reflection::meta::Variant field_Getter_Object_impl(size_t methodId, System::Reflection::meta::Variant&& obj);
        static System::Reflection::meta::FieldDesc const& fieldinfo_Object_impl(size_t fieldId);
        static System::Reflection::meta::Variant invoker_Object_impl(size_t methodId, System::Reflection::meta::Variant&& obj, const System::Reflection::meta::ArgumentList&& arguments);
        static System::Reflection::meta::MethodDesc const& methodinfo_Object_impl(size_t methodId);
        void DefineModuleFileMetaProperty( System::Reflection::meta::ReflectionDatabase &db );
        static intptr_t offset_adjustment_MetaProperty_impl(size_t classId, System::Reflection::meta::Variant& from);
        static void field_Setter_MetaProperty_impl(size_t methodId, System::Reflection::meta::Variant&& obj, System::Reflection::meta::Variant&& value);
        static System::Reflection::meta::Variant field_Getter_MetaProperty_impl(size_t methodId, System::Reflection::meta::Variant&& obj);
        static System::Reflection::meta::FieldDesc const& fieldinfo_MetaProperty_impl(size_t fieldId);
        static System::Reflection::meta::Variant invoker_MetaProperty_impl(size_t methodId, System::Reflection::meta::Variant&& obj, const System::Reflection::meta::ArgumentList&& arguments);
        static System::Reflection::meta::MethodDesc const& methodinfo_MetaProperty_impl(size_t methodId);

    public:
        AssemblyModulDefault();
        ~AssemblyModulDefault(void);

        void CreateTypes() override;
        void Initialize() override;
    };
}