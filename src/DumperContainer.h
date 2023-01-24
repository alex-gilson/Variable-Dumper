#pragma once
#include "Dumper.h"
#include <memory>
#include <thread>
#include <mutex>
#include <set>

#ifndef VARIABLE_DUMPER_DISABLED
#define CREATE_DUMPER_C0NTAINER(a)      DumperContainer::getDumperContainer(a)
#define UPDATE_DUMPER_CONTAINER_PATH(a) DumperContainer::getDumperContainer()->updatePath(a)
#define DESTROY_DUMPERS()               DumperContainer::getDumperContainer()->destroyDumpers()
#define INIT_DUMPER(fileName, ...)      DumperContainer::getDumperContainer()->createDumper(fileName, __VA_ARGS__)
#define DUMP_ARRAY(a,b)                 DumperContainer::getDumperContainer()->dump(a,b)
#define SET_DUMPER_PRECISION(a,b)       DumperContainer::getDumperContainer()->setDumperPrecision(a,b)
#define SET_DUMPERS_PRECISION(a)        DumperContainer::getDumperContainer()->setDumpersPrecision(a)
#else
#define CREATE_DUMPER_C0NTAINER(a)
#define UPDATE_DUMPER_CONTAINER_PATH(a)
#define DESTROY_DUMPERS()
#define INIT_DUMPER(a,b,c,d,e,f)
#define DUMP_ARRAY(a,b)
#define SET_DUMPER_PRECISION(a,b)
#define SET_DUMPERS_PRECISION(a)
#endif

namespace VariableDumper
{

class DumperContainer
{
private:
    DumperContainer(std::string path);
    static DumperContainer* instance;
    std::unordered_map<std::string,     std::unique_ptr<Dumper>> dumperFileNameMap_;
    std::unordered_map<std::thread::id, std::string> pathMap_;
    std::unordered_map<std::thread::id, std::set<std::string>> threadFileNamesMap_;
    std::string& getPath();

    std::mutex createDumperContainerMutex_;
    std::mutex writeToDumperMapMutex_;
    std::mutex updatePathMutex_;

public:
    static DumperContainer* getDumperContainer(const std::string& path = "");
    void updatePath(const std::string& path);
    void createDumper(const std::string& name, int dumpSize, int countMax = -1, int* buffPptr = nullptr, int auPMax = -1);
    Dumper* getDumper(const std::string& name);
    void destroyDumpers();
    void setDumperPrecision(const std::string& name, int precision);
    void setDumpersPrecision(int precision);
    template<typename T>
    void dump(T buf, const std::string& name)
    {
        Dumper* dumper = getDumper(name);
        if (dumper)
        {
            dumper->dump(buf);
        }
    }
};

} // namespace VariableDumper
