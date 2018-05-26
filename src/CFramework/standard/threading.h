#ifndef __STANDARD_THREADING_H__
#define __STANDARD_THREADING_H__

#include <atomic>
#include <standard/Platform/Common.h>
#include <boost/thread/tss.hpp>

namespace System
{
    namespace Threading
    {
        template<typename T>
        struct TC_CFRAMEWORK_API TSSObject
        {
            typedef boost::thread_specific_ptr<T> STSS;
            mutable STSS tss;

            T* get() const
            {
                T* ret = tss.get();
                if (!ret)
                {
                    ret = new T;
                    tss.reset(ret);
                }
                return ret;
            }
            T* operator->() const
            {
                T* ret = tss.get();
                if (!ret)
                {
                    ret = new T;
                    tss.reset(ret);
                }
                return ret;
            }
        };

        class TC_CFRAMEWORK_API IReferenceCountedMT
        {
        public:

            //! Constructor.
            IReferenceCountedMT()
            {
                ReferenceCounter = 1;
            }

            //! Destructor.
            virtual ~IReferenceCountedMT()
            {
            }

            virtual void AddReference() const
            {
                ++ReferenceCounter;
            }

            virtual bool RemoveReference() const
            {
                // someone is doing bad reference counting.
                assert(ReferenceCounter > 0);

                --ReferenceCounter;
                if (!getReferenceCount())
                {
                    delete this;
                    return true;
                }

                return false;
            }

            //! Get the reference count.
            /** \return Current value of the reference counter. */
            int getReferenceCount() const
            {
                return ReferenceCounter;
            }


        private:

            //! The reference counter. Mutable to do reference counting on const objects.
            mutable std::atomic<int> ReferenceCounter;
        };


        class TC_CFRAMEWORK_API IUnsafeReferenceCounted
        {
        public:

            //! Constructor.
            IUnsafeReferenceCounted()
            {
                ReferenceCounter = 1;
            }

            //! Destructor.
            virtual ~IUnsafeReferenceCounted()
            {
            }

            virtual void AddReference() const
            {
                ++ReferenceCounter;
            }

            virtual bool RemoveReference() const
            {
                // someone is doing bad reference counting.
                assert(ReferenceCounter > 0);

                --ReferenceCounter;
                if (!getReferenceCount())
                {
                    delete this;
                    return true;
                }

                return false;
            }

            //! Get the reference count.
            /** \return Current value of the reference counter. */
            int getReferenceCount() const
            {
                return ReferenceCounter;
            }


        private:

            //! The reference counter. Mutable to do reference counting on const objects.
            mutable int ReferenceCounter;
        };
    }
}

#endif // __STANDARD_THREAING_H__