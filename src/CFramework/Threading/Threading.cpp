/*
 * Copyright (C) 2009-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
flags
hread creation flag    Description
THR_CANCEL_DISABLE    Do not allow the thread to be cancelled.
THR_CANCEL_ENABLE    Allow the thread to be cancelled.
THR_DETACHED    Create an asynchronous thread. The exit status of the thread would not be available to any other threads. The thread resources are reclaimed by the operating system whenever the thread is terminated.
THR_JOINABLE    Allow the new thread's exit status to be available to other threads. This is also the default attribute in ACE-created threads. When this kind of thread dies, the resources are not reclaimed by the operating system until some other thread joins with it.
THR_NEW_LWP    Create an explicit kernel-level thread (as opposed to a user-level thread).
THR_SUSPENDED    Create a new thread in the suspended state.
*/

#include "StdAfx.h"
#include "Threading.h"

#include <standard/Timer.h>
#include <standard/callback.h>

#include <vector>
#include <stdio.h>

using namespace System;
using namespace System::Threading;

std::mutex Thread::m_threadCreateMutex;
ThreadMananger::ThreadTSSStorage ThreadMananger::m_ThreadStorage;

#ifndef WIN32
#include "pchdef.h"
//#include "AuthHandler.h"
//#include "World.h"
#endif

#ifdef MEMCHECK
#include "Policies/leaker.h"
#endif

#ifndef WIN32
static sigset_t   signal_set;  /* signals to block         */
struct sigaction act;

static void
err_abort(int status, const char *msg)
{
    perror(msg);
    abort();
}
#else
//
// Usage: SetThreadName (-1, "MainThread");
//
#include <windows.h>
const DWORD MS_VC_EXCEPTION=0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( DWORD dwThreadID, char* threadName)
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
   }
}
#endif

Runnable::Runnable() : m_latency(0), m_selfThread(0), m_lastupdateTime(0), m_running(true)
{
}

void Runnable::wait()
{
    m_selfThread->wait();
}

void Runnable::stop(bool noWait /*= false*/)
{
    m_lock.lock();
    m_running = false;

    if (m_selfThread && !noWait)
    {
        m_lock.unlock();
        m_selfThread->wait();
        if (m_selfThread->getReferenceCount() > 1)
            printf("WARNING THREAD HAVE TOO MORE REF COUNT NOT DELETE AT SHUTDOWN! REF COUNT: %i ThreadID: 0x%X\n", m_selfThread->getReferenceCount(), (int)m_selfThread->m_iThreadId);
        m_selfThread->RemoveReference();
    }
    else
        m_lock.unlock();
}

Thread* Runnable::start(const char* threadName, int groupId)
{
    m_selfThread = new Thread(this, groupId);
    m_selfThread->GetThreadInfo().m_name = threadName;
    m_selfThread->AddReference();
    std::lock_guard<std::mutex> guard(m_lock);
    m_lastupdateTime = 0;
    m_selfThread->start();
    return m_selfThread;
}

Thread* Runnable::GetThread() { return m_selfThread; }


int System::Threading::ThreadPriority::m_priority[(uint8)Priority::MAX_PRIORITY_NUM];

