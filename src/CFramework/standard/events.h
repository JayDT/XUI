/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CORE_EVENTS_H_
#define _CORE_EVENTS_H_

#include <string>
#include <vector>
#include <list>
#include <set>
#include <mutex>
#include <memory>

#include "standard/HookList.h"
#include "standard/misc.h"
#include "Reflection/Runtime/Object.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Method.h"
#include "System/BuildInMetaAttributes.h"

#include <functional>

namespace System
{
    template <typename Function> struct function_traits;

    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const> {
        using function = const std::function<ReturnType(Args...)>;
    };

    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...)> {
        using function = std::function<ReturnType(Args...)>;
    };

    template<typename T>
    auto make_function(T const& f) ->
        typename std::enable_if<std::is_function<T>::value && !std::is_bind_expression<T>::value, std::function<T>>::type
    {
        return f;
    }

    template<typename T>
    auto make_function(T const& f) ->
        typename std::enable_if<!std::is_function<T>::value && !std::is_bind_expression<T>::value, typename function_traits<decltype(&T::operator())>::function>::type
    {
        return static_cast<typename function_traits<decltype(&T::operator())>::function>(f);
    }

    template<typename T>
    auto make_function(T const& f) ->
        typename std::enable_if<std::is_bind_expression<T>::value, void>::type
    {
        static_assert(std::is_bind_expression<T>::value && false, "functional::make_function cannot be used with a bind expression.");
    }

    template<typename T>
    size_t buildin_address(std::function<T> const& f)
    {
        const T ** fnPointer = (const T **)f.template target<T*>();
        return (size_t)*fnPointer;
    }
}

namespace System
{
    namespace Events
    {
        template <typename T_EventArg>
        struct EventHook;

        struct IEventHandler;

        struct IEventArg : System::Reflection::Object
        {
            virtual int getTypeId() { return -1; }
        };

        struct EventArg : IEventArg
        {
            virtual ~EventArg() {}
            virtual std::string ToString() const override { return "EventArg"; }
            virtual int getTypeId() override { return 0; }
        };

        static EventArg sEventArg;

        struct IEventHook : System::Reflection::Object, System::EnableSharedFromThisEx<IEventHook>
        {
            virtual ~IEventHook() {}

            virtual size_t getHash() const = 0;
            virtual void* getInstancePtr() const = 0;

            /// EventArg not type safe but very generic
            virtual void operator()(void*, EventArg&) const = 0;
            virtual void Attach(IEventHandler* eh) = 0;
            virtual void Detach() = 0;
            virtual bool IsValid() = 0;
            virtual bool IsGeneric() { return false; }
            virtual bool IsEqual(void* object, void* func) const = 0;

            inline bool IsEqual(IEventHook* rhs) const
            {
                return rhs && getInstancePtr() == rhs->getInstancePtr() && getHash() == rhs->getHash();
            }

            //bool Equal(System::Reflection::VariantBase const& v) const override final
            //{
            //    return System::Reflection::Object::Equal(v);
            //}
        };

        struct IEventHandler
        {
            virtual ~IEventHandler() {}

            virtual void InvalidateEvent(IEventHook* _delegate) = 0;
            virtual void operator-=(IEventHook* _delegate) = 0;

			virtual void AddHandler(System::Events::IEventHook* handler) = 0;
			virtual void RemoveHandler(System::Events::IEventHook* handler) = 0;
        };

        struct
            Meta(System::PropertyEventType(typeid(System::Events::IEventHook)))
            IPropertyEvent : System::Reflection::Object
        {
            virtual ~IPropertyEvent() {}
        };

        template<typename TEventArg> struct PropertyEvent : IPropertyEvent
        {
			IEventHandler* _handler = nullptr;

            PropertyEvent(IEventHandler* handler) : _handler(handler)
            {}

            virtual ~PropertyEvent() {}

            void operator+=(System::Events::IEventHook* handler)
            {
				_handler->AddHandler(handler);
            }

            void operator-=(System::Events::IEventHook* handler)
            {
				_handler->RemoveHandler(handler);
			}

            void AddHandler(System::Events::IEventHook* handler)
            {
				_handler->AddHandler(handler);
			}

            void RemoveHandler(System::Events::IEventHook* handler)
            {
				_handler->RemoveHandler(handler);
			}
        };

