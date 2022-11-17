#pragma once


#include "MMDevice.h"
#include "DeviceBase.h"
#include "PE_Filter.h"
#include <string>

size_t const PELLTF_MAX_NAME_SIZE = 256;
const char* PELLTF_NAME = "PhotonEtc LLTF";

class PELLTF: public CGenericBase<PELLTF>
{
public:
	PELLTF();
	~PELLTF();

	int Initialize();
	int Shutdown();
	int GetWavelength(double* wavelength);
	int SetWavelength(const double wavelength);
	int GetWavelengthRange(double* minimum, double* maximum);
private:
	// TODO: change this to instead look for device files in the 3rdparty or other folder
	std::string confFilename = "M000010653.xml";
	PE_HANDLE handle;
	char systemName[PELLTF_MAX_NAME_SIZE];
};

