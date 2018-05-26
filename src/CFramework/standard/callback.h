#pragma once
#ifndef __SYSTEM_ASYNC_CALLBACK_H
#define __SYSTEM_ASYNC_CALLBACK_H

#include <standard\concurrentQueue.h>
 /// ------------ BASE CLASSES ------------

namespace System
{
    namespace Async
    {
        template<typename Param>
        struct IFuncHook
        {
            virtual void operator()(Param& arg) = 0;
        };

        template<typename Hooker, typename HookCall, typename Param>
        struct FuncHook : public IFuncHook<Param>
        {
            explicit FuncHook(HookCall ec, Hooker hooker) :  _hookCall(hooker), _fnHookCall(ec) {}
            virtual ~FuncHook() {}

            virtual unsigned int getHash() const { return 1; }
            virtual std::string ToString() { return "EventFuncHook"; }

            virtual void operator()(Param& arg) { if (_hookCall) (_hookCall->*_fnHookCall)(arg); }

            Hooker   _hookCall;
            union
            {
                HookCall _fnHookCall;
                void* _fnHookGenericPtr;
            };
        };

        // one pointer callback //

        template<class Class, typename ParamType1 = void, typename ParamType2 = void, typename ParamType3 = void, typename ParamType4 = void>
        class _PCallback
        {
        protected:

            typedef void (Class::*Method)(ParamType1*, ParamType2, ParamType3, ParamType4);
            Class *m_object;
            Method m_method;
            ParamType1* m_param1;
            ParamType2 m_param2;
            ParamType3 m_param3;
            ParamType4 m_param4;
            void _Execute() { (m_object->*m_method)(m_param1, m_param2, m_param3, m_param4); }

        public:

            _PCallback(Class *object, Method method, ParamType1* param1, ParamType2 param2, ParamType3 param3, ParamType4 param4)
                : m_object(object), m_method(method),
                m_param1(param1), m_param2(param2), m_param3(param3), m_param4(param4)
            {}

            _PCallback(_PCallback<Class, ParamType1, ParamType2, ParamType3, ParamType4> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2), m_param3(cb.m_param3), m_param4(cb.m_param4)
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2, typename ParamType3>
        class _PCallback<Class, ParamType1, ParamType2, ParamType3>
        {
        protected:

            typedef void (Class::*Method)(ParamType1*, ParamType2, ParamType3);
            Class *m_object;
            Method m_method;
            ParamType1* m_param1;
            ParamType2 m_param2;
            ParamType3 m_param3;
            void _Execute() { (m_object->*m_method)(m_param1, m_param2, m_param3); }

        public:
            _PCallback(Class *object, Method method, ParamType1* param1, ParamType2 param2, ParamType3 param3)
                : m_object(object), m_method(method),
                m_param1(param1), m_param2(param2)
            {}

            _PCallback(_PCallback<Class, ParamType1, ParamType2, ParamType3> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2), m_param3(cb.m_param3)
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2>
        class _PCallback<Class, ParamType1, ParamType2>
        {
        protected:

            typedef void (Class::*Method)(ParamType1*, ParamType2);
            Class *m_object;
            Method m_method;
            ParamType1* m_param1;
            ParamType2 m_param2;
            void _Execute() { (m_object->*m_method)(m_param1, m_param2); }

        public:

            _PCallback(Class *object, Method method, ParamType1* param1, ParamType2 param2)
                : m_object(object), m_method(method),
                m_param1(param1), m_param2(param2)
            {}

            _PCallback(_PCallback<Class, ParamType1, ParamType2> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2)
            {}
        };

        template<class Class, typename ParamType1>
        class _PCallback<Class, ParamType1>
        {
        protected:

            typedef void (Class::*Method)(ParamType1*);
            Class *m_object;
            Method m_method;
            ParamType1* m_param1;
            void _Execute() { (m_object->*m_method)(m_param1); }

        public:

            _PCallback(Class *object, Method method, ParamType1* param1)
                : m_object(object), m_method(method),
                m_param1(param1)
            {}

            _PCallback(_PCallback<Class, ParamType1> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1)
            {}
        };

        template<class Class>
        class _PCallback<Class>
        {
        protected:

