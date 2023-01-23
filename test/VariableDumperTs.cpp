
#include <filesystem>
#include <cassert>
#include "DumperContainer.h"

using namespace VariableDumper;
DumperContainer* DumperContainer::instance = 0;

class VariableDumperTs
{
public:
	virtual ~VariableDumperTs() {};
	virtual void setup()
	{
		std::filesystem::remove_all(path_);
		std::filesystem::create_directory(path_);
	};
	virtual void teardown()
	{
		std::filesystem::remove_all(path_);
	}
	virtual void run()
	{
		setup();
		runTest();
		teardown();
	}
protected:
	virtual void runTest() = 0;
	template<typename T>
	bool isArrayEqualToCSV(T* array, int size, const std::string& fileName, int precision = 12)
	{
		std::ifstream file(fileName);
		std::string line, cell;
		int index = 0;
		char delim1 = '\n';
		char delim2 = ';';
		while (getline(file, line, delim1))
		{
			if (index >= size)
			{
				return false;
			}
			std::stringstream lineStream(line);
			while (getline(lineStream, cell, delim2))
			{
				if (std::is_integral<T>::value)
				{
					if (stoi(cell) != array[index])
					{
						return false;
					}
				}
				else if (std::is_floating_point<T>::value)
				{
					std::stringstream ss;
					ss.precision(precision);
					ss << array[index];
					if (cell != ss.str())
					{
						return false;
					}
				}
				index++;
			}
		}
		if (index != size)
		{
			return false;
		}
		return true;
	}
	const std::string path_ = VARIABLE_DUMPER_TEST_DATA_DIR;
	const long double pi_ = 3.141592653589793238;
};

// Test one thread, one array of ints, one array of doubles
class OneThreadArrayDumpVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		UPDATE_DUMPER_CONTAINER_PATH(path_);

		std::string filePath1 = path_ + "array1.csv";
		std::string filePath2 = path_ + "array2.csv";
		constexpr int arraySize  = 4;
		int array1_1[arraySize] = { 1, 2, 3, 4 };
		int array1_2[arraySize] = { 5, 6, 7, 8 };
		int array1[arraySize*2] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		long double array2[arraySize] = { 1*pi_, 2*pi_, 3*pi_, 4*pi_ };

		INIT_DUMPER("array1.csv", arraySize);
		INIT_DUMPER("array2.csv", arraySize);
		SET_DUMPER_PRECISION("array2.csv", 12);
		DUMP_ARRAY(array1_1, "array1.csv");
		DUMP_ARRAY(array1_2, "array1.csv");
		DUMP_ARRAY(array2, "array2.csv");
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(array1, arraySize*2, filePath1));
		assert(isArrayEqualToCSV(array2, arraySize, filePath2, 12));
	}
public:
	virtual ~OneThreadArrayDumpVariableDumperTs() {};
};

// Test two threads
class MultithreadDumpVariableDumperTs : public VariableDumperTs
{
	void runThreadTask(const std::string path, const std::string fileName, int array[], int arraySize)
	{
		UPDATE_DUMPER_CONTAINER_PATH(path);

		INIT_DUMPER(fileName, arraySize);
		DUMP_ARRAY(array, fileName);

		// Destroys only the dumpers of the path associated with this thread
		//DESTROY_DUMPERS();

	}

	void runTest() override
	{
		// Test two threads with the same path
		{
			std::string fileName1 = "array1.csv";
			std::string fileName2 = "array2.csv";

			constexpr int arraySize = 4;
			int array1[arraySize] = { 1, 2, 3, 4 };
			int array2[arraySize] = { 1, 2, 3, 4 };

			std::thread t1(&MultithreadDumpVariableDumperTs::runThreadTask, this, path_, fileName1, array1, arraySize);
			std::thread t2(&MultithreadDumpVariableDumperTs::runThreadTask, this, path_, fileName2, array2, arraySize);
			t1.join();
			t2.join();

			DESTROY_DUMPERS();

			assert(isArrayEqualToCSV(array1, arraySize, path_ + fileName1));
			assert(isArrayEqualToCSV(array2, arraySize, path_ + fileName2));
		}
	}

public:
	virtual ~MultithreadDumpVariableDumperTs() {};
};

int main()
{
	CREATE_DUMPER_C0NTAINER(VARIABLE_DUMPER_TEST_DATA_DIR);
	OneThreadArrayDumpVariableDumperTs{}.run();
	MultithreadDumpVariableDumperTs{}.run();
	return false;
}