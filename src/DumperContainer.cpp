#include "DumperContainer.h"
#ifdef WIN32
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

std::mutex dumperContainerMutex;

DumperContainer::DumperContainer(std::string path)
{
	updatePath(path);
}

std::string& DumperContainer::getPath()
{
	return pathMap_[std::this_thread::get_id()];
}

DumperContainer* DumperContainer::getDumperContainer(const std::string& path)
{
	std::lock_guard<std::mutex> dumperLock(dumperContainerMutex);
	if (!instance) {
		instance = new DumperContainer{ path };
	}
	return instance;
}

void DumperContainer::updatePath(const std::string& path)
{
	std::lock_guard<std::mutex> updatePathLock(updatePathMutex_);
	pathMap_[std::this_thread::get_id()] = path;
}

void DumperContainer::createDumper(const std::string& name, int& audio_ptr,
	int bufferSize, int dumpSize, int countMax, int auPMax)
{
	std::lock_guard<std::mutex> createDumperVarLock(writeToDumperMapMutex_);
	std::string& folderDir = getPath();
	std::string folderDir2 = folderDir;
#ifdef WIN32
	std::filesystem::create_directory(folderDir2);
#else
	std::experimental::filesystem::create_directory(folderDir2);
#endif
	std::string fileName = folderDir + name;
	dumperMap_[fileName] = std::make_unique<Dumper>(fileName, &audio_ptr, bufferSize, dumpSize, countMax, auPMax);
}

void DumperContainer::destroyDumpers()
{
	std::lock_guard<std::mutex> destroyDumpersLock(writeToDumperMapMutex_);
	std::vector<std::string> mapKeys;
	for (auto& dumperIt : dumperMap_)
	{
		// Check that dumper belongs to current process.
		if (dumperIt.first.find(getPath()) != std::string::npos)
		{
			mapKeys.push_back(dumperIt.first);
		}
	}
	for (auto& mapKey : mapKeys)
	{
		dumperMap_.erase(mapKey);
	}
}

Dumper* DumperContainer::getDumper(const std::string& name)
{
	auto dumperIt = dumperMap_.find(getPath() + name);
	if (dumperIt != dumperMap_.end())
	{
		return dumperIt->second.get();
	}
	return nullptr;
}