        struct IDelegateObject
        {
            virtual ~IDelegateObject() {}

            virtual void DetachHook(IEventHook*) = 0;
            virtual void Invalidate(IEventHook*) = 0;
        };

        template <typename T_EventArg = EventArg>
        struct EventHook : public IEventHook
        {
            explicit EventHook() : mParent(NULL), mRefMgr(NULL) {}
            virtual ~EventHook()
            {
                Detach();
                Invalidate();
            }

            //virtual size_t getHash() const = 0;
            //virtual void* getInstancePtr() const = 0;

            virtual std::string ToString() const override { return "EventHook"; }

            //virtual void operator()(void*, EventArg&) const = 0;
            virtual bool IsSame(EventHook<T_EventArg>* _delegate) const = 0;
            virtual bool IsThreadSafe() const = 0;
            virtual void SetThreadSafe() {}

            virtual void Attach(IEventHandler* eh) override
            {
                if (mRefMgr)
                {
                    if (mParent) // only one parent possible (other way create new delegate)
                        throw "invalid operation";
                    mParent = eh;
                }
            }

            virtual void Detach() override
            {
                if (mParent)
                {
                    mParent->InvalidateEvent(this);
                    mParent = NULL;
                }
            }

            void SetRefMgr(IDelegateObject* m)
            {
                if (mParent) // too late call
                    throw "invalid operation";
                mRefMgr = m;
            }

            void Invalidate()
            {
                if (mRefMgr)
                {
                    mRefMgr->Invalidate(this);
                    mRefMgr = NULL;
                }
            }

            IEventHandler* mParent;
            IDelegateObject* mRefMgr;
        };

        template<typename Hooker, typename HookCall, typename T_EventArg>
        struct EventFuncHook : public EventHook<T_EventArg>
        {
            explicit EventFuncHook(HookCall ec, Hooker hooker) : EventHook<T_EventArg>(), _hookCall(hooker), _fnHookCall(ec), isThreadSafe(false) {}
            virtual ~EventFuncHook() {}

            virtual size_t getHash() const override
            {
                size_t hash = typeid(Hooker).hash_code();
                System::hash_combine(hash, size_t(reinterpret_cast<intptr_t>(reinterpret_cast<void* const&>(_fnHookGenericPtr))));
                return hash;
            }
            virtual void* getInstancePtr() const override { return (void*)_hookCall; }

            virtual std::string ToString() const override { return "EventFuncHook"; }
            void operator()(void* sender, EventArg& e) const override final
            {
                if (_hookCall)
                {
                    T_EventArg& _e = static_cast<T_EventArg&>(e);
                    (_hookCall->*_fnHookCall)(sender, _e);
                }
            }
            virtual void operator()(void* sender, T_EventArg& arg) { if (_hookCall) (_hookCall->*_fnHookCall)(sender, arg); }
            virtual bool IsEqual(void* object, void* func) const { return (object == (void*)_hookCall && func == (void*)_fnHookGenericPtr); }
            virtual bool IsSame(EventHook<T_EventArg>* _delegate) const { return _delegate->getHash() == getHash() && ((EventFuncHook*)_delegate)->_hookCall == _hookCall; }
            virtual void SetThreadSafe() override { isThreadSafe = true; }
            virtual bool IsThreadSafe() const override { return isThreadSafe; }
            virtual bool IsValid() override { return _hookCall != NULL; }

            virtual void Detach() override
            {
                _hookCall = NULL;
                EventHook<T_EventArg>::Detach();
            }

            Hooker   _hookCall;
            union
            {
                HookCall _fnHookCall;
                void* _fnHookGenericPtr;
            };

