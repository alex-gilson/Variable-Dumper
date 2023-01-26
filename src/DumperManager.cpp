#include "DumperManager.h"
#include <filesystem>
#include <vector>

namespace VariableDumper
{

std::mutex DumperManagerMutex;

DumperManager::DumperManager(std::string path)
{
	updatePath(path);
}

std::string& DumperManager::getPath()
{
	return pathMap_[std::this_thread::get_id()];
}

DumperManager* DumperManager::getDumperManager(const std::string& path)
{
	std::lock_guard<std::mutex> dumperLock(DumperManagerMutex);
	if (!instance) {
		instance = new DumperManager{ path };
	}
	return instance;
}

void DumperManager::updatePath(const std::string& path)
{
	std::lock_guard<std::mutex> updatePathLock(updatePathMutex_);
	pathMap_[std::this_thread::get_id()] = path;
}

void DumperManager::createDumper(const std::string& name, int dumpSize, int countMax)
{
	std::lock_guard<std::mutex> createDumperVarLock(writeToDumperMapMutex_);
	std::string& path = getPath();
	std::filesystem::create_directory(path);
	std::string fileName = path + name;
	if (!fileNamesSet_.insert(fileName).second)
	{
		throw std::runtime_error("Dumper file has already been initialized.");
	}
	dumperFileNameMap_[fileName] = std::make_unique<Dumper>(fileName, dumpSize, countMax);
	threadFileNamesMap_[std::this_thread::get_id()].insert(fileName);
}

void DumperManager::setDumperPrecision(const std::string& name, int precision)
{
	Dumper* dumper = getDumper(name);
	if (dumper)
	{
		dumper->setPrecision(precision);
	}
}

void DumperManager::setDumpersPrecision(int precision)
{
	for (auto& fileName : fileNamesSet_)
	{
		dumperFileNameMap_.at(fileName)->setPrecision(precision);
	}
}

void DumperManager::setDumpersCSVDelimiters(char valueDelimiter, char lineDelimiter)
{
	for (auto& fileName : fileNamesSet_)
	{
		dumperFileNameMap_.at(fileName)->setCSVDelimiters(valueDelimiter, lineDelimiter);
	}
}

void DumperManager::setDumperCSVDelimiters(const std::string& name, char valueDelimiter, char lineDelimiter)
{
	Dumper* dumper = getDumper(name);
	if (dumper)
	{
		dumper->setCSVDelimiters(valueDelimiter, lineDelimiter);
	}
}

void DumperManager::destroyDumpers()
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
		fileNamesSet_.erase(fileNameToErase);
	}
}

Dumper* DumperManager::getDumper(const std::string& name)
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
