#include "DumperManager.h"
#include <filesystem>
#include <vector>

namespace VariableDumper
{

DumperManager* DumperManager::instance = 0;
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

Dumper* DumperManager::createDumper(const std::string& name)
{
	std::lock_guard<std::mutex> createDumperVarLock(writeToDumperMapMutex_);
	std::string& path = getPath();
	if (!path.empty() && path != ".")
	{
		std::filesystem::create_directory(path);
	}
	std::string fullName = path + name;
	if (!fileNamesSet_.insert(fullName).second)
	{
		throw std::runtime_error("Dumper file has already been initialized.");
	}
	dumperFileNameMap_[fullName] = std::make_unique<Dumper>(fullName, defaultPrecision_, defaultCsvValueDelimiter_, defaultCsvLineDelimiter_);
	threadFileNamesMap_[std::this_thread::get_id()].insert(fullName);
	return dumperFileNameMap_[fullName].get();
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
	defaultPrecision_ = precision;
	for (auto& fileName : fileNamesSet_)
	{
		dumperFileNameMap_.at(fileName)->setPrecision(precision);
	}
}

void DumperManager::setDumpersCSVDelimiters(char valueDelimiter, char lineDelimiter)
{
	defaultCsvValueDelimiter_ = valueDelimiter;
	defaultCsvLineDelimiter_  = lineDelimiter;
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

void DumperManager::setDumperMaxDumps(const std::string& name, int maxDumps)
{
	Dumper* dumper = getDumper(name);
	if (dumper)
	{
		dumper->setMaxDumps(maxDumps);
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
	std::string fullName = getPath() + name;
	const auto& fileNameIt = fileNameSetFromThread.find(fullName);
	if (fileNameIt != fileNameSetFromThread.end())
	{
		return dumperFileNameMap_[fullName].get();
	}
	return createDumper(name);
}

} // namespace VariableDumper
