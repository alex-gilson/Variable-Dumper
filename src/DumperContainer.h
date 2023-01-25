#pragma once
#include "Dumper.h"
#include <memory>
#include <thread>
#include <mutex>
#include <set>

#ifndef VARIABLE_DUMPER_DISABLED
#define INIT_VARIABLE_DUMPER(a)         DumperContainer::getDumperContainer(a)
#define SET_DUMPERS_PATH(a)             DumperContainer::getDumperContainer()->updatePath(a)
#define DESTROY_DUMPERS()               DumperContainer::getDumperContainer()->destroyDumpers()
#define INIT_DUMPER(fileName, ...)      DumperContainer::getDumperContainer()->createDumper(fileName, __VA_ARGS__)
#define DUMP_VAR(a,b)                   DumperContainer::getDumperContainer()->dump(a,b)
#define SET_DUMPER_PRECISION(a,b)       DumperContainer::getDumperContainer()->setDumperPrecision(a,b)
#define SET_DUMPERS_PRECISION(a)        DumperContainer::getDumperContainer()->setDumpersPrecision(a)
#else
#define INIT_VARIABLE_DUMPER(a)
#define SET_DUMPERS_PATH(a)
#define DESTROY_DUMPERS()
#define INIT_DUMPER(a,b,c,d,e,f)
#define DUMP_VAR(a,b)
#define SET_DUMPER_PRECISION(a,b)
#define SET_DUMPERS_PRECISION(a)
#endif

namespace VariableDumper
{

class DumperContainer
{
private:
    // Constructor that takes in the path of the Dumper object for non multithreaded runtimes
    DumperContainer(std::string path);
    // Pointer to the singleton instance of DumperContainer
    static DumperContainer* instance;
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
    std::mutex createDumperContainerMutex_;
    std::mutex writeToDumperMapMutex_;
    std::mutex updatePathMutex_;

public:
    // Returns a pointer to the singleton instance of DumperContainer
    static DumperContainer* getDumperContainer(const std::string& path = "");
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
