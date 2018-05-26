/* ----------------------------------------------------------------------------
** GENERATED SOURCE FILE
** Do not modify the contents of this file.
**
** System::Reflection Meta Generator v1.0
** --------------------------------------------------------------------------*/

#include "Reflection/Runtime/Default_Assembly/Assembly_Modul_Default.h"

///////////////////////////////////////////////////////////////////////////////
// Module Files
///////////////////////////////////////////////////////////////////////////////


#include "Reflection/Runtime/Default_Assembly/Assembly_Object.h"
#include "Reflection/Runtime/Default_Assembly/Assembly_MetaProperty.h"

namespace m = System::Reflection::meta;

m::ReflectionModule* __CurrentRunTime = nullptr;

///////////////////////////////////////////////////////////////////////////////
// Module Initialize and Declare
///////////////////////////////////////////////////////////////////////////////

bool InitializeModulAssembly()
{
    if (!__CurrentRunTime)
    {
        __CurrentRunTime = m::ReflectionModulManager::Instance().RegisterAssembly<System::Reflection::Assembly::AssemblyModulDefault>();
        m::ReflectionModulManager::Instance().InitReflectionModule.push(__CurrentRunTime);
    }
    return true;
}

static bool Initalized = InitializeModulAssembly();

namespace System::Reflection::meta
{
    ReflectionModule* __GetCurrent_RunTime__ReflectionModule()
    {
        return __CurrentRunTime;
    }
}

System::Reflection::Assembly::AssemblyModulDefault::AssemblyModulDefault()
    : ReflectionModule("Default", m::GetCurrentModule())
{
}

void System::Reflection::Assembly::AssemblyModulDefault::CreateTypes()
{
    ReflectionModule::Initialize();

    ///////////////////////////////////////////////////////////////////////////
    // Module File Allocation
    ///////////////////////////////////////////////////////////////////////////
    
    AllocateModuleFileObject( *_db );
    AllocateModuleFileMetaProperty( *_db );
}

void System::Reflection::Assembly::AssemblyModulDefault::Initialize()
{
    ///////////////////////////////////////////////////////////////////////////
    // Module File Definition
    ///////////////////////////////////////////////////////////////////////////
    
    DefineModuleFileObject( *_db );
    DefineModuleFileMetaProperty( *_db );
}

System::Reflection::Assembly::AssemblyModulDefault::~AssemblyModulDefault(void)
{
}