            bool isThreadSafe : 1;
        };

        template<typename TInstance, typename TMethod, typename TEventArg>
        struct EventMethodHook : public EventHook<TEventArg>
        {
            explicit EventMethodHook(TMethod ec, std::weak_ptr<TInstance> hooker) : EventHook<TEventArg>(), _instance(hooker), _fnHookCall(ec), isThreadSafe(false) {}
            virtual ~EventMethodHook() {}

            virtual size_t getHash() const override
            {
                size_t hash = typeid(TInstance).hash_code();
                System::hash_combine(hash, size_t(reinterpret_cast<intptr_t>(reinterpret_cast<void* const&>(_fnHookGenericPtr))));
                return hash;
            }
            virtual void* getInstancePtr() const override { return (void*)_instance.lock().get(); }

            virtual std::string ToString() const override { return "EventMethodHook"; }
            void operator()(void* sender, EventArg& e) const override final
            {
                if (!_instance.expired())
                {
                    TEventArg& _e = static_cast<TEventArg&>(e);
                    (_instance.lock().get()->*_fnHookCall)(sender, _e);
                }
            }
            virtual void operator()(void* sender, TEventArg& arg) { if (!_instance.expired()) (_instance.lock().get()->*_fnHookCall)(sender, (TEventArg&)arg); }
            virtual bool IsEqual(void* object, void* func) const { return (object == _instance.lock().get() && func == (void*)_fnHookGenericPtr); }
            virtual bool IsSame(EventHook<TEventArg>* _delegate) const { return _delegate->getHash() == getHash() && ((EventMethodHook*)_delegate)->_instance.lock().get() == _instance.lock().get(); }
            virtual void SetThreadSafe() override { isThreadSafe = true; }
            virtual bool IsThreadSafe() const override { return isThreadSafe; }
            virtual bool IsValid() override { return _instance.lock().get() != nullptr; }

            virtual void Detach() override
            {
                _instance.reset();
                EventHook<TEventArg>::Detach();
            }

            std::weak_ptr<TInstance> _instance;
            union
            {
                TMethod _fnHookCall;
                void* _fnHookGenericPtr;
            };

            bool isThreadSafe : 1;
        };

        template<typename TInstance, typename TEventArg>
        struct EventReflectionHook : public EventHook<TEventArg>
        {
            explicit EventReflectionHook(const System::Reflection::Method* ec, std::weak_ptr<TInstance> hooker) : EventHook<TEventArg>(), _instance(hooker), _fnHookCall(ec), isThreadSafe(false) {}
            virtual ~EventReflectionHook() {}

            virtual size_t getHash() const override
            {
                size_t hash = typeid(TInstance).hash_code();
                System::hash_combine(hash, size_t(reinterpret_cast<intptr_t>(reinterpret_cast<void const* const&>(_fnHookCall))));
                return hash;
            }
            virtual void* getInstancePtr() const override { return (void*)_instance.lock().get(); }

            virtual std::string ToString() const override { return "EventReflectionHook"; }
            void operator()(void* sender, EventArg& e) const override final
            {
                if (!_instance.expired())
                {
                    TEventArg& _e = static_cast<TEventArg&>(e);
                    _fnHookCall->Invoke(_instance.lock().get(), sender, _e);
                }
            }
            virtual void operator()(void* sender, TEventArg& arg) { if (!_instance.expired()) _fnHookCall->Invoke(_instance.lock().get(), sender, arg); }
            virtual bool IsEqual(void* object, void* func) const { return (object == _instance.lock().get() && func == (void*)_fnHookGenericPtr); }
            virtual bool IsSame(EventHook<TEventArg>* _delegate) const { return _delegate->getHash() == getHash() && ((EventReflectionHook*)_delegate)->_instance.lock().get() == _instance.lock().get(); }
            virtual void SetThreadSafe() override { isThreadSafe = true; }
            virtual bool IsThreadSafe() const override { return isThreadSafe; }
            virtual bool IsValid() override { return _instance.lock().get() != nullptr; }

