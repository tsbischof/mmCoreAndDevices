#include "PhotonEtcLLTF.h"

#include <boost/filesystem.hpp>


MODULE_API void InitializeModuleData()
{
	RegisterDevice(PhotonEtcLLTF_NAME, MM::GenericDevice, PhotonEtcLLTF_DESCRIPTION);
}

MODULE_API void DeleteDevice(MM::Device* pDevice)
{
	delete pDevice;
}

MODULE_API MM::Device* CreateDevice(const char* deviceName)
{
	if (deviceName == 0) {
		return 0;
	}
	else if (strcmp(deviceName, PhotonEtcLLTF_NAME) == 0) {
		PhotonEtcLLTF* lltf = new PhotonEtcLLTF();
		return lltf;
	}
	else {
		return 0;
	}
}

PhotonEtcLLTF::PhotonEtcLLTF() {
	std::vector<std::string>confFilePaths = {
		".",
		"C:\\Program Files\\Photon etc\\PHySpecV2\\Devices",
		"C:\\Program Files (x86)\\Photon etc\\PHySpecV2\\Devices"
	};

	CPropertyAction* pAct = new CPropertyAction(this, &PhotonEtcLLTF::onConfFile);
	CreateProperty("Device XML File", "C:\\Program Files\\Photon etc\\PHySpecV2\\Devices\\M000010653.xml", MM::String, false, pAct, true);

	LogMessage("finding XML files");
	// TODO this loop causes a runtime error. Perhaps the version of boost linked here is not correct?
	//for (const auto& confFilePath : confFilePaths) {
	//	LogMessage(confFilePath);
	//	for (const auto& entry : boost::filesystem::directory_iterator(confFilePath)) {
	//		LogMessage(entry.path().filename().string().c_str());
	//		if (entry.path().filename().string().find("M00001") && entry.path().filename().string().rfind("xml")) {
	//			AddAllowedValue("Device XML File", entry.path().filename().string().c_str());
	//		}
	//	}
	//}
}

PhotonEtcLLTF::~PhotonEtcLLTF() {}


int PhotonEtcLLTF::Initialize() {
	PE_STATUS status;
	double wavelength;
	double wavelengthMinimum, wavelengthMaximum;
	char message[1024];
	char confFilename[1024];

	this->GetProperty("Device XML File", confFilename);
	std::snprintf(message, 1024, "confFilename=%s", confFilename);
	LogMessage(message);

	status = PE_Create(confFilename, &this->handle);
	std::snprintf(message, 1024, "handle=%p", this->handle);
	LogMessage(message);

	status = PE_GetSystemName(this->handle, 0, this->systemName, PhotonEtcLLTF_MAX_NAME_SIZE);
	LogMessage(this->systemName);
	CreateProperty("System Name", this->systemName, MM::String, true);

	status = PE_Open(this->handle, this->systemName);	
	std::snprintf(message, 1024, "handle=%p, name=%s, status=%d (%s)", 
		this->handle, this->systemName, status, status_messages[status].c_str()
	);
	LogMessage(message);
	if (status != PE_SUCCESS) {
		return DEVICE_ERR;
	} 

	CPropertyAction* pAct = new CPropertyAction(this, &PhotonEtcLLTF::onWavelength);
	PE_GetWavelength(this->handle, &wavelength);
	PE_GetWavelengthRange(this->handle, &wavelengthMinimum, &wavelengthMaximum);
	CreateProperty("Wavelength", std::to_string(wavelength).c_str(), MM::Float, false, pAct, false);
	SetPropertyLimits("Wavelength", wavelengthMinimum, wavelengthMaximum);

	return DEVICE_OK;
}

int PhotonEtcLLTF::Shutdown() {
	PE_Close(this->handle);
	PE_Destroy(this->handle);
	return DEVICE_OK;
}

void PhotonEtcLLTF::GetName(char* name) const {
	assert(strlen(PhotonEtcLLTF_NAME) < CDeviceUtils::GetMaxStringLength());
	CDeviceUtils::CopyLimitedString(name, PhotonEtcLLTF_NAME);
}

bool PhotonEtcLLTF::Busy() {
	// based on the documentation all methods should be synchronous,
	// so we should not need to wait unless currently calling another method
	return false;
}

int PhotonEtcLLTF::onConfFile(MM::PropertyBase* pProp, MM::ActionType eAct) {
	return DEVICE_OK;
}

int PhotonEtcLLTF::onWavelength(MM::PropertyBase* pProp, MM::ActionType eAct) {
	if (eAct == MM::BeforeGet) {
		double wavelength;
		int status = PE_GetWavelength(this->handle, &wavelength);
		if (status != PE_SUCCESS) { return status; }
		pProp->Set(wavelength);
	}
	else if (eAct == MM::AfterSet) {
		double wavelength;
		pProp->Get(wavelength);
		int status = PE_SetWavelength(this->handle, wavelength);
		if (status != PE_SUCCESS) { return status; }
	}
	return DEVICE_OK;
}