bool System::Threading::ThreadPriority::InitializePriorities()
{
    int basePriority;
#ifdef WIN32
    basePriority = THREAD_PRIORITY_NORMAL;
#else
    basePriority = SCHED_OTHER;
#endif

    for (int i = (uint8)Priority::IDLE; i < (int)Priority::MAX_PRIORITY_NUM; ++i)
        m_priority[i] = basePriority;

#ifdef WIN32
    m_priority[(uint8)Priority::IDLE]     = THREAD_PRIORITY_IDLE;
    m_priority[(uint8)Priority::LOWEST]   = THREAD_PRIORITY_LOWEST;
    m_priority[(uint8)Priority::LOW]      = THREAD_PRIORITY_BELOW_NORMAL;
    m_priority[(uint8)Priority::NORMAL]   = basePriority;
    m_priority[(uint8)Priority::HIGH]     = THREAD_PRIORITY_ABOVE_NORMAL;
    m_priority[(uint8)Priority::HIGHEST]  = THREAD_PRIORITY_HIGHEST;
    m_priority[(uint8)Priority::REALTIME] = THREAD_PRIORITY_TIME_CRITICAL;
#else
    m_priority[(uint8)Priority::IDLE]     = sched_get_priority_min(SCHED_IDLE);
    m_priority[(uint8)Priority::NORMAL]   = basePriority;
    m_priority[(uint8)Priority::REALTIME] = sched_get_priority_max(SCHED_RR);

    m_priority[(uint8)Priority::HIGHEST]  = (m_priority[(uint8)Priority::REALTIME] + m_priority[(uint8)Priority::NORMAL]) / 2;
    m_priority[(uint8)Priority::HIGH]     = (m_priority[(uint8)Priority::HIGHEST] + m_priority[(uint8)Priority::NORMAL]) / 2;

    m_priority[(uint8)Priority::LOWEST]   = (m_priority[(uint8)Priority::IDLE] + m_priority[(uint8)Priority::NORMAL]) / 2;
    m_priority[(uint8)Priority::LOW]      = (m_priority[(uint8)Priority::LOWEST] + m_priority[(uint8)Priority::NORMAL]) / 2;
#endif

    return true;
}

System::Threading::ThreadPriority::ThreadPriority()
    : m_currentPriority(Priority::NORMAL)
{
    static bool initialized = InitializePriorities();
    assert(initialized);
}

int System::Threading::ThreadPriority::getPriority(System::Threading::Priority p) const
{
    if (p < Priority::IDLE)
        p = Priority::IDLE;

    if (p > Priority::REALTIME)
        p = Priority::REALTIME;

    return m_priority[(int)p];
}

int System::Threading::ThreadPriority::setPriority(std::thread* th, Priority p)
{
    m_currentPriority = p;

#ifdef WIN32
    SetThreadPriority(th->native_handle(), (int)p);
#else
    pthread_setschedprio(th->native_handle(), (int)p);
#endif
    return 0;
}

System::Threading::Thread::Thread()
    : m_ThreadInfo(new ThreadInfo), m_task(nullptr) // => for lamdba start func
{
    //AddReference();
    setStatus(THREAD_RUNNING_STATUS_PREPARE);
    std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    GetThreadInfo().m_thread = this;
}

Thread::Thread(Runnable* instance, int /*groupId*/)
    : m_ThreadInfo(new ThreadInfo), m_task(instance)
{
    //AddReference();
    assert(instance);
    setStatus(THREAD_RUNNING_STATUS_PREPARE);
    std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    instance->m_selfThread = GetThreadInfo().m_thread = this;
    m_task->AddReference(); // register reference to m_task to prevent it deletion until destructor
}

void Thread::setStatus(THREAD_RUNNING_STATUS __c)
{
    std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    GetThreadInfo().m_Threadstatus = __c;
}

THREAD_RUNNING_STATUS Thread::getStatus()
{
    return GetThreadInfo().m_Threadstatus;
}

void Thread::SetThreadType(ThreadTypeId __c)
{
    std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    GetThreadInfo().m_ThreadTypeId = __c;
}

ThreadTypeId Thread::GetThreadType()
{
    std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    return GetThreadInfo().m_ThreadTypeId;
}

std::thread::id Thread::GetThreadId() const
{
    return GetThreadInfo().m_hThreadHandle->get_id();
}

ThreadInfo& Thread::GetThreadInfo()
{
    //std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    return *m_ThreadInfo;
}

ThreadInfo const& Thread::GetThreadInfo() const
{
    //std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    return *m_ThreadInfo;
}

Runnable* Thread::GetTask()
{
    return m_task;
}

