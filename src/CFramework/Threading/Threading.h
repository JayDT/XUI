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

#ifndef THREADING_H
#define THREADING_H

#define THREAD_SLEEP_CONST 150

#include <atomic>
#include <boost/thread/null_mutex.hpp>
#include <standard/threading.h>
#include <standard/concurrentQueue.h>

#ifdef WIN32
    void SetThreadName( DWORD dwThreadID, char* threadName);
#endif

namespace System
{
    namespace Async
    {
        class InvokeHandler;
    }

    namespace Threading
    {
        class Thread;
        class Runnable;
        class ThreadMananger;

        enum ThreadTypeId
        {
            ThreadTypeId_NotSetThreadType = 0,
            ThreadTypeId_WorldThread,
            ThreadTypeId_MapThread,
            ThreadTypeId_MapGroupThread,
            ThreadTypeId_DelaySqlThread,
            ThreadTypeId_CliThread,
            ThreadTypeId_RAThread,
            ThreadTypeId_LogonCommThread,
            ThreadTypeId_ClusterCommThread,
            ThreadTypeId_RootThread,
            ThreadTypeId_ThreadMgr
        };

        enum class Priority : uint8
        {
            IDLE = 0,
            LOWEST,
            LOW,
            NORMAL,
            HIGH,
            HIGHEST,
            REALTIME,
            MAX_PRIORITY_NUM
        };

        enum THREAD_RUNNING_STATUS
        {
            THREAD_RUNNING_STATUS_PREPARE = 0,
            THREAD_RUNNING_STATUS_RUNNING,
            THREAD_RUNNING_STATUS_SLEEP,
            THREAD_RUNNING_STATUS_SHUTDOWNING,
            THREAD_RUNNING_STATUS_SUSPENDED,
            THREAD_RUNNING_STATUS_EVENT_ACTIVATE_WAITING,
            THREAD_RUNNING_STATUS_CONDITION_THREAD_WEAK_UP,
            THREAD_RUNNING_STATUS_SHUTTING_DOWN
        };

        enum ThreadMgrGroupIds
        {
            ThreadGroupId_Default = 0,
            ThreadGroupId_Map,
            ThreadGroupId_Network,
            ThreadGroupId_World,
            ThreadGroupId_Cluster,
        };

        struct ThreadInfo
        {
        public:
            ThreadInfo() : m_thread(NULL)
            {
                //m_iThreadId = 0;
                m_hThreadHandle = 0;
                m_ThreadTypeId = ThreadTypeId_NotSetThreadType;
                m_Threadstatus = THREAD_RUNNING_STATUS_PREPARE;
            }

            ThreadInfo(const ThreadInfo* m_threadInfo) = delete;

            THREAD_RUNNING_STATUS m_Threadstatus;
            ThreadTypeId m_ThreadTypeId;
            size_t m_iThreadId;
            std::thread* m_hThreadHandle;
            Thread * m_thread;
            std::string m_name;
        private:
            ThreadInfo& operator=(const ThreadInfo&);
        };

        class TC_CFRAMEWORK_API Runnable : virtual public Threading::IReferenceCountedMT
        {
        public:
            friend class ThreadMananger;
            friend class Thread;
            explicit Runnable();
            virtual ~Runnable() {}
            virtual void run();
            virtual void update(uint32 const& diff);
            virtual void stop(bool noWait = false);
            virtual void wait();
            Thread* start(const char* threadName, int groupId = ThreadGroupId_Default);
            void Link();
            inline void ClearLatency() { m_lastupdateTime = 0; }
            inline void ResetLatency() { m_lastupdateTime = time(NULL); }
            inline time_t GetLatencyTimer() { return m_lastupdateTime; }
            Thread* GetThread();
            inline bool IsRunning() { return m_running; }
        protected:
            uint32 m_latency;

            Thread* m_selfThread;
            std::mutex m_lock;

        private:
            time_t m_lastupdateTime;
            bool m_running;
        };

