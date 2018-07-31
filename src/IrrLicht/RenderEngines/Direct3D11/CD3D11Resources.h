#ifndef __C_D3D11_RESOURCE_H_INCLUDED__
#define __C_D3D11_RESOURCE_H_INCLUDED__

#include "IReferenceCounted.h"

namespace irr
{
    namespace video
    {
        class CD3D11Driver;

        struct D3D11DeviceResource
        {
            D3D11DeviceResource(CD3D11Driver* driver);

            virtual ~D3D11DeviceResource();

            virtual void OnDeviceLost(CD3D11Driver* device) = 0;
            virtual void OnDeviceRestored(CD3D11Driver* device) = 0;

            CD3D11Driver* GetDriver() const { return Driver; }

        protected:

            CD3D11Driver* Driver;
        };
    }
}

#endif //!__C_D3D11_RESOURCE_H_INCLUDED__