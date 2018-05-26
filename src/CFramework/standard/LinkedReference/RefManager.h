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

#ifndef _REFMANAGER_H
#define _REFMANAGER_H

//=====================================================

#include "standard/LinkedReference/Reference.h"

namespace System::Collection
{
    template <class TO, class FROM>
    class RefManager : public RefManagerBase
    {
    public:

        typedef IteratorLL<Reference<TO, FROM> > iterator;
        iterator iEnd;

        explicit RefManager()
        {
            iFirst.iNext = &iLast;
            iLast.iPrev = &iFirst;
#ifdef _LL_DEBUG
            iFirst.headElement = false;
            iLast.headElement = false;
            iFirst.m_mgr = this;
            iLast.m_mgr = this;
            iLast.beingIterated = NULL;
#endif
        }
        virtual ~RefManager() { clearReferences(); }

#ifdef _LL_DEBUG
        iterator begin() { return iterator(getFirst(), this); }
        iterator end() { return iterator(NULL, this); }
        iterator rbegin() { return iterator(getLast(), this); }
        iterator rend() { return iterator(&iFirst, this); }
#else
        iterator begin() { return iterator(getFirst()); }
        iterator& end() { return iEnd; }
        iterator rbegin() { return iterator(getLast()); }
        iterator rend() { return iterator(&iFirst); }
#endif

        void clearReferences()
        {
            Reference<TO, FROM>* ref = getFirst();
            while (!isEmpty())
            {
#ifdef _LL_DEBUG
                ref->headElement = false;
                ref->beingIterated = NULL;
#endif
                ref->invalidate();
                ref->delink();                              // the delink might be already done by invalidate(), but doing it here again does not hurt and insures an empty list
                ref = getFirst();
            }
        }

        inline bool isEmpty() { return iFirst.iNext == &iLast; }
        inline bool isEmpty() const { return iFirst.iNext == &iLast; }

        virtual Reference<TO, FROM>      * getFirst() { return (isEmpty() ? NULL : iFirst.iNext); }
        virtual Reference<TO, FROM> const *getFirst() const { return (isEmpty() ? NULL : iFirst.iNext); }

        virtual Reference<TO, FROM>      * getLast() { return (isEmpty() ? NULL : iLast.iPrev); }
        virtual Reference<TO, FROM> const *getLast() const { return (isEmpty() ? NULL : iLast.iPrev); }

        void insertFirst(Reference<TO, FROM>* pElem)
        {
            iFirst.insertAfter(pElem);
            incSize();
#ifdef _LL_DEBUG
            assert(pElem->iRefFrom);
            assert(pElem->iNext);
            assert(pElem->iPrev);
            pElem->m_mgr = this;
#endif
        }

        void insertLast(Reference<TO, FROM>* pElem)
        {
            iLast.insertBefore(pElem);
            incSize();
#ifdef _LL_DEBUG
            assert(pElem->iRefFrom);
            assert(pElem->iNext);
            assert(pElem->iPrev);
            pElem->m_mgr = this;
#endif
        }

        // Should be called if we really need the size
        virtual size_t getSize() const
        {
            size_t result = 0;
            Reference<TO, FROM> const* ref = getFirst();
            while (ref != iEnd._Ptr)
            {
                ++result;
                ref = ref->next();
            }

            return result;
        }

        virtual void incSize() {}
        virtual void decSize() {}

        virtual void lock_read() {}
        virtual void lock_write() {}
        virtual void unlock() {}
        virtual std::recursive_mutex* Getlock() { return NULL; }

    private:
        Reference<TO, FROM> iFirst;
        Reference<TO, FROM> iLast;
    };
}
#endif
