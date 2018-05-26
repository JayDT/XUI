#ifndef __WCLIENT_APP_H__
#define __WCLIENT_APP_H__

namespace irr
{
    class IrrlichtDevice;
}

namespace WorldClient
{
    class Application
    {
        bool m_running = true;

        void InitializeComponents();

    public:
        Application();

        void Initialize();
        void Run();

        void OnApplicationExit(void* sender, System::Events::EventArg& e);
    };
}

#endif // !__WCLIENT_APP_H__
