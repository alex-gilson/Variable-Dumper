#include "DumperContainer.h"
#include <filesystem>
#include <vector>

namespace VariableDumper
{

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

void DumperContainer::createDumper(const std::string& name, int dumpSize, int countMax, int* buffPptr, int auPMax)
{
	std::lock_guard<std::mutex> createDumperVarLock(writeToDumperMapMutex_);
	std::string& path = getPath();
	std::filesystem::create_directory(path);
	std::string fileName = path + name;
	if (!threadFileNamesMap_[std::this_thread::get_id()].insert(fileName).second)
	{
		throw std::runtime_error("Dumper file has already been initialized and associated to a thread.");
	}
	dumperFileNameMap_[fileName] = std::make_unique<Dumper>(fileName, buffPptr, dumpSize, countMax, auPMax);
}

void DumperContainer::setDumperPrecision(const std::string& name, int precision)
{
	Dumper* dumper = getDumper(name);
	if (dumper)
	{
		dumper->setPrecision(precision);
	}
}
void DumperContainer::setDumpersPrecision(int precision)
{
	for (auto& dumperIt : dumperFileNameMap_)
	{
		dumperIt.second->setPrecision(precision);
	}
}

void DumperContainer::destroyDumpers()
{
	std::lock_guard<std::mutex> destroyDumpersLock(writeToDumperMapMutex_);
	std::vector<std::string> fileNamesToErase;
	std::set<std::string>& fileNameSetFromThread = threadFileNamesMap_[std::this_thread::get_id()];
	for (auto& fileName : fileNameSetFromThread)
	{
		fileNamesToErase.push_back(fileName);
	}
	for (auto& fileNameToErase : fileNamesToErase)
	{
		dumperFileNameMap_.erase(fileNameToErase);
		fileNameSetFromThread.erase(fileNameToErase);
	}
}

Dumper* DumperContainer::getDumper(const std::string& name)
{
	std::set<std::string>& fileNameSetFromThread = threadFileNamesMap_[std::this_thread::get_id()];
	std::string fileName = getPath() + name;
	const auto& fileNameIt = fileNameSetFromThread.find(fileName);
	if (fileNameIt != fileNameSetFromThread.end())
	{
		return dumperFileNameMap_[fileName].get();
	}
	return nullptr;
}

} // namespace VariableDumper