void Runnable::run()
{
    GetThread()->SetThreadType(System::Threading::ThreadTypeId_NotSetThreadType);

    uint32 realCurrTime = 0;
    uint32 realPrevTime = System::Time::getMSTime();
    uint32 prevSleepTime = 0;

    while (IsRunning())
    {
        ResetLatency();
        m_latency = System::Time::getMSTimeDiff(realPrevTime,(realCurrTime = System::Time::getMSTime()));
        update(m_latency);
        realPrevTime = realCurrTime;
        // diff (D0) include time of previous sleep (d0) + tick time (t0)
        // we want that next d1 + t1 == MAP_SLEEP_CONST
        // we can't know next t1 and then can use (t0 + d1) == MAP_SLEEP_CONST requirement
        // d1 = MAP_SLEEP_CONST - t0 = MAP_SLEEP_CONST - (D0 - d0) = MAP_SLEEP_CONST + d0 - D0
        if (m_latency <= 10+prevSleepTime)
        {
            prevSleepTime = 10+prevSleepTime-m_latency;
        }
        System::Threading::Thread::Sleep(prevSleepTime);
    }
}

void Runnable::update(uint32 const& /*diff*/)
{
    assert(false); //unused thread?!
}

bool Thread::isTerminate()
{
    std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    return (m_ThreadInfo == nullptr ? true : false);
}

Thread::~Thread()
{
    // deleted runnable object (if no other references)
    //if (GetThreadInfo().m_ThreadTypeId == ThreadTypeId_MapThread)
    //sLog.outError("End thread thread instance reference count: %d (If not 0 not delete) TypeId: %d", (GetThreadInfo().m_task->getRefCount()-1), GetThreadInfo().m_ThreadTypeId );

    // TSS will be destroy (lifetime equal with thread)
    //if (GetThreadInfo().m_hThreadHandle)
    //    delete GetThreadInfo().m_hThreadHandle;
    //GetThreadInfo().m_hThreadHandle = nullptr;

    sThreadManager->DespawnThread(this);

    std::lock_guard<std::recursive_mutex> guard(m_dataLock);
    if (m_task)
        m_task->RemoveReference();

    m_task = nullptr;
}

//initialize Thread's class static member
ThreadPriority Thread::m_TpEnum;

bool Thread::wait()
{
    m_dataLock.lock();
    if (isTerminate() || !GetThreadInfo().m_hThreadHandle)
    {
        m_dataLock.unlock();
        return false;
    }

    std::thread* th = (std::thread*)GetThreadInfo().m_hThreadHandle;
    m_dataLock.unlock();

    th->join();
    return true;
}

void Thread::destroy()
{
    //printf ("Thread destroy 1 [Thread Count: %i] \n", ACE_Thread::count_threads());
    if (!GetThreadInfo().m_iThreadId || !GetTask())
        return;

    GetTask()->stop();
    wait();
}

int Thread::suspend()
{
    return 0;
}

int Thread::resume()
{
    return 0;
}

void Thread::Initialize()
{
    if (GetTask())
        GetTask()->m_lock.lock();

    m_dataLock.lock();
    //sLog.outString("[THREAD]: Initialize threadid: %s (%s)", Thread::CurrentIdToString().c_str(), this->GetThreadInfo().m_name.c_str());
    assert(GetThreadInfo().m_iThreadId == Thread::CurrentId());
    m_dataLock.unlock();

    sThreadManager->SetTSSThreadStore(this);
    if (GetTask())
        GetTask()->m_lock.unlock();
    sThreadManager->SpawnThread(this, ThreadGroupId_Default);
#ifdef WIN32
    SetThreadName(GetThreadInfo().m_iThreadId, (char*)GetThreadInfo().m_name.c_str());
#else
    pthread_setname_np(pthread_self(), (char*)GetThreadInfo().m_name.c_str());
#endif

}

