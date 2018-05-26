#pragma once

#ifndef _WC_PCH_COMMON_H_
#define _WC_PCH_COMMON_H_

// Required for Boost (1.64)
#define _HAS_AUTO_PTR_ETC	1

namespace System::Reflection::Assembly
{
    class AssemblyModulworldclient;
}

#include <standard/Platform/Common.h>
#include "standard/BasicPrimities.h"
#include <standard/Errors.h>
#include <standard/events.h>
#include <standard/callback.h>
#include <standard/misc.h>

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "rxcpp/rx.hpp"
#include "XamlCPP/Interfaces/IXamlCPP.h"

#endif // _WC_PCH_COMMON_H_
