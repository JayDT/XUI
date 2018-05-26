#include "App.h"
#include "IrrXUI/XUIIrrRenderer.h"
#include "Log/Log.h"
#include "standard/Timer.h"
#include "standard/String/atoi_imp.h"
#include "Threading/Threading.h"

#include "IrrLicht/include/irrlicht.h"

#include "XUI/Core/Dependencies/DependencyObject.h"
#include "XamlCPP/DataBind/Binding.h"
#include "XamlCPP/Core/NotifyPropertyChanged.h"
#include "XUI/UI/TextBlock.h"
#include "standard/BasicPrimities.h"

#include "Reflected/DataObject.h"
#include "Reflection/Runtime/ReflectionModule.h"

#include "XUI/Core/GlobalInitializerScheduler.h"

#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/Assembler/Settings.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"
#include "XUI/UI/Window.h"
#include "XUI/UI/Grid.h"
#include "XUI/UI/Button.h"
#include "XUI/UI/Border.h"
#include "XUI/Core/Media/Brush.h"
#include "XUI/Core/TypeConverter/TypeConverters.h"
#include "XUI/Core/Application.h"

#include "CLogger.h"

#include <System/BaseTypes.h>

#include "UI/MainUI.h"
#include "UI/DebugUI.h"

extern void UnitTest();
extern void UnitTextXaml();
extern void UnitTestSytleXaml();
extern std::shared_ptr<XUI::UI::Window> UnitTestControlTemplateXaml();

WorldClient::Application::Application()
{
}


void WorldClient::Application::InitializeComponents()
{
    IrrBackend::XUIIrrPlatformDriver::Instance()->OnApplicationExit += System::make_eventhandler(&Application::OnApplicationExit, this);

#if false
    UnitTest();
    UnitTextXaml();
    UnitTestSytleXaml();
    UnitTestControlTemplateXaml();
#endif

    auto mainWindow = XUI::Platform::UIRender::LoadXaml(System::URI("pack://application:,,,/IrrXUI;/UI/MainUI.xaml", true));
    //auto debugWindow = XUI::Platform::UIRender::LoadXaml(System::URI("pack://application:,,,/IrrXUI;/UI/DebugUI.xaml", true));
    //static_cast<DebugWindow*>(debugWindow.get())->SetDebuggedWindow(static_cast<XUI::UI::Window*>(mainWindow.get()));
}

void WorldClient::Application::Initialize()
{
    IrrBackend::XUIIrrPlatformDriver::InitializeXUI();
    IrrBackend::XUIIrrPlatformDriver::Instance()->SetDebugFlag(IrrBackend::XUIIrrPlatformDriver::Instance()->GetDebugFlag() | XUI::Core::Dependency::RenderDebugFlags::RDF_FPS);

    InitializeComponents();
}

void WorldClient::Application::Run()
{
    IrrBackend::XUIIrrPlatformDriver* mgr = IrrBackend::XUIIrrPlatformDriver::Instance();

    int32 _lastpasstime = 0, _passtimediff = 0;
    uint32 _curMSTime = System::Time::getMSTime();

    while (m_running)
    {
        _lastpasstime = _curMSTime;
        _curMSTime = System::Time::getMSTime();
        _passtimediff = _curMSTime - _lastpasstime;

        mgr->Update(_passtimediff);
    }
}

void WorldClient::Application::OnApplicationExit(void * sender, System::Events::EventArg & e)
{
    m_running = false;

    // ToDo: uninitizers
}