void Thread::ThreadTask(void * param)
{
    Thread * _thread = (Thread*)param;
    Runnable * _task = _thread->GetTask();

    _thread->Initialize();
    _thread->setStatus(THREAD_RUNNING_STATUS_RUNNING);
    _task->run();
    _task->RemoveReference(); // task execution complete, free referecne added at
    _thread->setStatus(THREAD_RUNNING_STATUS_SHUTDOWNING);
    sThreadManager->DespawnThread(_thread);
    _thread->RemoveReference(); // free thread data
    _thread->shutdown();
}

void System::Threading::Thread::ThreadCallback(void * param, void * param1)
{
    if (param1 == nullptr)
        return;

    Action* _cb = (Action*)param1;
    Thread * _thread = (Thread*)param;

    _thread->Initialize();
    _thread->setStatus(THREAD_RUNNING_STATUS_RUNNING);
    (*_cb)();
    if (!_thread->isTerminate())
    {
        _thread->setStatus(THREAD_RUNNING_STATUS_SHUTDOWNING);
        _thread->shutdown();
    }
    sThreadManager->DespawnThread(_thread);
    _thread->RemoveReference(); // free thread data
    delete _cb;
}

size_t Thread::CurrentId()
{
    long l;
    std::stringstream ss;
    ss << std::this_thread::get_id();
    ss >> l;
    return l;
}

std::string System::Threading::Thread::CurrentIdToString()
{
    std::stringstream ss;
    ss << System::Threading::Thread::CurrentId();
    return ss.str();
}

std::thread const* Thread::currentHandle()
{
    //ACE_hthread_t _handle;
    //ACE_Thread::self(_handle);

    return nullptr;
}

inline int System::Threading::Thread::start()
{
    m_threadCreateMutex.lock();
    m_dataLock.lock();
    GetThreadInfo().m_hThreadHandle = new std::thread(&Thread::ThreadTask, this);
    InitializeThreadID();
    m_dataLock.unlock();
    m_threadCreateMutex.unlock();
    return 0;
}

void System::Threading::Thread::InitializeThreadID()
{
    assert(GetThreadInfo().m_hThreadHandle->native_handle());

    long threadid;
    std::stringstream ss;
    ss << GetThreadInfo().m_hThreadHandle->get_id();
    ss >> threadid;

    GetThreadInfo().m_iThreadId = threadid;
    m_iThreadId = threadid;
}

void Thread::setPriority(Priority type)
{
    if (!currentHandle())
        return;

    void* nativeHandle = reinterpret_cast<void*>(((std::thread*)currentHandle())->native_handle());
    int _priority = m_TpEnum.getPriority(type);
    int _ok;
#ifdef WIN32
    _ok = SetThreadPriority(nativeHandle, _priority);
#else
    _ok = pthread_setschedprio((pthread_t)nativeHandle, _priority);
#endif
    //remove this ASSERT in case you don't want to know is thread priority change was successful or not
    assert (_ok == 0);
}

void Thread::Sleep(unsigned long msecs)
{
    Thread* currentThread = ThreadMananger::CurrentThread();
    if (currentThread)
    {
        auto pushedState = currentThread->getStatus();
        currentThread->setStatus(System::Threading::THREAD_RUNNING_STATUS::THREAD_RUNNING_STATUS_SLEEP);
        std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
        currentThread->setStatus(pushedState);
        sThreadManager->ThreadFinishSleep();
    }
    else
        std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}

Thread * ThreadMananger::CurrentThread()
{
    //sLog.outString("getCurrent threadid:"I64FMT" dec: "UI64FMTD,Thread::currentId(), Thread::currentId());
    ThreadInfo * m_ts_object = m_ThreadStorage.get();
    //MODUL_ASSERT(m_ts_object);
    if (!m_ts_object)
        return NULL;
    return m_ts_object->m_thread;
}

ThreadMananger::ThreadMananger()
{
    m_selfCallback = new System::Async::InvokeHandler();
    t_active = false;
    t_suspendAll = 0;

}

