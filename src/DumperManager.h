#pragma once
#include "Dumper.h"
#include <memory>
#include <thread>
#include <mutex>
#include <set>

#ifndef VARIABLE_DUMPER_DISABLED
//#define INIT_VARIABLE_DUMPER(a)          DumperManager::getDumperManager(a)
#define INIT_VARIABLE_DUMPER(...)        DumperManager::getDumperManager(__VA_ARGS__)
#define SET_DUMPERS_PATH(a)              DumperManager::getDumperManager()->updatePath(a)
#define DESTROY_DUMPERS()                DumperManager::getDumperManager()->destroyDumpers()
#define INIT_DUMPER(fileName, ...)       DumperManager::getDumperManager()->createDumper(fileName, __VA_ARGS__)
#define DUMP_VAR(a,b)                    DumperManager::getDumperManager()->dump(a,b)
#define SET_DUMPER_PRECISION(a,b)        DumperManager::getDumperManager()->setDumperPrecision(a,b)
#define SET_DUMPERS_PRECISION(a)         DumperManager::getDumperManager()->setDumpersPrecision(a)
#define SET_DUMPER_CSV_DELIMITERS(a,b,c) DumperManager::getDumperManager()->setDumperCSVDelimiters(a,b,c)
#define SET_DUMPERS_CSV_DELIMITERS(a,b)  DumperManager::getDumperManager()->setDumpersCSVDelimiters(a,b)
#else
#define INIT_VARIABLE_DUMPER(a)
#define SET_DUMPERS_PATH(a)
#define DESTROY_DUMPERS()
#define INIT_DUMPER(a,b,c,d,e,f)
#define DUMP_VAR(a,b)
#define SET_DUMPER_PRECISION(a,b)
#define SET_DUMPERS_PRECISION(a)
#define SET_DUMPER_CSV_DELIMITERS(a,b,c)
#define SET_DUMPERS_CSV_DELIMITERS(a,b)
#endif

namespace VariableDumper
{

class DumperManager
{
private:
    // Constructor that takes in the path of the Dumper object for non multithreaded runtimes
    DumperManager(std::string path);
    // Pointer to the singleton instance of DumperManager
    static DumperManager* instance;
    // Maps the file name of a Dumper object to the corresponding Dumper
    std::unordered_map<std::string, std::unique_ptr<Dumper>> dumperFileNameMap_;
    // Maps the thread ID of a thread to the path of all Dumpers assigned to that thread
    std::unordered_map<std::thread::id, std::string> pathMap_;
    // Maps the thread ID of a thread to a set of file names of the Dumper objects created by that thread
    std::unordered_map<std::thread::id, std::set<std::string>> threadFileNamesMap_;
    // Set of file names of all the Dumper objects created by all threads
    std::set<std::string> fileNamesSet_;
    // Returns the path of the Dumper object associated with the current thread
    std::string& getPath();

    // Mutex objects for synchronizing access to the container
    std::mutex createDumperManagerMutex_;
    std::mutex writeToDumperMapMutex_;
    std::mutex updatePathMutex_;

public:
    // Returns a pointer to the singleton instance of DumperManager
    static DumperManager* getDumperManager(const std::string& path = "");
    // Updates the path of the Dumper object created by the current thread
    void updatePath(const std::string& path);
    // Creates a new Dumper object with the specified name, dump size and count max
    void createDumper(const std::string& name, int dumpSize = -1, int countMax = -1);
    // Returns a pointer to the Dumper object with the specified name
    Dumper* getDumper(const std::string& name);
    // Destroys all the Dumper objects associated with the current thread
    void destroyDumpers();
    // Sets the precision of the Dumper object with the specified name and associated with the current thread
    void setDumperPrecision(const std::string& name, int precision);
    // Sets the precision of all the Dumper objects created
    void setDumpersPrecision(int precision);
    // Sets the csv delimiter characters for all the dumpers
    void setDumpersCSVDelimiters(char valueDelimiter, char lineDelimiter);
    // Sets the csv delimiter characters for the Dumper object with the specified name and associated with the current thread
    void setDumperCSVDelimiters(const std::string& name, char valueDelimiter, char lineDelimiter);
    // Dumps the data buffer using the Dumper object with the specified name
    template<typename T>
    void dump(T& buf, const std::string& name)
    {
        Dumper* dumper = getDumper(name);
        if (dumper)
        {
            dumper->dump(buf);
        }
    }
};

} // namespace VariableDumper