            typedef void (Class::*Method)();
            Class *m_object;
            Method m_method;
            void _Execute() { (m_object->*m_method)(); }

        public:
            _PCallback(Class *object, Method method)
                : m_object(object), m_method(method)
            {}
            _PCallback(_PCallback<Class> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method)
            {}
        };

        // non pointer callback //

        template<class Class, typename ParamType1 = void, typename ParamType2 = void, typename ParamType3 = void, typename ParamType4 = void>
        class _Callback
        {
        protected:

            typedef void (Class::*Method)(ParamType1, ParamType2, ParamType3, ParamType4);
            Class *m_object;
            Method m_method;
            ParamType1 m_param1;
            ParamType2 m_param2;
            ParamType3 m_param3;
            ParamType4 m_param4;
            void _Execute() { (m_object->*m_method)(m_param1, m_param2, m_param3, m_param4); }

        public:

            _Callback(Class *object, Method method, ParamType1 param1, ParamType2 param2, ParamType3 param3, ParamType4 param4)
                : m_object(object), m_method(method),
                m_param1(param1), m_param2(param2), m_param3(param3), m_param4(param4)
            {}

            _Callback(_Callback<Class, ParamType1, ParamType2, ParamType3, ParamType4> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2), m_param3(cb.m_param3), m_param4(cb.m_param4)
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2, typename ParamType3>
        class _Callback<Class, ParamType1, ParamType2, ParamType3>
        {
        protected:

            typedef void (Class::*Method)(ParamType1, ParamType2, ParamType3);
            Class *m_object;
            Method m_method;
            ParamType1 m_param1;
            ParamType2 m_param2;
            ParamType3 m_param3;
            void _Execute() { (m_object->*m_method)(m_param1, m_param2, m_param3); }

        public:
            _Callback(Class *object, Method method, ParamType1 param1, ParamType2 param2, ParamType3 param3)
                : m_object(object), m_method(method),
                m_param1(param1), m_param2(param2), m_param3(param3)
            {}

            _Callback(_Callback<Class, ParamType1, ParamType2, ParamType3> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2), m_param3(cb.m_param3)
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2>
        class _Callback<Class, ParamType1, ParamType2>
        {
        protected:

            typedef void (Class::*Method)(ParamType1, ParamType2);
            Class *m_object;
            Method m_method;
            ParamType1 m_param1;
            ParamType2 m_param2;
            void _Execute() { (m_object->*m_method)(m_param1, m_param2); }

        public:

            _Callback(Class *object, Method method, ParamType1 param1, ParamType2 param2)
                : m_object(object), m_method(method),
                m_param1(param1), m_param2(param2)
            {}

            _Callback(_Callback<Class, ParamType1, ParamType2> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2)
            {}
        };

        template<class Class, typename ParamType1>
        class _Callback<Class, ParamType1>
        {
        protected:

            typedef void (Class::*Method)(ParamType1);
            Class *m_object;
            Method m_method;
            ParamType1 m_param1;
            void _Execute() { (m_object->*m_method)(m_param1); }

        public:

            _Callback(Class *object, Method method, ParamType1 param1)
                : m_object(object), m_method(method),
                m_param1(param1)
            {}

            _Callback(_Callback<Class, ParamType1> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method),
                m_param1(cb.m_param1)
            {}
        };

        template<class Class>
        class _Callback<Class>
        {
        protected:

            typedef void (Class::*Method)();
            Class *m_object;
            Method m_method;
            void _Execute() { (m_object->*m_method)(); }

        public:
            _Callback(Class *object, Method method)
                : m_object(object), m_method(method)
            {}
            _Callback(_Callback<Class> const& cb)
                : m_object(cb.m_object), m_method(cb.m_method)
            {}
        };

        /// ---- Statics ----

        template<typename ParamType1 = void, typename ParamType2 = void, typename ParamType3 = void, typename ParamType4 = void>
        class _SCallback
        {
        protected:

            typedef void(*Method)(ParamType1, ParamType2, ParamType3, ParamType4);
            Method m_method;
            ParamType1 m_param1;
            ParamType2 m_param2;
            ParamType3 m_param3;
            ParamType4 m_param4;
            void _Execute() { (*m_method)(m_param1, m_param2, m_param3, m_param4); }

        public:

