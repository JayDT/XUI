#include "Precompiled.h"
#include <mutex>

System::Reflection::meta::InvokableSignature & System::Reflection::meta::MethodDesc::GetInvokableSignature() const
{
    if (m_invokeSignatures.empty() && !m_argHashStorage.empty())
    {
	    static std::mutex __lock;
        __lock.lock();
        if (m_invokeSignatures.empty())
        {
            m_invokeSignatures.resize(m_argHashStorage.size());
            for (int i = 0; i < m_argHashStorage.size(); ++i)
            {
                const Type& type = System::Reflection::type_of(m_argHashStorage[i]);
                m_invokeSignatures[i] = &type;
            }
        }
        __lock.unlock();
    }

    return m_invokeSignatures;
}
