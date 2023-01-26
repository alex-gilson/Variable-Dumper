![Variable Dumper logo](https://github.com/alex-gilson/Variable-Dumper/blob/main/variable_dumper_logo.png)

# Description

Variable Dumper is a cross-platform C++ library to write to a csv file data contained in arrays or std containers. It has multithreaded support, allowing the developer to write the same variable to different files when an algorithm is being run by two or more threads. Variable-Dumper works with arrays and containers of integer, float, std::string, C string, and std::complex types.

# Installation

```bash
git clone https://github.com/alex-gilson/Variable-Dumper.git
```

Add the following line to you CMakeLists.txt:

```cmake
add_subdirectory(Variable-Dumper)
```

# Usage

Simply add the following lines to your code:

```C++
#include "DumperManager.h"
using namespace VariableDumper;
```

To initialize the DumperManager add the command below with the desired output path. It only has to be run once by any thread. You will be able to update this path for each thread individually but, for now, give it a default. If no argument is given, the current path will be used.

```C++
INIT_VARIABLE_DUMPER(OUTPUT_PATH);
```

For each variable you wish to dump, an associated dumper must be initialized.

```C++
INIT_DUMPER("myVector");
```

To then write to a file use

```C++
std::vector<int> myVector = {1, 2, 3, 4};
DUMP_VAR(myVector, "myVector");
```

For the case where you are dealing with a c style array or when you want to reduce the number of values being printed of a container, you can set up a size.

```C++
int array[] = {1,2,3};
INIT_DUMPER("myArray", 3);
DUMP_VAR(array, "myArray");
```

It is also possible to limit the number of times a variable is printed to file. Set the size to -1 so that it uses the size of the container and then give a value the limit of prints. With the following example, only the first row of myLongVector will be written to file.

```C++
std::vector<int> myLongVector = { {1, 2, 3, 4}, {5, 6, 7} };
INIT_DUMPER("myLongVector", -1, 1);
DUMP_VAR("myLongVector", myLongVector);
```

When you are finished printing. Destroy the dumpers by calling:

```C++
DESTROY_DUMPERS();
```

The resulting files will be in the specified path and will be named "myVector.csv", "myArray.csv" and "myLongVector.csv".

If the same algorithm is being run by two different threads but you wish to have outputs in different paths, run this command inside the thread:

```C++
SET_DUMPERS_PATH(path);
```

This command will create a directory in the specified path and each thread will dump the contents of the same variable to their respective files.

Variable-Dumper also supports setting the precision of the doubles to be printed.

```C++
SET_DUMPER_PRECISION("myVectorOfDoubles", 12);
SET_DUMPERS_PRECISION(13);
```

You can also customize the delimiters used for the csv file for each individual dumper or all of them.

```C++
SET_DUMPERS_CSV_DELIMITERS(',', ';');
SET_DUMPER_CSV_DELIMITERS("myVector", ';', '\n');
```

Any time you modify a dumper by name, it will only affect the one with the same name and in the current thread. However, when modifying all of them it will affect all the dumpers within all threads.

The following example demostrates how to run two threads for the same algorithm and have the output of the same variable in two different folders.

```C++
#include "DumperManager.h"
#include <vector>
#include <thread>
#include <string>

using namespace VariableDumper;

void runThreadTask(const std::string path, const std::string dumperName, int array[], int size)
{
  // Sets the path of the dumpers associated to this thread
  SET_DUMPERS_PATH(path);

  INIT_DUMPER(dumperName, size);
  DUMP_VAR(array, dumperName);

  // Destroys the dumpers associated to this thread
  DESTROY_DUMPERS();
}

int main()
{
  std::string path = OUTPUT_DIR;
  std::string dumperName1 = "array1";
  std::string dumperName2 = "array2";
  std::string path1 = path + "thread_1/";
  std::string path2 = path + "thread_2/";

  constexpr int size = 4;
  int array1[size] = { 1, 2, 3, 4 };
  int array2[size] = { 1, 2, 3, 4 };

  std::thread t1(runThreadTask, path1, dumperName1, array1, size);
  std::thread t2(runThreadTask, path2, dumperName2, array2, size);
  t1.join();
  t2.join();
  
  return false;
}
```

To easily disable Variable-Dumper just add this line above inclusion of the DumperManager header file.

```C++
#define VARIABLE_DUMPER_DISABLED
```