            _SCallback(Method method, ParamType1 param1, ParamType2 param2, ParamType3 param3, ParamType4 param4)
                : m_method(method),
                m_param1(param1), m_param2(param2), m_param3(param3), m_param4(param4)
            {}

            _SCallback(_SCallback<ParamType1, ParamType2, ParamType3, ParamType4> const& cb)
                : m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2), m_param3(cb.m_param3), m_param4(cb.m_param4)
            {}
        };

        template<typename ParamType1, typename ParamType2, typename ParamType3>
        class _SCallback<ParamType1, ParamType2, ParamType3>
        {
        protected:

            typedef void(*Method)(ParamType1, ParamType2, ParamType3);
            Method m_method;
            ParamType1 m_param1;
            ParamType2 m_param2;
            ParamType3 m_param3;
            void _Execute() { (*m_method)(m_param1, m_param2, m_param3); }

        public:
            _SCallback(Method method, ParamType1 param1, ParamType2 param2, ParamType3 param3)
                : m_method(method),
                m_param1(param1), m_param2(param2), m_param3(param3)
            {}
            _SCallback(_SCallback<ParamType1, ParamType2, ParamType3> const& cb)
                : m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2), m_param3(cb.m_param3)
            {}
        };

        template<typename ParamType1, typename ParamType2>
        class _SCallback<ParamType1, ParamType2>
        {
        protected:

            typedef void(*Method)(ParamType1, ParamType2);
            Method m_method;
            ParamType1 m_param1;
            ParamType2 m_param2;
            void _Execute() { (*m_method)(m_param1, m_param2); }

        public:
            _SCallback(Method method, ParamType1 param1, ParamType2 param2)
                : m_method(method),
                m_param1(param1), m_param2(param2)
            {}

            _SCallback(_SCallback<ParamType1, ParamType2> const& cb)
                : m_method(cb.m_method),
                m_param1(cb.m_param1), m_param2(cb.m_param2)
            {}
        };

        template<typename ParamType1>
        class _SCallback<ParamType1>
        {
        protected:

            typedef void(*Method)(ParamType1);
            Method m_method;
            ParamType1 m_param1;
            void _Execute() { (*m_method)(m_param1); }

        public:
            _SCallback(Method method, ParamType1 param1)
                : m_method(method),
                m_param1(param1)
            {}

            _SCallback(_SCallback<ParamType1> const& cb)
                : m_method(cb.m_method),
                m_param1(cb.m_param1)
            {}
        };

        template<>
        class _SCallback<>
        {
        protected:

            typedef void(*Method)();
            Method m_method;
            void _Execute() { (*m_method)(); }

        public:

            _SCallback(Method method)
                : m_method(method)
            {}

            _SCallback(_SCallback<> const& cb)
                : m_method(cb.m_method)
            {}
        };

        /// --------- GENERIC CALLBACKS ----------
        class ICallback
        {
        public:

            virtual void Execute() = 0;
            virtual ~ICallback() {}
        };

        template<class CB>
        class _ICallback : public CB, public ICallback
        {
        public:

            _ICallback(CB const& cb) : CB(cb)
            {}

            void Execute() { CB::_Execute(); }
        };


        template<class Class, typename ParamType1 = void, typename ParamType2 = void, typename ParamType3 = void, typename ParamType4 = void>
        class PCallback : public _ICallback<_PCallback<Class, ParamType1, ParamType2, ParamType3, ParamType4> >
        {
        private:

            typedef _PCallback<Class, ParamType1, ParamType2, ParamType3, ParamType4> P4;

        public:

            PCallback(Class *object, typename P4::Method method, ParamType1* param1, ParamType2 param2, ParamType3 param3, ParamType4 param4)
                : _ICallback<P4>(P4(object, method, param1, param2, param3, param4))
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2, typename ParamType3>
        class PCallback<Class, ParamType1, ParamType2, ParamType3> : public _ICallback<_PCallback<Class, ParamType1, ParamType2, ParamType3> >
        {
        private:

            typedef _PCallback<Class, ParamType1, ParamType2, ParamType3> P3;

        public:

