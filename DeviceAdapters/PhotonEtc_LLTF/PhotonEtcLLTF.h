#pragma once


#include "MMDevice.h"
#include "DeviceBase.h"
#include "PE_Filter.h"
#include <string>

size_t const PhotonEtcLLTF_MAX_NAME_SIZE = 256;
const char* const PhotonEtcLLTF_NAME = "PhotonEtc LLTF";
const char* const PhotonEtcLLTF_DESCRIPTION = "Photon etc Laser Line Tunable Filter";

std::vector<std::string> status_messages = {
	"PE_SUCCESS",
	"PE_INVALID_HANDLE",
	"PE_FAILURE",
	"PE_MISSING_CONFIGFILE",
	"PE_INVALID_CONFIGURATION",
	"PE_INVALID_WAVELENGTH",
	"PE_MISSSING_HARMONIC_FILTER",
	"PE_INVALID_FILTER",
	"PE_UNKNOWN",
	"PE_INVALID_GRATING",
	"PE_INVALID_BUFFER",
	"PE_INVALID_BUFFER_SIZE",
	"PE_UNSUPPORTED_CONFIGURATION",
	"PE_NO_FILTER_CONNECTED"
};

class PhotonEtcLLTF: public CGenericBase<PhotonEtcLLTF>
{
public:
	PhotonEtcLLTF();
	~PhotonEtcLLTF();

	int Initialize();
	int Shutdown();
	void GetName(char* name) const;
	bool Busy();

	int onWavelength(MM::PropertyBase* pProp, MM::ActionType eAct);
	int onConfFile(MM::PropertyBase* pProp, MM::ActionType eAct);
private:
	// TODO: change this to instead look for device files in the 3rdparty or other folder
	PE_HANDLE handle = NULL;
	char systemName[PhotonEtcLLTF_MAX_NAME_SIZE];
};

