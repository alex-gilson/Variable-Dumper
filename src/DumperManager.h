#pragma once
#include "Dumper.h"
#include <memory>
#include <thread>
#include <mutex>
#include <set>

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
    // Default precision for doubles in Dumpers
    int defaultPrecision_ = 15;
    char defaultCsvValueDelimiter_ = ',';
    char defaultCsvLineDelimiter_  = '\n';

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
    Dumper* createDumper(const std::string& name);
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
    // Sets the maximum number of dumps to run on a specific Dumper
    void setDumperMaxDumps(const std::string& name, int maxDumps);
    // Dumps the data buffer using the Dumper object with the specified name
    template<typename T>
    void dump(const std::string& name, T& buf, std::optional<size_t> size = std::nullopt)
    {
        Dumper* dumper = getDumper(name);
        if (dumper)
        {
            dumper->dump(buf, size);
        }
    }
};

} // namespace VariableDumper