        class TC_CFRAMEWORK_API ThreadPriority
        {
            static bool InitializePriorities();
        public:
            ThreadPriority();
            int getPriority(Priority p) const;
            int setPriority(std::thread* th, Priority p);

        private:
            Priority m_currentPriority;
            static int m_priority[(uint8)Priority::MAX_PRIORITY_NUM];
        };

        class TC_CFRAMEWORK_API Thread : virtual public Threading::IReferenceCountedMT
        {
            typedef std::function<void()> Action;
        public:
            friend class ThreadMananger;
            friend class Runnable;

            static std::mutex m_threadCreateMutex;

            virtual ~Thread();

            bool wait();
            void destroy();

            int suspend();
            int resume();

            template<typename FUNC, typename T>
            int start(FUNC cb, T owner, const char* threadName = nullptr, int groupId = -1)
            {
                Action* action = new Action([this, cb, owner]()
                {
                    (owner->*cb)();
                    shutdown();
                });

                m_threadCreateMutex.lock();
                if (threadName)
                    GetThreadInfo().m_name = threadName;
                m_dataLock.lock();
                GetThreadInfo().m_hThreadHandle = new std::thread(&Thread::ThreadCallback, this, action);
                InitializeThreadID();
                m_dataLock.unlock();
                m_threadCreateMutex.unlock();
                return 0;
            }

            int start();

            void InitializeThreadID();
            void Initialize();

            void setPriority(Priority type);
            Runnable* GetTask();
            bool isTerminate();

            static void Sleep(unsigned long msecs);
            static size_t CurrentId();
            static std::string CurrentIdToString();
            static std::thread const* currentHandle();

            void setStatus(THREAD_RUNNING_STATUS __c);
            THREAD_RUNNING_STATUS getStatus();
            void SetThreadType(ThreadTypeId __c);
            ThreadTypeId GetThreadType();
            ThreadInfo& GetThreadInfo();
            ThreadInfo const& GetThreadInfo() const;
            std::thread::id GetThreadId() const;
            std::atomic<int> m_refs;

            void shutdown()
            {
                m_dataLock.lock();
                m_ThreadInfo = nullptr;
                m_dataLock.unlock();
            }

            Thread();
        private:
            Thread(const Thread&) = delete;
            Thread& operator=(const Thread&) = delete;
            explicit Thread(Runnable* instance, int groupId = ThreadGroupId_Default);

            static void ThreadTask(void * param);
            static void ThreadCallback(void * param, void * param1);

            mutable std::recursive_mutex m_dataLock;

            Runnable * m_task;
            ThreadInfo* m_ThreadInfo;
            //use this object to determine current OS thread priority values mapped to enum Priority{}
            size_t m_iThreadId;
            static ThreadPriority m_TpEnum;
        };

        struct IAsyncThreadTaskPool
        {
            System::Concurrent::ActivationQueue<void*> Tasks;

            void PushWork(void* work)
            {
                Tasks.push(work);
            }
        };

        template<class TASK>
        class AsyncThreadTaskPool : public IAsyncThreadTaskPool
        {
            std::vector<TASK*> Workers;
        public:

            virtual ~AsyncThreadTaskPool()
            {
                Deactivate();
            }

            void Activate(uint workerCount, const char* poolname)
            {
                Workers.resize(workerCount);
                for (uint i = 0; i != workerCount; ++i)
                    (Workers[i] = new TASK(this))->start(poolname != nullptr ? poolname : "Task Worker ");
            }

            void Deactivate()
            {
                Tasks.Cancel();

                for (auto worker : Workers)
                {
                    worker->AddReference();
                    worker->GetThread()->AddReference();
                    worker->stop();
                    worker->wait();
                    worker->GetThread()->RemoveReference();
                    worker->RemoveReference();
                }

                Workers.clear();
            }

        };

        struct IAsyncTask : Runnable
        {
            IAsyncThreadTaskPool* poolMgr;
            IAsyncTask(IAsyncThreadTaskPool* _poolMgr) : poolMgr(_poolMgr) {}

