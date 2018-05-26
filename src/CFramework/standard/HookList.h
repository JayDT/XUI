#ifndef __CF_HOOKLIST_H__
#define __CF_HOOKLIST_H__

namespace System
{
    namespace Collection
    {
        // simple class for not-modifyable list
        template <typename T>
        class HookList
        {
        public:
            typedef typename std::list<T*> HookContainer;
            typedef typename HookContainer::iterator HookIterator;
        protected:
            HookContainer m_list;
        public:

            explicit HookList() {}
            virtual ~HookList()
            {
                clear();
            }

            virtual void Lock() {}
            virtual void Unlock() {}

            void clear()
            {
                Lock();
                while (!m_list.empty())
                {
                    T* hookPtr = (*m_list.begin());
                    m_list.erase(m_list.begin());
                    delete hookPtr;
                }
                Unlock();
            }

            virtual void operator+=(T* t)
            {
                Lock();
                m_list.push_back(t);
                Unlock();
            }

            virtual void operator-=(T* t)
            {
                Lock();
                m_list.remove(t);
                Unlock();
            }

            HookList<T> & operator+(T* t)
            {
                Lock();
                m_list.push_back(t);
                Unlock();
                return *this;
            }

            HookList<T> & operator-(T* t)
            {
                Lock();
                m_list.remove(t);
                Unlock();
                return *this;
            }

            inline operator bool()
            {
                return !empty();
            }

            void CopyTo(HookList<T>& ref)
            {
                Lock();
                ref.m_list.insert(ref.m_list.end(), m_list.begin(), m_list.end());
                Unlock();
            }

            inline bool empty()
            {
                return m_list.empty();
            }

            inline size_t size()
            {
                return m_list.size();
            }

            inline HookIterator begin()
            {
                return m_list.begin();
            }

            inline HookIterator end()
            {
                return m_list.end();
            }
        };
    }
}

#endif