            virtual void Detach() override
            {
                _instance.reset();
                EventHook<TEventArg>::Detach();
            }

            std::weak_ptr<TInstance> _instance;
            const System::Reflection::Method* _fnHookCall;

            bool isThreadSafe : 1;
        };

        struct EventGenericReflectionHook : public EventHook<EventArg>
        {
            explicit EventGenericReflectionHook(const System::Reflection::Method* ec, System::Reflection::Variant const& hooker) : EventHook<EventArg>(), _instance(hooker), _fnHookCall(ec), isThreadSafe(false) {}
            virtual ~EventGenericReflectionHook() {}

            virtual size_t getHash() const override
            {
                size_t hash = _instance.GetNativeTypeHash();
                System::hash_combine(hash, size_t(reinterpret_cast<intptr_t>(reinterpret_cast<void const* const&>(_fnHookCall))));
                return hash;
            }
            virtual void* getInstancePtr() const override { return (void*)_instance.toPointer(); }

            virtual std::string ToString() const override { return "EventReflectionHook"; }
            void operator()(void* sender, EventArg& e) const override final
            {
                _fnHookCall->Invoke(_instance, sender, e);
            }
            virtual bool IsEqual(void* object, void* func) const override { return (object == _instance.toPointer() && func == (void*)_fnHookCall); }
            virtual bool IsSame(EventHook<EventArg>* _delegate) const override { return _delegate->getHash() == getHash() && ((EventGenericReflectionHook*)_delegate)->_instance.toPointer() == _instance.toPointer(); }
            virtual void SetThreadSafe() override { isThreadSafe = true; }
            virtual bool IsThreadSafe() const override { return isThreadSafe; }
            virtual bool IsValid() override { return _instance.GetBase() != nullptr; }
            bool IsGeneric() override final { return true; }

            virtual void Detach() override
            {
                _instance = System::Reflection::Variant::Empty;
                EventHook<>::Detach();
            }

            mutable System::Reflection::Variant _instance;
            const System::Reflection::Method* _fnHookCall;

            Meta(Disable) bool isThreadSafe : 1;
        };

        template<typename HookCall, typename T_EventArg>
        struct EventSFuncHook : public EventHook<T_EventArg>
        {
            explicit EventSFuncHook(HookCall ec) : EventHook<T_EventArg>(), _fnHookCall(ec) {}
            virtual ~EventSFuncHook() {}

            virtual size_t getHash() const override { return size_t(reinterpret_cast<intptr_t>(reinterpret_cast<void* const&>(_fnHookGenericPtr))); }
            virtual void* getInstancePtr() const override { return nullptr; }

            virtual std::string ToString() const override { return "EventSFuncHook"; }
            void operator()(void* sender, EventArg& e) const override final
            {
                T_EventArg& _e = static_cast<T_EventArg&>(e);
                _fnHookCall(sender, _e);
            }
            virtual void operator()(void* sender, T_EventArg& arg) { _fnHookCall(sender, arg); }
            virtual bool IsThreadSafe() const override { return true; }
            virtual bool IsValid() override { return true; }

            virtual void Detach() override
            {
                EventHook<T_EventArg>::Detach();
            }

#if defined(WIN32) && _MSC_VER < 1900 // Not support "Generalized lambda capture"
            virtual bool IsEqual(void* /*object*/, void* func) const { assert(false); return false; }
            virtual bool IsSame(EventHook<T_EventArg>* _delegate) const { return _delegate->getHash() == getHash(); }

            HookCall _fnHookCall; // Not Supported "Unrestricted unions"
#else
            virtual bool IsEqual(void* /*object*/, void* func) const { return (func == (void*)_fnHookGenericPtr); }
            virtual bool IsSame(EventHook<T_EventArg>* _delegate) const { return _delegate->getHash() == getHash(); }

            union
            {
                HookCall _fnHookCall;
                void* _fnHookGenericPtr;
            };
#endif
        };

