/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
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

#ifndef _REFERENCE_H
#define _REFERENCE_H

#ifdef WIN32
#   define _LL_DEBUG
#endif

#include "standard/Platform/Common.h"

namespace System::Collection
{
    class RefManagerBase
    {
    };

    //=====================================================

    struct BaseReference
    {
        explicit BaseReference()
        {
        }

        virtual ~BaseReference()
        {
        }

        virtual void invalidate(bool lock = true) = 0;
    };

    template<class TO, class FROM>
    class Reference : public BaseReference
    {
    public:

        TO * iRefTo;
        FROM* iRefFrom;

        Reference<TO, FROM>* iNext;
        Reference<TO, FROM>* iPrev;
#ifdef _LL_DEBUG
        bool headElement;
        void* beingIterated;
        std::string m_unlinkTrace;
        RefManagerBase* m_mgr;
#endif

        explicit Reference()
            : iRefTo(NULL), iRefFrom(NULL), iNext(NULL), iPrev(NULL)
#ifdef _LL_DEBUG
            , headElement(false), beingIterated(NULL), m_mgr(NULL)
#endif
        {
        }

        virtual ~Reference()
        {
            delink();
            /*#ifdef _LL_DEBUG
                        //if(!headElement && (iNext || iPrev))
                        if (headElement)
                        {
                            printf("Failed to remove reference from linkedlist! %p\n", this);
                            assert(false);
                        }
            #endif*/
        }

        // Create new link
        void link(TO* toObj, FROM* fromObj)
        {
            if (isValid())
                unlink();

#ifdef _LL_DEBUG
            assert(iRefTo == NULL);
            assert(fromObj);                                // fromObj MUST not be NULL
            assert(!headElement);
#endif

            if (toObj != NULL)
            {
                iRefTo = toObj;
                iRefFrom = fromObj;
                lock_write();
                this->targetObjectBuildLink();
                unlock();
            }
        }

        // We don't need the reference anymore. Call comes from the refFrom object
        // Tell our refTo object, that the link is cut
        virtual void unlink(bool lock = true)
        {
#ifdef _LL_DEBUG
            assert(!beingIterated);
            //assert(!headElement);
#endif
            if (lock) {
                lock_write();
            }
            this->targetObjectDestroyLink();
            delink();
            if (lock) {
                unlock();
            }
            iRefTo = NULL;
            iRefFrom = NULL;
            /*#ifdef _LL_DEBUG
                        if(!headElement && (iNext || iPrev))
                        {
                            printf("Failed to remove reference from linkedlist! %p\n", this);
                            assert(false);
                        }
            #endif*/
        }

        // Link is invalid due to destruction of referenced target object. Call comes from the refTo object
        // Tell our refFrom object, that the link is cut
        void invalidate(bool lock = true) override                                  // the iRefFrom MUST remain!!
        {
            if (lock) {
                lock_write();
            }
            this->sourceObjectDestroyLink();
            delink();
            if (lock) {
                unlock();
            }
            iRefTo = NULL;
            /*#ifdef _LL_DEBUG
                        if(!headElement && (iNext || iPrev))
                        {
                            printf("Failed to remove reference from linkedlist! %p\n", this);
                            assert(false);
                        }
            #endif*/
        }

        inline bool isValid() const { return iRefTo != nullptr; } // Only check the iRefTo 
        inline bool isInList() const { return iNext && iPrev; }

        inline Reference<TO, FROM>      * next() { return iNext && iNext->iNext ? iNext : NULL; }
        inline Reference<TO, FROM> const *next() const { return iNext && iNext->iNext ? iNext : NULL; }
        inline Reference<TO, FROM>      * prev() { return iPrev && iPrev->iPrev ? iPrev : NULL; }
        inline Reference<TO, FROM> const *prev() const { return iPrev && iPrev->iPrev ? iPrev : NULL; }

        inline void delink()
        {
#ifdef _LL_DEBUG
            assert(!headElement);
#endif
            if (isInList())
            {
                iNext->iPrev = iPrev;
                iPrev->iNext = iNext;
                iNext = NULL;
                iPrev = NULL;
#ifdef _LL_DEBUG
                assert(!headElement);
#endif
            }
        }

        inline void insertBefore(Reference<TO, FROM>* pElem)
        {
#ifdef _LL_DEBUG
            assert(!pElem->headElement);
#endif
            pElem->iNext = this;
            pElem->iPrev = iPrev;
            iPrev->iNext = pElem;
            iPrev = pElem;
        }

        inline void insertAfter(Reference<TO, FROM>* pElem)
        {
#ifdef _LL_DEBUG
            assert(!pElem->headElement);
#endif
            pElem->iPrev = this;
            pElem->iNext = iNext;
            iNext->iPrev = pElem;
            iNext = pElem;
        }

        TO* operator->() const { return iRefTo; }
        inline TO* getTarget() const { return iRefTo; }

        inline FROM* getSource() const { return iRefFrom; }
        inline FROM* getSource() { return iRefFrom; }

        virtual void lock_read() {}
        virtual void lock_write() {}
        virtual void unlock() {}
        virtual std::recursive_mutex* Getlock() { return NULL; }

    protected:

        // Tell our refTo (target) object that we have a link
        virtual void targetObjectBuildLink() {};

        // Tell our refTo (taget) object, that the link is cut
        virtual void targetObjectDestroyLink() {};

