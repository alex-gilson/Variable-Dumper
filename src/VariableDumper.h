#pragma once
#include "DumperManager.h"

#ifndef VARIABLE_DUMPER_DISABLED
#define SET_DUMPERS_PATH(a)              VariableDumper::DumperManager::getDumperManager()->updatePath(a)
#define DESTROY_DUMPERS()                VariableDumper::DumperManager::getDumperManager()->destroyDumpers()
#define DUMP_VAR(...)                    VariableDumper::DumperManager::getDumperManager()->dump(__VA_ARGS__)
#define SET_DUMPER_PRECISION(a,b)        VariableDumper::DumperManager::getDumperManager()->setDumperPrecision(a,b)
#define SET_DUMPERS_PRECISION(a)         VariableDumper::DumperManager::getDumperManager()->setDumpersPrecision(a)
#define SET_DUMPER_CSV_DELIMITERS(a,b,c) VariableDumper::DumperManager::getDumperManager()->setDumperCSVDelimiters(a,b,c)
#define SET_DUMPERS_CSV_DELIMITERS(a,b)  VariableDumper::DumperManager::getDumperManager()->setDumpersCSVDelimiters(a,b)
#define SET_DUMPER_MAX_DUMPS(a,b)        VariableDumper::DumperManager::getDumperManager()->setDumperMaxDumps(a,b)
#else
#define SET_DUMPERS_PATH(a)
#define DESTROY_DUMPERS()
#define DUMP_VAR(...)
#define SET_DUMPER_PRECISION(a,b)
#define SET_DUMPERS_PRECISION(a)
#define SET_DUMPER_CSV_DELIMITERS(a,b,c)
#define SET_DUMPERS_CSV_DELIMITERS(a,b)
#endif
