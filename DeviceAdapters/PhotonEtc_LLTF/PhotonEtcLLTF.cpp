#include "PhotonEtcLLTF.h"

// TODO replace with boost::filesystem, or with std::filesystem (if the project shifts to C++17)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

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
	fs::path defaultFilename;
	CPropertyAction* pActConf = new CPropertyAction(this, &PhotonEtcLLTF::onConfFile);

	std::vector<std::string>confFileSearchPaths = {
		".",
		"C:\\Program Files\\Photon etc\\PHySpecV2\\Devices",
		"C:\\Program Files (x86)\\Photon etc\\PHySpecV2\\Devices"
	};

	std::vector<fs::path>confFilePaths;

	for (const auto& confFilePath : confFileSearchPaths) {
		for (const auto& entry : fs::directory_iterator(confFilePath)) {
			std::string filename = entry.path().filename().string();
			if (fs::is_regular_file(entry) &&
				filename.find("M00001", 0) != std::string::npos &&
				filename.rfind("xml", filename.length() - 3) != std::string::npos) {
				confFilePaths.push_back(entry.path());
			}
		}
	}

	if (confFilePaths.size() > 0) {
		CreateProperty("LLTF XML File", confFilePaths[0].string().c_str(), MM::String, false, pActConf, true);
	}
	else {
		// TODO add popup explaining that there is not a valid configuration file
		CreateProperty("LLTF XML File", "None", MM::String, false, pActConf, true);
	}
	
	// TODO should skip the first element since we already added it
	for (const auto& confFilePath : confFilePaths) {
		AddAllowedValue("LLTF XML File", confFilePath.string().c_str());
	}
}

PhotonEtcLLTF::~PhotonEtcLLTF() {}


int PhotonEtcLLTF::Initialize() {
	PE_STATUS status;
	double wavelength;
	double wavelengthMinimum, wavelengthMaximum;
	char message[1024];
	char confFilename[1024];

	this->GetProperty("LLTF XML File", confFilename);
	std::snprintf(message, 1024, "confFilename=%s", confFilename);
	LogMessage(message);
	if (confFilename[0] == '\0') { return DEVICE_ERR; }

	status = PE_Create(confFilename, &this->handle);
	std::snprintf(message, 1024, "handle=%p, status=%d (%s)", this->handle, status, status_messages[status].c_str());
	LogMessage(message);
	if (status != PE_SUCCESS) { return DEVICE_ERR; }

	status = PE_GetSystemName(this->handle, 0, this->systemName, PhotonEtcLLTF_MAX_NAME_SIZE);
	std::snprintf(message, 1024, "systemName=%s, status=%d (%s)", this->systemName, status, status_messages[status].c_str());
	LogMessage(message);
	CreateProperty("System Name", this->systemName, MM::String, true);
	if (status != PE_SUCCESS) { return DEVICE_ERR; }

	status = PE_Open(this->handle, this->systemName);	
	std::snprintf(message, 1024, "handle=%p, name=%s, status=%d (%s)", 
		this->handle, this->systemName, status, status_messages[status].c_str()
	);
	LogMessage(message);
	if (status != PE_SUCCESS) { return DEVICE_ERR; }

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