        struct DelegateObject : public IDelegateObject
        {
            typedef std::set<System::Events::IEventHook*> DelegateSet;
            DelegateSet mDelegateStore;

            virtual ~DelegateObject()
            {
                for (DelegateSet::iterator iDelegate = mDelegateStore.begin(); iDelegate != mDelegateStore.end(); ++iDelegate)
                {
                    (*iDelegate)->Detach();
                    delete (*iDelegate);
                }
                mDelegateStore.clear();
            }

            template <typename H, typename C, typename T_EventArg>
            EventHook<T_EventArg>*
                AttachHook(H _HookCall, C caller, T_EventArg /*fake*/)
            {
                auto hook = new EventFuncHook<C, H, T_EventArg>(_HookCall, caller);
                mDelegateStore.insert(hook);
                hook->SetRefMgr(this);
                return hook;
            }

            template <typename H, typename T_EventArg>
            EventHook<T_EventArg>*
                AttachHook(H _HookCall, T_EventArg /*fake*/)
            {
                auto hook = new EventSFuncHook<H, T_EventArg>(_HookCall);
                mDelegateStore.insert(hook);
                hook->SetRefMgr(this);
                return hook;
            }

            template <typename H, typename C, typename T_EventArg>
            EventHook<T_EventArg>*
                operator()(H _HookCall, C caller, T_EventArg /*fake*/)
            {
                auto hook = new EventFuncHook<C, H, T_EventArg>(_HookCall, caller);
                mDelegateStore.insert(hook);
                hook->SetRefMgr(this);
                return hook;
            }

            template <typename H, typename T_EventArg>
            EventHook<T_EventArg>*
                operator()(H _HookCall, T_EventArg /*fake*/)
            {
                auto hook = new EventSFuncHook<H, T_EventArg>(_HookCall);
                mDelegateStore.insert(hook);
                hook->SetRefMgr(this);
                return hook;
            }

            template<typename T, typename ARG >
            EventHook<ARG>* operator()(void(T::*Func)(void*, ARG&))
            {
                return AttachHook(Func, (T*)this, ARG());
            }

            void
                DetachHook(System::Events::IEventHook* hook)
            {
                hook->Detach();
                Invalidate(hook);
            }

            void
                Invalidate(System::Events::IEventHook* hook)
            {
                mDelegateStore.erase(hook);
            }
        };

        struct EventUnregisterException
        {
            
        };

        struct DummyMutex
        {
            void lock() {}
            void unlock() {}
        };

        template <typename T_EventArg = EventArg, typename I_MUTEX = std::recursive_mutex>
        struct EventHandler : Collection::HookList< EventHook<T_EventArg> >, public IEventHandler
        {
            I_MUTEX i_lock;

			typedef EventHook<T_EventArg> ImplHook;
            typedef Collection::HookList< EventHook<T_EventArg> > Impl;
            std::vector<typename Impl::HookIterator*> m_iHookDeeps;

            explicit EventHandler() : Impl()
            {
            }

            virtual ~EventHandler() 
            {
                Invalidate(); // delete later in ~HookList (only remove referencies)
            }

            virtual void Lock() override
            {
                i_lock.lock();
            }

            virtual void Unlock() override
            {
                i_lock.unlock();
            }

            virtual std::string ToString() const { return "EventHandler"; }

            inline void ClearNonThreadSafeCalls()
            {
                Lock();
                for ( typename Impl::HookIterator iHook = Impl::begin(); iHook != Impl::end(); )
                {
                    if (!(*iHook)->IsThreadSafe() || !(*iHook)->IsValid())
                    {
                        for (typename Impl::HookIterator* m_iHook : m_iHookDeeps)
                            if ((*m_iHook) != Impl::end() && (*m_iHook) == iHook)
                                ++(*m_iHook);

                        EventHook<T_EventArg>* delegatePtr = *iHook;
                        this->m_list.erase(iHook++);
                        delete delegatePtr;
                    }
                    else
                        ++iHook;
                }
                Unlock();
            }

