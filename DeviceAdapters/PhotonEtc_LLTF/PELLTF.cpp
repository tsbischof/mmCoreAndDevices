#include "PELLTF.h"


MODULE_API void InitializeModuleData()
{
	RegisterDevice(PELLTF_NAME, MM::GenericDevice, PELLTF_NAME);
}

MODULE_API void DeleteDevice(MM::Device* pDevice)
{
	delete pDevice;
}

MODULE_API MM::Device* CreateDevice(const char* deviceName)
{
	return 0;
}

PELLTF::PELLTF() {
	PE_Create(this->confFilename.c_str(), &this->handle);
	PE_GetSystemName(this->handle, 0, this->systemName, PELLTF_MAX_NAME_SIZE);
}

PELLTF::~PELLTF() {}


int PELLTF::Initialize() {
	PE_Open(this->handle, this->systemName);
	return DEVICE_OK;
}

int PELLTF::Shutdown() {
	PE_Close(this->handle);
	PE_Destroy(this->handle);
	return DEVICE_OK;
}

int PELLTF::GetWavelength(double* wavelength) {
	PE_GetWavelength(this->handle, wavelength);
	return DEVICE_OK;
}

int PELLTF::SetWavelength(const double wavelength) {
	PE_SetWavelength(this->handle, wavelength);
	return DEVICE_OK;
}

int PELLTF::GetWavelengthRange(double* minimum, double* maximum) {
	PE_GetWavelengthRange(this->handle, minimum, maximum);
	return DEVICE_OK;
}