        // Tell our refFrom (source) object, that the link is cut (Target destroyed)
        virtual void sourceObjectDestroyLink() {};
    };

    //=====================================================

    struct BaseIteratorLL
    {
        virtual ~BaseIteratorLL()
        {
        }

        virtual void* GetHash() = 0;
        virtual bool IsMynode(BaseIteratorLL& ptr) = 0;
        virtual bool IsMynode(void* ptr) { return ptr == GetHash(); }
        virtual void stepForward() = 0;
    };

    template<class _Ty>
    class IteratorLL : public BaseIteratorLL
    {
    public:

        typedef std::bidirectional_iterator_tag iterator_category;
        typedef _Ty value_type;
        typedef ptrdiff_t difference_type;
        typedef ptrdiff_t distance_type;
        typedef _Ty* pointer;
        typedef _Ty const *const_pointer;
        typedef _Ty& reference;
        typedef _Ty const& const_reference;

        pointer _Ptr;                               // pointer to node
#ifdef _LL_DEBUG
        RefManagerBase* m_mgr;
#endif

        virtual void* GetHash() { return (void*)_Ptr; }
        virtual bool IsMynode(BaseIteratorLL& ptr) { return GetHash() == ptr.GetHash(); }
        virtual void stepForward() { ++(*this); }

        IteratorLL()
            : _Ptr(0)
#ifdef _LL_DEBUG
            , m_mgr(0)
#endif
        {                                           // construct with null node pointer
        }

        IteratorLL(pointer _Pnode
#ifdef _LL_DEBUG
            , RefManagerBase* mgr
#endif
        )
            : _Ptr(_Pnode)
        {                                           // construct with node pointer _Pnode
#ifdef _LL_DEBUG
            m_mgr = mgr;
            if (_Pnode)
                _Pnode->beingIterated = this;
#endif
        }

        virtual ~IteratorLL()
        {                                           // construct with null node pointer
#ifdef _LL_DEBUG
            if (_Ptr)
                _Ptr->beingIterated = NULL;
#endif
        }


        IteratorLL& operator=(IteratorLL const &_Right)
        {
#ifdef _LL_DEBUG
            if (_Ptr)
                _Ptr->beingIterated = NULL;
            if (_Right._Ptr)
                _Right._Ptr->beingIterated = this;
            if (_Right._Ptr)
            {
                if (!m_mgr) // First init
                    m_mgr = _Right._Ptr->m_mgr;
                assert(m_mgr == _Right._Ptr->m_mgr);
            }
            return (*this) = _Right._Ptr;
#else
            return (*this) = _Right._Ptr;
#endif
        }

        IteratorLL& operator=(const_pointer const &_Right)
        {
            _Ptr = (pointer)_Right;
            return (*this);
        }

        reference operator*()
        {                                           // return designated value
#ifdef _LL_DEBUG
            assert(_Ptr);
#endif
            return *_Ptr;
        }

        pointer operator->()
        {                                           // return pointer to class object
            return _Ptr;
        }

        IteratorLL& operator++()
        {                                           // preincrement
#ifdef _LL_DEBUG
            if (_Ptr)
                _Ptr->beingIterated = NULL;
            _Ptr = _Ptr->next();
            if (_Ptr)
            {
                assert(m_mgr == _Ptr->m_mgr);
                _Ptr->beingIterated = this;
            }
            return (*this);
#else
            _Ptr = _Ptr->next();
            return(*this);
#endif
        }

        IteratorLL operator++(int)
        {                                           // postincrement
            IteratorLL _Tmp = *this;
            ++(*this);
            return _Tmp;
        }

        IteratorLL& operator--()
        {                                           // predecrement
#ifdef _LL_DEBUG
            if (_Ptr)
                _Ptr->beingIterated = NULL;

            _Ptr = _Ptr->prev();
            if (_Ptr)
            {
                assert(m_mgr == _Ptr->m_mgr);
                _Ptr->beingIterated = this;
            }

            return(*this);
#else
            _Ptr = _Ptr->prev();
            return(*this);
#endif
        }

        IteratorLL operator--(int)
        {                                           // postdecrement
            IteratorLL _Tmp = *this;
            --(*this);
            return _Tmp;
        }

        bool operator==(IteratorLL const &_Right) const
        {                                           // test for iterator equality
#ifdef _LL_DEBUG
            if (_Ptr)
            {
                assert(m_mgr == _Right.m_mgr);
            }
#endif
            return (_Ptr == _Right._Ptr);
        }

        bool operator!=(IteratorLL const &_Right) const
        {                                           // test for iterator inequality
#ifdef _LL_DEBUG
            if (_Ptr)
            {
                assert(m_mgr == _Right.m_mgr);
            }
#endif
            return (_Ptr != _Right._Ptr);
        }

        bool operator==(pointer const &_Right) const
        {                                           // test for pointer equality
            return (_Ptr == _Right);
        }

        bool operator!=(pointer const &_Right) const
        {                                           // test for pointer equality
            return (_Ptr != _Right);
        }

        bool operator==(const_reference _Right) const
        {                                           // test for reference equality
            return (_Ptr == &_Right);
        }

        bool operator!=(const_reference _Right) const
        {                                           // test for reference equality
            return (_Ptr != &_Right);
        }

        pointer _Mynode()
        {                                           // return node pointer
            return (_Ptr);
        }
    };
}

#endif