			void AddHandler(System::Events::IEventHook* handle) override final
            {
				ImplHook* eventHandle = dynamic_cast<ImplHook*>(handle);
				(*(Impl*)this) += eventHandle;
            }

			void RemoveHandler(System::Events::IEventHook* handle) override final
            {
				(*this) -= handle;
			}

            template<typename T>
            EventHook<T_EventArg>* operator()(T* instance, void(T::*Func)(void*, T_EventArg&))
            {
                auto eventhandler = new EventFuncHook<T, void(T::*)(void*, T_EventArg&), T_EventArg>(Func, instance);
                (*(Impl*)this) += eventhandler;
                return eventhandler;
            }

            virtual void operator-=(EventHook<T_EventArg>* _delegate) override
            {
                Lock();

                for ( typename Impl::HookIterator iHook = Impl::begin(); iHook != Impl::end(); )
                {
                    if (!(*iHook)->IsValid() || (*iHook)->IsSame(_delegate))
                    {
                        for (typename Impl::HookIterator* m_iHook : m_iHookDeeps)
                            if ((*m_iHook) != Impl::end() && (*m_iHook) == iHook)
                                ++(*m_iHook);

                        EventHook<T_EventArg>* delegatePtr = *iHook;
                        this->m_list.erase(iHook++);
                        delegatePtr->Detach();
                        delete delegatePtr;
                    }
                    else
                        ++iHook;
                }

                delete _delegate;
                Unlock();
            }

            virtual void operator+=(EventHook<T_EventArg>* _delegate) override
            {
                Impl:: operator+=(_delegate);
                _delegate->Attach(this);
            }

            //virtual void operator+=(IEventHook* _delegate)
            //{
            //    auto event = dynamic_cast<EventHook<T_EventArg>*>(_delegate);
            //    if (!event)
            //        throw std::runtime_error("invalid eventhandler");
            //    Impl:: operator+=(event);
            //    _delegate->Attach(this);
            //}

            // This not delete delegate (internal call delegate invalidate from handler)
            void operator-=(IEventHook* _delegate) override
            {
                Lock();

                for (typename Impl::HookIterator iHook = Impl::begin(); iHook != Impl::end();)
                {
                    // @ToDo: Because Lambda function ptr is not unique required rework remove methods!!!
                    if ((*iHook) == _delegate) //->Equal((EventHook<T_EventArg>*)_delegate))
                    {
                        for (typename Impl::HookIterator* m_iHook : m_iHookDeeps)
                            if ((*m_iHook) != Impl::end() && (*m_iHook) == iHook)
                                ++(*m_iHook);

                        this->m_list.erase(iHook++);
                    }
                    else
                        ++iHook;
                }
                Unlock();
            }

            EventHook<T_EventArg>* operator+=(std::function<void(void*, T_EventArg&)> f)
            {
                auto __Hook = new EventSFuncHook<decltype(f), T_EventArg>(f);
                (*(Impl*)this) += __Hook;
                return __Hook;
            }

            void operator-=(std::function<void(void*, T_EventArg&)> f)
            {
                auto __Hook = new EventSFuncHook<decltype(f), T_EventArg>(f);
                (*(Impl*)this) -= __Hook;
            }

            virtual void InvalidateEvent(IEventHook* _delegate) override
            {
                Lock();

                for (typename Impl::HookIterator iHook = Impl::begin(); iHook != Impl::end();)
                {
                    if (*iHook == _delegate)
                    {
                        for (typename Impl::HookIterator* m_iHook : m_iHookDeeps)
                            if ((*m_iHook) != Impl::end() && (*m_iHook) == iHook)
                                ++(*m_iHook);

                        this->m_list.erase(iHook++);
                    }
                    else
                        ++iHook;
                }
                Unlock();
            }

