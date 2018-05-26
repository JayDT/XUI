#ifndef _CFramework_DB_COMMON_H_
#define _CFramework_DB_COMMON_H_

#include <atomic>
#include <future>
#include <boost/thread/null_mutex.hpp>
#include <System/System.h>
#include <standard/events.h>
#include <Threading/Threading.h>
#include <standard/concurrentQueue.h>

#include <standard/Platform/Common.h>
#include <standard/client/DataSource_Standard.h>
#include "XamlCPP/Interfaces/IXamlCPP.h"

MetaAssemblyBegin
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/System", "System")) // templorary before implement clr namespace
MetaAssemblyEnd

#endif _CLIENT_DB_COMMON_H_
