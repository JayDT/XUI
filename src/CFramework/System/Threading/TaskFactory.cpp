#include "StdAfx.h"
#include "TaskFactory.h"
#include "Task.h"


System::Threading::TaskBackgroundWorker::~TaskBackgroundWorker()
{

}

System::Threading::TaskBackgroundWorker* System::Threading::TaskBackgroundWorker::Instance()
{
    static std::mutex lock;
    static System::Threading::TaskBackgroundWorker* _instance = nullptr;

    if (!_instance)
    {
        lock.lock();
        if (!_instance)
        {
            _instance = new System::Threading::TaskBackgroundWorker;
            _instance->start("Main Task Worker", 0);
        }
        lock.unlock();
    }

    return _instance;
}

void System::Threading::TaskBackgroundWorker::Add(std::shared_ptr<System::Threading::Task> const & obj)
{
    obj->m_active.exchange(true);
    m_BGObjects.push(obj);
}

void System::Threading::TaskBackgroundWorker::AddPriorityTask(std::shared_ptr<System::Threading::Task> const & obj)
{
    obj->m_active.exchange(true);
    m_BGObjects.push_front(obj);
}

void System::Threading::TaskBackgroundWorker::run()
{
    m_selfThread->setStatus(System::Threading::THREAD_RUNNING_STATUS_CONDITION_THREAD_WEAK_UP);

    std::shared_ptr<System::Threading::Task> s;

    while (IsRunning() && m_BGObjects.pop(s))
    {
        s->Execute();
        s->m_active.exchange(false);
    }
}

std::shared_ptr<System::Threading::Task> System::Threading::TaskBackgroundWorker::CreateTask()
{
    return nullptr;
}

std::shared_ptr<System::Threading::Task> System::Threading::TaskBackgroundWorker::CreateTask(std::shared_ptr<System::Events::EventHook<System::Events::EventArg>> const & task)
{
    std::shared_ptr<System::Threading::Task> t = std::make_shared<System::Threading::LambdaTask>(task, this);
    return t;
}