ThreadMananger::~ThreadMananger()
{
    printf("Destruct ThreadMgr ..... \n");
    if (m_selfCallback)
        delete m_selfCallback;

    i_lock.lock();

    if (m_ThreadStore.size() > 1)
    {
        assert(false);
    }
    else if (m_ThreadStore.size() == 1)
    {
        //assert(m_ThreadStore.begin()->second->GetThreadType() == ThreadTypeId::ThreadTypeId_RootThread);
    }

    i_lock.unlock();
    ThreadStore::iterator itr = m_ThreadExternalStore.begin();
    while (itr != m_ThreadExternalStore.end())
    {
        if (itr->second->getReferenceCount() > 1)
            printf("WARNING EXTERNAL THREAD HAVE TOO MORE REF COUNT NOT DELETE AT SHUTDOWN! REF COUNT: %i ThreadID: 0x%X\n", m_selfThread->getReferenceCount(), (int)m_selfThread->CurrentId());
        itr->second->m_task = NULL;
        itr->second->RemoveReference();
        m_ThreadExternalStore.erase(itr);
        itr = m_ThreadExternalStore.begin();
    }

    // Free root thread info
    delete m_ThreadStorage.release();
    //delete m_ThreadStorage.release();
}

void ThreadMananger::SetTSSThreadStore(Thread * m_thread)
{
    m_ThreadStorage.reset(&m_thread->GetThreadInfo());
}

void ThreadMananger::ThreadFinishSleep()
{
    if (t_suspendAll)
    {
        std::unique_lock<std::mutex> mlock(mutex_Suspend);
        if (t_suspendAll)
        {
            Thread* m_thread = CurrentThread();
            THREAD_RUNNING_STATUS lastState = m_thread->getStatus();
            m_thread->setStatus(THREAD_RUNNING_STATUS_SUSPENDED);

            cond_Suspend.wait(mlock);

            m_thread->setStatus(lastState);
        }
    }
}

