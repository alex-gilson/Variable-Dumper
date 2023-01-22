
#include <filesystem>
#include <cassert>
#include "DumperContainer.h"

using namespace VariableDumper;

class VariableDumperTs
{
public:
	virtual ~VariableDumperTs() {};
	virtual void setup()
	{
		std::filesystem::create_directory(path_);
	};
	virtual void teardown()
	{
		std::filesystem::remove(path_);
	}
	virtual void run()
	{
		setup();
		assert(runTest());
		teardown();
	}
private:
	virtual bool runTest() = 0;
	const std::string path_ = VARIABLE_DUMPER_TEST_DATA_DIR;
};

class BasicOneThreadDumpVariableDumperTs : public VariableDumperTs
{
	bool runTest() override
	{
		return true;
	}
public:
	virtual ~BasicOneThreadDumpVariableDumperTs() {};
};

int main()
{
	BasicOneThreadDumpVariableDumperTs{}.run();
	return false;
}