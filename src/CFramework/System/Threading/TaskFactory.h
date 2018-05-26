#ifndef __SYSTEM_THREADING_TASKFACTORY_H__
#define __SYSTEM_THREADING_TASKFACTORY_H__

#include <System/System.h>
#include <standard/events.h>
#include <Threading/Threading.h>
#include <standard/concurrentQueue.h>

namespace System::Threading
{
    class Task;

    class TC_CFRAMEWORK_API TaskBackgroundWorker : public System::Threading::Runnable
    {
        friend class Task;

    private:

        System::Concurrent::ActivationQueue<std::shared_ptr<System::Threading::Task>> m_BGObjects;

        void Add(std::shared_ptr<System::Threading::Task> const& obj);

        void AddPriorityTask(std::shared_ptr<System::Threading::Task> const& obj);

    public:

        virtual ~TaskBackgroundWorker();

        static TaskBackgroundWorker* Instance();

        virtual void stop(bool noWait = false)
        {
            System::Threading::Runnable::stop(noWait);
        }

        virtual void run();

        std::shared_ptr<System::Threading::Task> CreateTask();

        std::shared_ptr<System::Threading::Task> CreateTask(std::shared_ptr<System::Events::EventHook<System::Events::EventArg>> const& task);

    };
}

#endif // !__SYSTEM_THREADING_TASKFACTORY_H__