int ThreadMananger::SuspendAllThreadExcludeCurrentThread()
{
    if (t_suspendAll == 0)
    {
        mutex_Suspend.lock();
        if (t_suspendAll == 0)
        {
            t_suspendAll++;

            mutex_Suspend.unlock();

            i_lock.lock_shared();
            time_t timeout = time(NULL) + 10;
            Thread* m_currentThread = CurrentThread();
            for (ThreadStore::iterator itr = m_ThreadStore.begin(); itr != m_ThreadStore.end();)
            {
                if (m_currentThread == itr->second || 
                    itr->second->GetThreadInfo().m_ThreadTypeId == ThreadTypeId_ThreadMgr ||
                    itr->second->getStatus() == THREAD_RUNNING_STATUS_EVENT_ACTIVATE_WAITING || 
                    itr->second->getStatus() == THREAD_RUNNING_STATUS_CONDITION_THREAD_WEAK_UP ||
                    itr->second->getStatus() == THREAD_RUNNING_STATUS_SLEEP ||
                    itr->second->getStatus() == THREAD_RUNNING_STATUS_SUSPENDED ||
                    itr->second->getStatus() == THREAD_RUNNING_STATUS_SHUTTING_DOWN )
                {
                    ++itr;
                    continue;
                }

                if ( time(NULL) > timeout )
                {
                    //sLog.outString("[WAIT FOR SUSPEND]: Timeout");
                    ResumeAllThread();
                    i_lock.unlock_shared();
                    return -1;
                }

                //sLog.outString("[WAIT FOR SUSPEND]: %s thread id: %u type: %u", itr->second->GetThreadInfo().m_name.c_str(), (uint)itr->second->GetThreadInfo().m_iThreadId, itr->second->GetThreadInfo().m_ThreadTypeId);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            i_lock.unlock_shared();
            return 0;
        }
        else
            mutex_Suspend.unlock();
    }
    return -1;
}

int ThreadMananger::ResumeAllThread()
{
    i_lock.lock_shared();
    std::unique_lock<std::mutex> mlock(mutex_Suspend);
    t_suspendAll--;
    cond_Suspend.notify_all();
    i_lock.unlock_shared();
    return 0;
}

void ThreadMananger::run(void)
{
#ifndef WIN32
    int status;
    sigemptyset (&signal_set);
    sigaddset (&signal_set, SIGUSR1);
    status = pthread_sigmask (SIG_BLOCK, &signal_set, NULL);
    if (status != 0)
        err_abort (status, "Set signal mask");
#endif

    m_selfThread->SetThreadType(System::Threading::ThreadTypeId_ThreadMgr);

    uint32 realCurrTime = 0;
    uint32 realPrevTime = System::Time::getMSTime();
    uint32 prevSleepTime = 0;

    for (uint32 i = 0; i != THRMGRT_MAXTIMERS; ++i)
        m_timers[i] = 0;

    m_timers[THRMGRT_FREEZEDETECTOR] = 60000; //server start not checked
    m_timers[THRMGRT_MEMORYDETECTOR] = 60000; //server start not checked

    while (IsRunning())
    {
        ResetLatency();

        realCurrTime = System::Time::getMSTime();
        m_latency = System::Time::getMSTimeDiff(realPrevTime,realCurrTime);
        if (t_active)
            Update(m_latency);
        m_selfCallback->Update();
        realPrevTime = realCurrTime;
        if (m_latency <= 1000+prevSleepTime)
        {
            prevSleepTime = 1000+prevSleepTime-m_latency;
            System::Threading::Thread::Sleep(prevSleepTime);
        }
        else
            prevSleepTime = 0;
    }
}

void ThreadMananger::Update(const uint32& m_diff)
{
    i_lock.lock_shared();
    if (m_timers[THRMGRT_FREEZEDETECTOR] <= m_diff)
    {
        for (ThreadStore::iterator itr = m_ThreadStore.begin(); itr != m_ThreadStore.end(); ++itr)
        {
            if (!itr->second->GetTask())
                continue;

            if (!itr->second->GetTask()->GetLatencyTimer()) //not active freeze detector this thread
                continue;

            if ((itr->second->GetTask()->GetLatencyTimer() + 1200) < time(NULL))
            {
                HandleThreadFreeze(itr->second);
                continue;
            }

            //if ((itr->second->GetTask()->GetLatencyTimer() + 5) < time(NULL))
            //    sLog.outError("WARNING THREAD LAGG SPIKE: TypeID: %u Status: %d ThreadID: 0x%X time: %li ms PID: %u", (int)(itr->second->GetThreadInfo().m_ThreadTypeId),
            //                                itr->second->GetThreadInfo().m_Threadstatus, (int)(itr->second->GetThreadInfo().m_iThreadId),
            //                                (time(NULL) - itr->second->GetTask()->GetLatencyTimer()), (int)0);
        }
        m_timers[THRMGRT_FREEZEDETECTOR] = 5000;
    }
    else
        m_timers[THRMGRT_FREEZEDETECTOR] -= m_diff;

#ifndef WIN32
    if (m_timers[THRMGRT_MEMORYDETECTOR] <= m_diff)
    {
        /*if (uint32 memoryCap = sConfig.getConfig(CONFIG_UINT32_SHUTDOWN_MEMORY_PERCENT))
        {
            memoryStatistics pMem;
            uint32 usedMemory = pMem.usedMemoryPercent();
            if (usedMemory > memoryCap)
            {
                sLog.outError("Memory limit cap! Used: %d Cap: %d ", usedMemory, memoryCap);
                raise(2);
            }
        }*/
        m_timers[THRMGRT_MEMORYDETECTOR] = 1000;
    } else
        m_timers[THRMGRT_MEMORYDETECTOR] -= m_diff;
#endif
    i_lock.unlock_shared();

    //sLog.UpdateDelayEvents();
}

void ThreadMananger::HandleThreadFreeze(Thread* m_thread)
{
    //sLog.outError("WARNING THREAD FREEZE: TypeID: %d Status: %d ThreadID: 0x%x time: %li ms", (int)m_thread->GetThreadInfo().m_ThreadTypeId, (int)m_thread->GetThreadInfo().m_Threadstatus, (int)m_thread->GetThreadInfo().m_iThreadId, (time(NULL) - m_thread->GetTask()->GetLatencyTimer()));
}

int ThreadMananger::SpawnThread(Thread* Thread, uint32 /*Groupid*/)
{
    //assert(Thread->GetTask());
    if (Thread->GetTask())
        Thread->GetTask()->AddReference();

    i_lock.lock();
    //sLog.outString("New thread: 0x%x \n", (int)Thread::CurrentId());
    m_ThreadStore[Thread::CurrentId()] = Thread;
    i_lock.unlock();

    return 0; //All ok
}

void ThreadMananger::DespawnThread(Thread* Thread)
{
    if (!Thread)
        return;

    size_t mThreadId = Thread->m_iThreadId;
    if (mThreadId == 0)
        return;

    //if (!sLog.si_destroyed)
    //    sLog.outString("Delinked Thread manager 0x%x \n", (uint32)mThreadId);

    //Thread->setStatus(System::Threading::THREAD_RUNNING_STATUS_SHUTTING_DOWN);

    i_lock.lock();
    m_ThreadStore.erase(mThreadId);
    i_lock.unlock();
}

void ThreadMananger::BuildThreadInfo(Runnable* m_instance, Thread* m_thread, bool rootThread)
{
    m_thread->AddReference();

    // Now not support Unix os like suspend and resume
    m_thread->GetThreadInfo().m_hThreadHandle = nullptr;

//#ifdef WIN32
//    HANDLE native_threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());
//    m_thread->GetThreadInfo().m_hThreadHandle = new std::thread(native_threadHandle);
//#else
//    m_thread->GetThreadInfo().m_hThreadHandle = Thread::currentHandle();
//#endif

    m_thread->GetThreadInfo().m_iThreadId = Thread::CurrentId();
    assert(Thread::CurrentId());
    m_thread->m_task = m_instance;
    if (m_thread->GetTask())
        m_thread->GetTask()->AddReference();
    m_thread->GetThreadInfo().m_thread = m_thread;
    m_thread->GetThreadInfo().m_Threadstatus = THREAD_RUNNING_STATUS_RUNNING;
    m_thread->GetThreadInfo().m_ThreadTypeId = rootThread ? ThreadTypeId_RootThread : ThreadTypeId_NotSetThreadType;
}

int ThreadMananger::LinkExternalThread(Runnable* instance)
{
    assert(instance);
    i_lock.lock();
    m_ThreadExternalStore[Thread::CurrentId()] = new Thread();
    BuildThreadInfo(instance, m_ThreadExternalStore[Thread::CurrentId()], false);
    //sLog.outString("New (external) thread: 0x%x \n", (int)Thread::CurrentId());
    m_ThreadStore[Thread::CurrentId()] = m_ThreadExternalStore[Thread::CurrentId()]; //not grouped!
    i_lock.unlock();

    SetTSSThreadStore(m_ThreadExternalStore[Thread::CurrentId()]);
    return 0; //All ok!
}

int ThreadMananger::LinkRootThread(Runnable* instance)
{
    assert(instance);
    m_rootThread = new Thread();
    BuildThreadInfo(instance, m_rootThread, true);
    instance->m_selfThread = m_rootThread;

    i_lock.lock();
    //sLog.outString("Main thread: 0x%x \n", (int)m_rootThread->GetThreadInfo().m_iThreadId);
    m_ThreadStore[m_rootThread->GetThreadInfo().m_iThreadId] = m_rootThread; //not grouped!
    i_lock.unlock();

    SetTSSThreadStore(m_rootThread);
    return 0; //All ok!
}
