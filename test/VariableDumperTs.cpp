
#include "VariableDumper.h"
#include <filesystem>
#include <cassert>
#include <array>
#include <complex>
#include <vector>
#include <cstring>

class VariableDumperTs
{
public:
	virtual ~VariableDumperTs() {};
	virtual void setup()
	{
		SET_DUMPERS_PRECISION(15);
		SET_DUMPERS_CSV_DELIMITERS(',', '\n');
		std::filesystem::remove_all(path_);
		std::filesystem::create_directory(path_);
	};
	virtual void teardown()
	{
		std::filesystem::remove_all(path_);
	}
	virtual void run(int numRuns = 1)
	{
		for (int i = 0; i < numRuns; i++)
		{
			setup();
			runTest();
			teardown();
		}
	}
protected:
	virtual void runTest() = 0;
	template<typename T>
	bool isArrayEqualToCSV(std::vector<std::vector<T>>& array, const std::string& fileName, int precision = 12, char valueDelimiter = ',', char lineDelimiter = '\n')
	{
		std::ifstream file(fileName);
		std::string line, cell;
		size_t index1 = 0;
		while (getline(file, line, lineDelimiter))
		{
			size_t index2 = 0;
			if (index1 >= array.size())
			{
				return false;
			}
			std::stringstream lineStream(line);
			while (getline(lineStream, cell, valueDelimiter))
			{
				if (index2 >= array[index1].size())
				{
					return false;
				}
				if constexpr (std::is_integral<T>::value)
				{
					if (stoi(cell) != array[index1][index2])
					{
						return false;
					}
				}
				else if constexpr (std::is_floating_point<T>::value)
				{
					std::stringstream ss;
					ss.precision(precision);
					ss << array[index1][index2];
					if (cell != ss.str())
					{
						return false;
					}
				}
				else if constexpr(std::is_same_v<T, std::string>)
				{
					if (cell != array[index1][index2])
					{
						return false;
					}
				}
				else if constexpr(std::is_same_v<T, const char*>)
				{
					if (strcmp(cell.c_str(), array[index1][index2]))
					{
						return false;
					}
				}
				index2++;
			}
			if (index2 != array[index1].size())
			{
				return false;
			}
			index1++;
		}
		if (index1 != array.size())
		{
			return false;
		}
		return true;
	}
	const std::string path_ = VARIABLE_DUMPER_TEST_DATA_DIR;
	const long double pi_ = 3.141592653589793238;
};

class CArrayVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "array1.csv";
		std::string filePath2 = path_ + "array2.csv";
		constexpr int size  = 4;

		int array1_1[size] = { 1, 2, 3, 4 };
		int array1_2[size] = { 5, 6, 7, 8 };
		long double array2[size] = { 1*pi_, 2*pi_, 3*pi_, 4*pi_ };

		std::vector<std::vector<int>> testArray1;
		testArray1.push_back(std::vector<int>{&array1_1[0], &array1_1[0] + size });
		testArray1.push_back(std::vector<int>{&array1_2[0], &array1_2[0] + size });

		std::vector<std::vector<long double>> testArray2;
		testArray2.push_back(std::vector<long double>{&array2[0], &array2[0] + size });

		DUMP_VAR("array1", array1_1, size);
		DUMP_VAR("array1", array1_2, size);
		DUMP_VAR("array2", array2,   size);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(testArray1, filePath1));
		assert(isArrayEqualToCSV(testArray2, filePath2, 15));
	}
public:
	virtual ~CArrayVariableDumperTs() {};
};

class MultithreadVariableDumperTs : public VariableDumperTs
{
public:
	void runThreadTask(const std::string path, const std::string dumperName, int array[], int size)
	{
		// Sets the path of the dumpers associated to this thread
		SET_DUMPERS_PATH(path);

		DUMP_VAR(dumperName, array, size);

		// Destroys the dumpers associated to this thread
		DESTROY_DUMPERS();
	}
public:
	virtual ~MultithreadVariableDumperTs() {};
};

class MultithreadSamePathVariableDumperTs : public MultithreadVariableDumperTs
{
protected:
	void runTest() override
	{
		std::string dumperName1 = "array1";
		std::string dumperName2 = "array2";

		constexpr int size = 4;
		int array1[size] = { 1, 2, 3, 4 };
		int array2[size] = { 1, 2, 3, 4 };

		std::vector<std::vector<int>> testArray1, testArray2;
		testArray1.push_back(std::vector<int>{&array1[0], &array1[0] + size });
		testArray2.push_back(std::vector<int>{&array2[0], &array2[0] + size });

		std::thread t1(&MultithreadSamePathVariableDumperTs::runThreadTask, this, path_, dumperName1, array1, size);
		std::thread t2(&MultithreadSamePathVariableDumperTs::runThreadTask, this, path_, dumperName2, array2, size);
		t1.join();
		t2.join();

		assert(isArrayEqualToCSV(testArray1, path_ + dumperName1 + ".csv"));
		assert(isArrayEqualToCSV(testArray2, path_ + dumperName2 + ".csv"));
	}
public:
	virtual ~MultithreadSamePathVariableDumperTs() {};
};

