#ifndef __STANDARD_COMPONENT_SERVICE_PROVIDER_H__
#define __STANDARD_COMPONENT_SERVICE_PROVIDER_H__

#include <boost/uuid/uuid.hpp>

namespace System
{
    //struct IService
    //{
    //
    //};

    class ServiceContainer
    {
        std::map<boost::uuids::uuid, void*> _container;

    public:

        /// <summary>
        /// Add service for provider. Required Implement __uuid static variable
        /// </summary>
        template<typename TService>
        void Add(TService* service)
        {
            _container[TService::__uuid] = reinterpret_cast<void*>(service);
        }

        /// <summary>
        /// Remove service from provider. Required Implement __uuid static variable
        /// </summary>
        template<typename TService>
        void Remove()
        {
            _container.erase(TService::__uuid);
        }

        template<typename TService>
        TService* Get() const
        {
            std::map<boost::uuids::uuid, void*>::const_iterator iService = _container.find(TService::__uuid);
            if (iService == _container.end())
                return nullptr;
            return reinterpret_cast<TService*>(iService->second);
        }

        template<typename TService>
        TService* GetCache(bool clear = false) const
        {
            static TService* __cached = nullptr;
            if (clear)
                __cached = nullptr;
            if (!__cached)
                __cached = Get<TService>();
            return __cached;
        }
    };
}

#endif // !__STANDARD_COMPONENT_SERVICE_PROVIDER_H__