            PCallback(Class *object, typename P3::Method method, ParamType1* param1, ParamType2 param2, ParamType3 param3)
                : _ICallback<P3>(P3(object, method, param1, param2, param3))
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2>
        class PCallback<Class, ParamType1, ParamType2> : public _ICallback<_PCallback<Class, ParamType1, ParamType2> >
        {
        private:

            typedef _PCallback<Class, ParamType1, ParamType2> P2;

        public:

            PCallback(Class *object, typename P2::Method method, ParamType1* param1, ParamType2 param2)
                : _ICallback<P2>(P2(object, method, param1, param2))
            {}
        };

        template<class Class, typename ParamType1>
        class PCallback<Class, ParamType1> : public _ICallback<_PCallback<Class, ParamType1> >
        {
        private:

            typedef _PCallback<Class, ParamType1> P1;

        public:

            PCallback(Class *object, typename P1::Method method, ParamType1* param1)
                : _ICallback<P1>(P1(object, method, param1))
            {}
        };

        template<class Class, typename ParamType1 = void, typename ParamType2 = void, typename ParamType3 = void, typename ParamType4 = void>
        class Callback : public _ICallback<_Callback<Class, ParamType1, ParamType2, ParamType3, ParamType4> >
        {
        private:

            typedef _Callback<Class, ParamType1, ParamType2, ParamType3, ParamType4> C4;

        public:

            Callback(Class *object, typename C4::Method method, ParamType1 param1, ParamType2 param2, ParamType3 param3, ParamType4 param4)
                : _ICallback<C4>(C4(object, method, param1, param2, param3, param4))
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2, typename ParamType3>
        class Callback<Class, ParamType1, ParamType2, ParamType3> : public _ICallback<_Callback<Class, ParamType1, ParamType2, ParamType3> >
        {
        private:

            typedef _Callback<Class, ParamType1, ParamType2, ParamType3> C3;

        public:

            Callback(Class *object, typename C3::Method method, ParamType1 param1, ParamType2 param2, ParamType3 param3)
                : _ICallback<C3>(C3(object, method, param1, param2, param3))
            {}
        };

        template<class Class, typename ParamType1, typename ParamType2>
        class Callback<Class, ParamType1, ParamType2> : public _ICallback<_Callback<Class, ParamType1, ParamType2> >
        {
        private:

            typedef _Callback<Class, ParamType1, ParamType2> C2;

        public:
            Callback(Class *object, typename C2::Method method, ParamType1 param1, ParamType2 param2)
                : _ICallback<C2>(C2(object, method, param1, param2))
            {}
        };

        template<class Class, typename ParamType1>
        class Callback<Class, ParamType1> : public _ICallback<_Callback<Class, ParamType1> >
        {
        private:

            typedef _Callback<Class, ParamType1> C1;

        public:

            Callback(Class *object, typename C1::Method method, ParamType1 param1)
                : _ICallback<C1>(C1(object, method, param1))
            {}
        };

        template<class Class>
        class Callback<Class> : public _ICallback<_Callback<Class> >
        {
        private:

            typedef _Callback<Class> C0;

        public:

            Callback(Class *object, typename C0::Method method)
                : _ICallback<C0>(C0(object, method))
            {}
        };

        template<typename FUNC>
        class _LCallback
        {
        protected:

            FUNC m_method;
            void _Execute()
            {
                m_method();
            }

        public:

            _LCallback(FUNC method)
                : m_method(method)
            {}

            _LCallback(_LCallback<FUNC> const& cb)
                : m_method(cb.m_method)
            {}
        };

        template<typename FUNC>
        class LCallback : public _ICallback<_LCallback<FUNC> >
        {
        public:
            LCallback(FUNC func) : _ICallback<_LCallback<FUNC> >(func) {}
        };

        class InvokeHandler
        {
        public:
            explicit InvokeHandler() : m_active(true) {}
            virtual ~InvokeHandler()
            {
                System::Async::ICallback* _callback = nullptr;
                while (m_addStore.pop(_callback))
                    delete _callback;
                while (m_callbackStore.pop(_callback))
                    delete _callback;
            }

            void SetActive(bool _s) { m_active = _s; }
            bool GetActiveState() { return m_active; }

            template<typename Func>
            InvokeHandler* operator +=(Func fn)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_LCallback<Func> >(System::Async::_LCallback<Func>(fn)));
                return this;
            }

