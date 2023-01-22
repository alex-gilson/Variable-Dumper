#pragma once
#include "Dumper.h"
#include <memory>
#include <thread>
#include <mutex>

#ifndef VARIABLE_DUMPER_DISABLED
#define CREATE_DUMPER_C0NTAINER(a)      DumperContainer::getDumperContainer(a)
#define UPDATE_DUMPER_CONTAINER_PATH(a) DumperContainer::getDumperContainer()->updatePath(a)
#define DESTROY_DUMPERS()               DumperContainer::getDumperContainer()->destroyDumpers()
#define INIT_DUMPER(a,b,c,d,e,f)        DumperContainer::getDumperContainer()->createDumper(a,b,c,d,e,f)
#define DUMP_ARRAY(a,b)                 DumperContainer::getDumperContainer()->dump(a,b)
#else
#define CREATE_DUMPER_C0NTAINER(a)
#define UPDATE_DUMPER_CONTAINER_PATH(a)
#define DESTROY_DUMPERS()
#define INIT_DUMPER(a,b,c,d,e,f)
#define DUMP_ARRAY(a,b)
#endif

namespace VariableDumper
{

class DumperContainer
{
private:
    DumperContainer(std::string path);
    static DumperContainer* instance;
    std::unordered_map<std::string, std::unique_ptr<Dumper> > dumperMap_;
    std::string& getPath();

    std::mutex createDumperContainerMutex_;
    std::mutex writeToDumperMapMutex_;
    std::mutex updatePathMutex_;

public:
    std::unordered_map<std::thread::id, std::string> pathMap_;
    static DumperContainer* getDumperContainer(const std::string& path = "");
    void updatePath(const std::string& path);
    void createDumper(const std::string& name, int& audio_ptr, int bufferSize,
        int dumpSize, int countMax, int auPMax);
    Dumper* getDumper(const std::string& name);
    void destroyDumpers();
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