            template<typename T_HOOKCALL>
            void Remove(void* obj, T_HOOKCALL func)
            {
                Lock();
                union
                {
                    T_HOOKCALL handle;
                    void* genericHandler;
                } pFunc;

                pFunc.handle = func;

                for (typename Impl::HookIterator iHook = Impl::begin(); iHook != Impl::end();)
                {
                    if (!(*iHook)->IsValid() || (*iHook)->IsEqual(obj, pFunc.genericHandler))
                    {
                        for (typename Impl::HookIterator* m_iHook : m_iHookDeeps)
                            if ((*m_iHook) != Impl::end() && (*m_iHook) == iHook)
                                ++(*m_iHook);

                        EventHook<T_EventArg>* delegatePtr = *iHook;
                        this->m_list.erase(iHook++);
                        delete delegatePtr;
                    }
                    else
                        ++iHook;
                }
                Unlock();
            }

            inline void operator()(void* sender, T_EventArg& arg)
            {
                if ( !this->empty() )
                {
                    Lock();
                    typename Impl::HookIterator m_iHook = Impl::begin();
                    m_iHookDeeps.push_back(&m_iHook);
                    for ( ; m_iHook != Impl::end(); )
                    {
                        auto currHook = m_iHook++;
                        try
                        {
                            (**(currHook))(sender, arg);
                        }
                        catch ( EventUnregisterException )
                        {
                            (*this) -= (IEventHook*)*(currHook);
                            delete (IEventHook*)*(currHook);
                        }
                    }
                    m_iHookDeeps.pop_back();
                    Unlock();
                }
            }

        protected:
            void Invalidate()
            {
                for (typename Impl::HookIterator iHook = Impl::begin(); iHook != Impl::end(); ++iHook)
                    (*Impl::begin())->Invalidate();

                for (typename Impl::HookIterator* m_iHook : m_iHookDeeps)
                    (*m_iHook) = Impl::end();
            }
        };

        template <typename T_EventArg, typename T_MUTEX>
        struct EventHandlerThreadSafe : EventHandler < T_EventArg, DummyMutex>
        {
            T_MUTEX ace_lock;

            explicit EventHandlerThreadSafe() : EventHandler< T_EventArg, DummyMutex >()
            {
            }

            virtual ~EventHandlerThreadSafe() {}
            virtual std::string ToString() const override { return "EventHandlerThreadSafe"; }

            virtual void Lock() override
            {
                ace_lock.lock();
            }

            virtual void Unlock() override
            {
                ace_lock.unlock();
            }
        };
    };
};

namespace System
{
    template <typename Function> struct eventhandler_traits;

    template <typename ClassType, typename ReturnType, typename T_EventArg>
    struct eventhandler_traits<ReturnType(ClassType::*)(void*, T_EventArg) const> {
        using class_type = ClassType;
        using event_arg_type = typename std::remove_cv<typename std::remove_reference<T_EventArg>::type>::type;
        using return_type = ReturnType;
    };

    template <typename ClassType, typename ReturnType, typename T_EventArg>
    struct eventhandler_traits<ReturnType(ClassType::*)(void*, T_EventArg)> {
        using class_type = ClassType;
        using event_arg_type = typename std::remove_cv<typename std::remove_reference<T_EventArg>::type>::type;
        using return_type = ReturnType;
    };

    template <typename ReturnType, typename T_EventArg>
    struct eventhandler_traits<ReturnType(void*, T_EventArg)> {
        using event_arg_type = typename std::remove_cv<typename std::remove_reference<T_EventArg>::type>::type;
        using return_type = ReturnType;
    };

    template <typename TMethod, typename TInstance>
    auto make_shared_eventhandler(TMethod f, TInstance* _instance) ->
        std::shared_ptr<Events::EventHook<typename eventhandler_traits<TMethod>::event_arg_type>>
    {
        return std::make_shared<Events::EventFuncHook<TInstance*, TMethod, typename eventhandler_traits<TMethod>::event_arg_type>>(f, _instance);
    }

