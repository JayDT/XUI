#include "StdAfx.h"
#include "Task.h"
#include "TaskFactory.h"

void System::Threading::Task::Start()
{
    if (m_worker)
    {
        m_worker->Add(shared_from_this());
    }
}

void System::Threading::Task::Wait()
{
    while (m_active.load())
    {
        System::Threading::Thread::Sleep(1); // Todo: correct implement Spin wait or conditinal signal
    }
}

bool System::Threading::Task::IsRunning() const
{
    return m_active.load();
}

System::Threading::LambdaTask::~LambdaTask()
{
}

void System::Threading::LambdaTask::Execute()
{
    if (_execute)
    {
        if (ExecuteArgument)
        {
            (*_execute)(this, *ExecuteArgument.get());
        }
        else
        {
            System::Events::EventArg e;
            (*_execute)(this, e);
        }
    }
}