            template<class Class>
            void AddCallbackToQueue(Class *object, void (Class::*method)())
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_Callback<Class> >(System::Async::_Callback<Class>(object, method)));
            }

            template<class Class, typename ParamType1>
            void AddCallbackToQueue(Class *object, void (Class::*method)(ParamType1), ParamType1 param1)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_Callback<Class, ParamType1> >(System::Async::_Callback<Class, ParamType1>(object, method, param1)));
            }

            template<class Class, typename ParamType1, typename ParamType2>
            void AddCallbackToQueue(Class *object, void (Class::*method)(ParamType1, ParamType2), ParamType1 param1, ParamType2 param2)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_Callback<Class, ParamType1, ParamType2> >(System::Async::_Callback<Class, ParamType1, ParamType2>(object, method, param1, param2)));
            }

            template<class Class, typename ParamType1, typename ParamType2, typename ParamType3>
            void AddCallbackToQueue(Class *object, void (Class::*method)(ParamType1, ParamType2, ParamType3), ParamType1 param1, ParamType2 param2, ParamType3 param3)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_Callback<Class, ParamType1, ParamType2, ParamType3> >(System::Async::_Callback<Class, ParamType1, ParamType2, ParamType3>(object, method, param1, param2, param3)));
            }

            template<class Class, typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4>
            void AddCallbackToQueue(Class *object, void (Class::*method)(ParamType1, ParamType2, ParamType3, ParamType4), ParamType1 param1, ParamType2 param2, ParamType3 param3, ParamType4 param4)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_Callback<Class, ParamType1, ParamType2, ParamType3, ParamType4> >(System::Async::_Callback<Class, ParamType1, ParamType2, ParamType3, ParamType4>(object, method, param1, param2, param3, param4)));
            }

            void AddSCallbackToQueue(void(*method)())
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_SCallback<> >(System::Async::_SCallback<>(method)));
            }

            template<typename ParamType1>
            void AddSCallbackToQueue(void(*method)(ParamType1), ParamType1 param1)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_SCallback<ParamType1> >(System::Async::_SCallback<ParamType1>(method, param1)));
            }

            template<typename ParamType1, typename ParamType2>
            void AddSCallbackToQueue(void(*method)(ParamType1, ParamType2), ParamType1 param1, ParamType2 param2)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_SCallback<ParamType1, ParamType2> >(System::Async::_SCallback<ParamType1, ParamType2>(method, param1, param2)));
            }

            template<typename ParamType1, typename ParamType2, typename ParamType3>
            void AddSCallbackToQueue(void(*method)(ParamType1, ParamType2, ParamType3), ParamType1 param1, ParamType2 param2, ParamType3 param3)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_SCallback<ParamType1, ParamType2, ParamType3> >(System::Async::_SCallback<ParamType1, ParamType2, ParamType3>(method, param1, param2, param3)));
            }

            template<typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4>
            void AddSCallbackToQueue(void(*method)(ParamType1, ParamType2, ParamType3, ParamType4), ParamType1 param1, ParamType2 param2, ParamType3 param3, ParamType4 param4)
            {
                if (m_active)
                    m_addStore.push(new System::Async::_ICallback<System::Async::_SCallback<ParamType1, ParamType2, ParamType3, ParamType4> >(System::Async::_SCallback<ParamType1, ParamType2, ParamType3, ParamType4>(method, param1, param2, param3, param4)));
            }

            inline operator bool() const
            {
                return !empty() || !m_addStore.empty();
            }

            inline void operator()()
            {
                Update();
            }

            inline void Update()
            {
                System::Async::ICallback* _callback = nullptr;
                while (m_addStore.pop(_callback))
                    m_callbackStore.push(_callback);

                while (m_callbackStore.pop(_callback))
                {
                    _callback->Execute();
                    delete _callback;
                }

                if (!m_active)
                {
                    while (m_addStore.pop(_callback))
                    {
                        _callback->Execute();
                        delete _callback;
                    }
                }
            }

            inline bool empty() const { return m_callbackStore.empty(); }

        private:
            System::Concurrent::Queue<System::Async::ICallback*> m_callbackStore;
            System::Concurrent::Queue<System::Async::ICallback*> m_addStore;
            bool m_active;
        };
    }
}

#endif