            void* GetData()
            {
                try
                {
                    return poolMgr->Tasks.pop();
                }
                catch (...)
                {
                    return nullptr;
                }
            }
        };

        enum thrmgr_timers
        {
            THRMGRT_FREEZEDETECTOR = 0,
            THRMGRT_MEMORYDETECTOR = 0,
            THRMGRT_MAXTIMERS
        };

        class TC_CFRAMEWORK_API ThreadMananger : public System::Threading::Runnable
        {
            friend class Thread;
            friend class Runnable;
        public:
            explicit ThreadMananger();
            virtual ~ThreadMananger();

            static ThreadMananger* Instance()
            {
                static ThreadMananger m_instance;
                return &m_instance;
            }

            void run(void);

            void SetTSSThreadStore(Thread *);
            static Thread * CurrentThread();

            int LinkExternalThread(Runnable* instance);
            int LinkRootThread(Runnable* instance);
            int SpawnThread(Thread* Thread, uint32 Groupid);
            void DespawnThread(Thread* Thread);

            int SuspendAllThreadExcludeCurrentThread();
            int ResumeAllThread();

            bool t_active;

            void ThreadFinishSleep();

            System::Async::InvokeHandler* GetCallBack() { return m_selfCallback; }

        private:
            void Update(const uint32& m_diff);
            void HandleThreadFreeze(Thread* m_thread);
            void BuildThreadInfo(Runnable* m_instance, Thread* m_thread, bool rootThread = false);

            static ThreadMananger* m_instance;

            //global object - container for Thread class representation of every thread
            typedef boost::thread_specific_ptr<ThreadInfo> ThreadTSSStorage;
            static ThreadTSSStorage m_ThreadStorage;
            //std::mutex m_tssMutex;

            typedef UNORDERED_MAP<uint64, Thread*> ThreadStore;
            //typedef std::multimap<uint32, uint64> ThreadGroupStore;

            ThreadStore      m_ThreadStore;
            ThreadStore      m_ThreadExternalStore;
            //ThreadGroupStore m_ThreadGroupStore;

            uint32 m_timers[THRMGRT_MAXTIMERS];

            std::shared_mutex i_lock;

            Thread* m_rootThread;
            System::Async::InvokeHandler* m_selfCallback;

            int8 t_suspendAll;
            std::mutex mutex_Suspend;
            std::condition_variable cond_Suspend;
        };

        struct ThreadSuspendGuard
        {
            bool success;

            ThreadSuspendGuard()
            {
                success = ThreadMananger::Instance()->SuspendAllThreadExcludeCurrentThread() != -1;
            }

            ~ThreadSuspendGuard()
            {
                if (success)
                    ThreadMananger::Instance()->ResumeAllThread();
            }

            operator bool()
            {
                return success;
            }
        };
    }
}

struct TC_CFRAMEWORK_API BasicReferenceCounter
{
    explicit BasicReferenceCounter() : refCount(0) {}
    virtual ~BasicReferenceCounter() {}

    virtual int AddReference()
    {
        assert (refCount < 0xFFFF); //int max
        return ++refCount;
    }

    virtual int RemoveReference()
    {
        assert (refCount > 0);
        return --refCount;
    }

    void Reset() { refCount = 0; }
    bool HaveReference() const { return refCount > 0; }

    unsigned short refCount;
};

struct TC_CFRAMEWORK_API LockedReferenceCounter : public BasicReferenceCounter
{
    explicit LockedReferenceCounter() {}
    virtual ~LockedReferenceCounter() {}

    virtual int AddReference()
    {
        std::lock_guard<std::mutex> guard(_lock);
        return BasicReferenceCounter::AddReference();
    }

    virtual int RemoveReference()
    {
        std::lock_guard<std::mutex> guard(_lock);
        return BasicReferenceCounter::RemoveReference();
    }

    int GetRefCount()
    {
        std::lock_guard<std::mutex> guard(_lock);
        return refCount;
    }

    std::mutex _lock;
};

#define sThreadManager System::Threading::ThreadMananger::Instance()

#endif
