#include "PluginManager.hpp"
#include <fstream>
#include "../../Utils/JSON/json.hpp"
#include "../../Utils/Logging/Logging.hpp"

bool PM::ReadManifest(const wchar_t* FilePath, JSONData& Data)
{
	std::ifstream ManifestFile(FilePath);

	// Error opening file?
	if (ManifestFile.bad())
		return false;

	// Read the manifest
	nlohmann::json Manifest;
	ManifestFile >> Manifest;

	try
	{
		Data.ModID = Manifest["modid"];
		Data.Name = Manifest["name"];
		Data.Version = Manifest["version"];
		Data.Description = Manifest["desc"];
		Data.NeedsPreload = Manifest["forcepreload"];
	}
	catch (nlohmann::json::exception& e)
	{
		Utils::Logging::Error(__FILE__, __LINE__, "Error parsing plugin manifest: %s", e.what());
		return false;
	}

	return true;
}

bool PM::WriteManifest(const wchar_t* FilePath, JSONData& Data)
{
	std::ofstream ManifestFile("file.json");

	// Error opening file?
	if (ManifestFile.bad())
		return false;

	// Read the manifest
	nlohmann::json Manifest;

	try
	{
		Manifest["modid"] = Data.ModID;
		Manifest["name"] = Data.Name;
		Manifest["desc"] = Data.Description;
		Manifest["version"] = Data.Version;
		Manifest["forcepreload"] = Data.NeedsPreload;
	}
	catch (nlohmann::json::exception& e)
	{
		Utils::Logging::Error(__FILE__, __LINE__, "Error writing json object: %s", e.what());
		return false;
	}

	ManifestFile << std::setw(4) << Manifest;

	return true;
}
