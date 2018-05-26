#ifndef __SYSTEM_THREADING_TASK_H__
#define __SYSTEM_THREADING_TASK_H__

#include <System/System.h>
#include <standard/events.h>
#include <future>

namespace System::Threading
{
    class TaskBackgroundWorker;

    class TC_CFRAMEWORK_API Task : public System::Reflection::Object, public System::EnableSharedFromThisEx<Task>
    {
        friend class TaskBackgroundWorker;

    private:

        mutable std::atomic<bool> m_active;
        TaskBackgroundWorker* m_worker = nullptr;

    public:

        Task(TaskBackgroundWorker* worker)
            : m_worker(worker), m_active(false)
        {

        }

        virtual ~Task()
        {

        }

        virtual void Execute() = 0;
        virtual void Start();
        virtual void Wait();
        bool IsRunning() const;
    };

    class TC_CFRAMEWORK_API LambdaTask : public Task
    {
    private:

        typedef std::shared_ptr<System::Events::EventHook<System::Events::EventArg>> ExecuteEventHandler;

        ExecuteEventHandler _execute;

    public:

        std::shared_ptr<System::Events::EventArg> ExecuteArgument;

        LambdaTask(ExecuteEventHandler const& _e, TaskBackgroundWorker* worker)
            : Task(worker), _execute(_e)
        {

        }
        virtual ~LambdaTask();


        // Inherited via Task
        virtual void Execute() override;

    };
}

#endif // !__SYSTEM_IMAGE_JPEGBITMAPDECODER_H__