class MultithreadDifferentPathVariableDumperTs : public MultithreadVariableDumperTs
{
protected:
	void runTest() override
	{
		std::string dumperName1 = "array1";
		std::string dumperName2 = "array2";
		std::string path1 = path_ + "thread_1/";
		std::string path2 = path_ + "thread_2/";

		constexpr int size = 4;
		int array1[size] = { 1, 2, 3, 4 };
		int array2[size] = { 1, 2, 3, 4 };

		std::vector<std::vector<int>> testArray1, testArray2;
		testArray1.push_back(std::vector<int>{&array1[0], &array1[0] + size });
		testArray2.push_back(std::vector<int>{&array2[0], &array2[0] + size });

		std::thread t1(&MultithreadDifferentPathVariableDumperTs::runThreadTask, this, path1, dumperName1, array1, size);
		std::thread t2(&MultithreadDifferentPathVariableDumperTs::runThreadTask, this, path2, dumperName2, array2, size);
		t1.join();
		t2.join();

		assert(isArrayEqualToCSV(testArray1, path1 + dumperName1 + ".csv"));
		assert(isArrayEqualToCSV(testArray2, path2 + dumperName2 + ".csv"));
	}
public:
	virtual ~MultithreadDifferentPathVariableDumperTs() {};
};

class BasicContainerVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec.csv";
		std::string filePath2 = path_ + "array.csv";

		constexpr int size = 4;
		std::vector<std::vector<int>> vec = { { 1, 2, 3, 4}, {5, 6, 7, 8 } };
		std::array<int, size> array = { 1, 2, 3, 4 };
		std::vector<std::vector<int>> testArray;
		testArray.push_back(std::vector<int>(array.begin(), array.end()));

		DUMP_VAR("vec", vec[0]);
		DUMP_VAR("vec", vec[1]);
		DUMP_VAR("array", array);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec, filePath1));
		assert(isArrayEqualToCSV(testArray, filePath2));
	}
public:
	virtual ~BasicContainerVariableDumperTs() {};
};

class NoCopyCustomContainerVariableDumperTs : public VariableDumperTs
{
	// Custom container to test that no copies are made when dumping
	class testingContainer
	{
		std::vector<int> data_;
	public:
		explicit testingContainer(const std::vector<int>& vec)
		{
			data_ = vec;
		};
		testingContainer(const testingContainer& other)
		{
			// This container should not be copied
			assert(false);
			data_ = other.data_;
		}
		size_t size()
		{
			return data_.size();
		}
		int& operator[](const int idx)
		{
			return data_[idx];
		}
		int* data()
		{
			return data_.data();
		}
		class iterator
		{
		private:
			std::vector<int>::iterator ptr;

		public:
			iterator(std::vector<int>::iterator p) : ptr(p) {}
			iterator operator++() { ptr++; return *this; }
			bool operator!=(const iterator& other) const { return ptr != other.ptr; }
			int& operator*() { return *ptr; }
		};
		iterator begin() { return iterator(data_.begin()); }
		iterator end() { return iterator(data_.end()); }
	};
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "cont.csv";

		std::vector<std::vector<int>>  vec1 = { { 1, 2, 3, 4 } };
		testingContainer cont(vec1[0]);

		DUMP_VAR("cont", cont);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec1, filePath1));
	}
public:
	virtual ~NoCopyCustomContainerVariableDumperTs() {};
};

class ContainerDifferentSizeVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec.csv";

		std::vector<std::vector<int>> vec = { { 1, 2, 3, 4}, {5, 6, 7, 8, 9, 10, 11, 12 } };

		DUMP_VAR("vec", vec[0]);
		DUMP_VAR("vec", vec[1]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec, filePath1));
	}
public:
	virtual ~ContainerDifferentSizeVariableDumperTs() {};
};

class StringsContainerVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "strings.csv";

		std::vector<std::vector<std::string>> strings = { { "This ", "is", " a", " test."}, {"This ", "is", " also ", "a ", "test ", "of ", "different", "length", "."}};

		DUMP_VAR("strings", strings[0]);
		DUMP_VAR("strings", strings[1]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(strings, filePath1));
	}
public:
	virtual ~StringsContainerVariableDumperTs() {};
};

class CStringsContainerVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "strings.csv";

		std::vector<std::vector<const char*>> strings = { { "This ", "is", " a", " test."}, {"This ", "is", " also ", "a ", "test ", "of ", "different", "length", "."}};

		DUMP_VAR("strings", strings[0]);
		DUMP_VAR("strings", strings[1]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(strings, filePath1));
	}
public:
	virtual ~CStringsContainerVariableDumperTs() {};
};

class SetPrecisionVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec1.csv";

		std::vector<std::vector<long double>> vec1 = { { 1 * pi_, 2 * pi_, 3 * pi_, 4 * pi_ } };

		SET_DUMPER_PRECISION("vec1", 12);
		DUMP_VAR("vec1", vec1[0]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec1, filePath1, 12));

		SET_DUMPERS_PRECISION(13);
		DUMP_VAR("vec1", vec1[0]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec1, filePath1, 13));
	}
public:
	virtual ~SetPrecisionVariableDumperTs() {};
};

class MaxCountVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec.csv";

		std::vector<std::vector<int>> vec = { { 1, 2, 3, 4}, {5, 6, 7, 8 } };
		std::vector<std::vector<int>> testVec = { { 1, 2, 3, 4} };

		SET_DUMPER_MAX_DUMPS("vec", 1);
		DUMP_VAR("vec", vec[0]);
		DUMP_VAR("vec", vec[1]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(testVec, filePath1));
	}
public:
	virtual ~MaxCountVariableDumperTs() {};
};

class DumpSizeVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec.csv";

		std::vector<std::vector<int>> vec = { { 1, 2, 3, 4}, {5, 6, 7, 8 } };
		std::vector<std::vector<int>> testVec = { { 1, 2, 3}, {5, 6, 7, 8 } };

		DUMP_VAR("vec", vec[0], 3);
		DUMP_VAR("vec", vec[1], 5);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(testVec, filePath1));
	}
public:
	virtual ~DumpSizeVariableDumperTs() {};
};

class ComplexContainerVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec.csv";

		std::vector<std::vector<std::complex<double>>> vec = { { {1., 2.}, {3., 4.} } };
		std::vector<std::vector<double>> testVec = { {1., 2., 3., 4.} };

		DUMP_VAR("vec", vec[0]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(testVec, filePath1, 15));
	}
public:
	virtual ~ComplexContainerVariableDumperTs() {};
};

class SetDelimitersVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec.csv";

		std::vector<std::vector<int>> vec = { { 1, 2, 3, 4}, {5, 6, 7, 8 } };

		SET_DUMPERS_CSV_DELIMITERS(',', ';');
		DUMP_VAR("vec", vec[0]);
		DUMP_VAR("vec", vec[1]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec, filePath1, 12, ',', ';'));

		SET_DUMPER_CSV_DELIMITERS("vec", ';', '\n');
		DUMP_VAR("vec", vec[0]);
		DUMP_VAR("vec", vec[1]);
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec, filePath1, 12, ';', '\n'));
	}
public:
	virtual ~SetDelimitersVariableDumperTs() {};
};

class MultidimensionalContainerVariableDumperTs : public VariableDumperTs
{
	void runTest() override
	{
		SET_DUMPERS_PATH(path_);

		std::string filePath1 = path_ + "vec.csv";
		std::string filePath2 = path_ + "vecStrings.csv";

		std::vector<std::vector<int>> vec = { { 1, 2, 3, 4}, {5, 6, 7, 8 } };
		std::vector<std::string> thisIsATest = {"This ", "is", " a", " test."};
		std::vector<std::string> thisIsAlsoATestOfDifferentLength = {"This ", "is", " also ", "a ", "test ", "of ", "different", "length", "."};
		std::vector<std::vector<std::string>> strings = { thisIsATest, thisIsAlsoATestOfDifferentLength };
		std::vector<std::vector<std::vector<std::string>>> vecStrings;
		vecStrings.push_back(strings);
		vecStrings.push_back(strings);

		// Flatten vecStrings
		std::vector<std::vector<std::string>> testVecStrings = { thisIsATest, thisIsAlsoATestOfDifferentLength, thisIsATest, thisIsAlsoATestOfDifferentLength };

		SET_DUMPER_MAX_DUMPS("vecStrings", 1);

		DUMP_VAR("vec", vec);
		DUMP_VAR("vecStrings", vecStrings);
		DUMP_VAR("vecStrings", vecStrings); // Ignored because max dumps is set to 1
		DESTROY_DUMPERS();

		assert(isArrayEqualToCSV(vec, filePath1));
		assert(isArrayEqualToCSV(testVecStrings, filePath2));
	}
public:
	virtual ~MultidimensionalContainerVariableDumperTs() {};
};

int main()
{
	CArrayVariableDumperTs{}.run();
	MultithreadSamePathVariableDumperTs{}.run(10);
	MultithreadDifferentPathVariableDumperTs{}.run(10);
	BasicContainerVariableDumperTs{}.run();
	NoCopyCustomContainerVariableDumperTs{}.run();
	ContainerDifferentSizeVariableDumperTs{}.run();
	StringsContainerVariableDumperTs{}.run();
	CStringsContainerVariableDumperTs{}.run();
	ComplexContainerVariableDumperTs{}.run();
	SetPrecisionVariableDumperTs{}.run();
	MaxCountVariableDumperTs{}.run();
	DumpSizeVariableDumperTs{}.run();
	SetDelimitersVariableDumperTs{}.run();
	MultidimensionalContainerVariableDumperTs{}.run();
	return false;
}