#include "CD3D11Resources.h"
#include "CD3D11Driver.h"

irr::video::D3D11DeviceResource::D3D11DeviceResource(CD3D11Driver * driver)
    : Driver(driver)
{
    driver->AddResourceObject(this);
}

irr::video::D3D11DeviceResource::~D3D11DeviceResource()
{
    Driver->RemoveResourceObject(this);
}