    template <typename TMethod, typename TInstance>
    auto make_shared_eventhandler(TMethod f, std::shared_ptr<TInstance> const& _instance) ->
        std::shared_ptr<Events::EventHook<typename eventhandler_traits<TMethod>::event_arg_type>>
    {
        return std::make_shared<Events::EventMethodHook<TInstance, TMethod, typename eventhandler_traits<TMethod>::event_arg_type>>(f, _instance);
    }

	template <typename TMethod, typename TInstance>
	auto make_shared_eventhandler(TMethod f, std::weak_ptr<TInstance> const& _instance) ->
		std::shared_ptr<Events::EventHook<typename eventhandler_traits<TMethod>::event_arg_type>>
	{
		return std::make_shared<Events::EventMethodHook<TInstance, TMethod, typename eventhandler_traits<TMethod>::event_arg_type>>(f, _instance);
	}

    template<typename T>
    auto make_shared_eventhandler(T* f) ->
        typename std::enable_if<std::is_function<T>::value, std::shared_ptr<Events::EventHook<typename eventhandler_traits<T>::event_arg_type>>>::type
    {
        return std::make_shared<Events::EventSFuncHook<T*, typename eventhandler_traits<T>::event_arg_type>>(f);
    }

    template<typename T>
    auto make_shared_eventhandler(T const& f) ->
        typename std::enable_if<!std::is_function<decltype(&T::operator())>::value, std::shared_ptr<Events::EventHook<typename eventhandler_traits<decltype(&T::operator())>::event_arg_type>>>::type
    {
        return std::make_shared<Events::EventSFuncHook<T, typename eventhandler_traits<decltype(&T::operator())>::event_arg_type>>(f);
    }


    template <typename TMethod, typename TInstance>
    auto make_eventhandler(TMethod f, TInstance* _instance) ->
        Events::EventHook<typename eventhandler_traits<TMethod>::event_arg_type>*
    {
        return new Events::EventFuncHook<TInstance*, TMethod, typename eventhandler_traits<TMethod>::event_arg_type>(f, _instance);
    }

    template <typename TMethod, typename TInstance>
    auto make_eventhandler(TMethod f, std::shared_ptr<TInstance> const& _instance) ->
        Events::EventHook<typename eventhandler_traits<TMethod>::event_arg_type>*
    {
        return new Events::EventMethodHook<TInstance, TMethod, typename eventhandler_traits<TMethod>::event_arg_type>(f, _instance);
    }

	template <typename TMethod, typename TInstance>
	auto make_eventhandler(TMethod f, std::weak_ptr<TInstance> const& _instance) ->
		Events::EventHook<typename eventhandler_traits<TMethod>::event_arg_type>*
	{
		return new Events::EventMethodHook<TInstance, TMethod, typename eventhandler_traits<TMethod>::event_arg_type>(f, _instance);
	}

    template<typename T>
    auto make_eventhandler(T* f) ->
        typename std::enable_if<std::is_function<T>::value, Events::EventHook<typename eventhandler_traits<T>::event_arg_type>*>::type
    {
        return new Events::EventSFuncHook<T*, typename eventhandler_traits<T>::event_arg_type>(f);
    }

    template<typename T>
    auto make_eventhandler(T const& f) ->
        typename std::enable_if<!std::is_function<decltype(&T::operator())>::value, Events::EventHook<typename eventhandler_traits<decltype(&T::operator())>::event_arg_type>*>::type
    {
        return new Events::EventSFuncHook<T, typename eventhandler_traits<decltype(&T::operator())>::event_arg_type>(f);
    }

    // Reflection bind
    inline Events::IEventHook* make_eventhandler(const System::Reflection::Method* _method, System::Reflection::Variant const& _instance)
    {
        return new Events::EventGenericReflectionHook(_method, _instance);
    }

	inline std::shared_ptr<Events::IEventHook> make_shared_eventhandler(const System::Reflection::Method* _method, System::Reflection::Variant const& _instance)
	{
		return std::make_shared<Events::EventGenericReflectionHook>(_method, _instance);
	}
}

#endif // _CORE_EVENTS_H_
