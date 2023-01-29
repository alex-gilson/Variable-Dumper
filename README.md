![Variable Dumper logo](https://github.com/alex-gilson/Variable-Dumper/blob/main/variable_dumper_logo.png)

# Description

Variable Dumper is a cross-platform C++ library to write to a csv file data contained in arrays or std containers. It has multithreaded support, allowing the developer to write the same variable to different files when an algorithm is being run by two or more threads. Variable-Dumper works with arrays and containers of integer, float, std::string, C string, and std::complex types by default. The user can also define custom class printers.

# Installation

First, set up your project for CMake. Then, in your project directory run

```bash
git submodule add https://github.com/alex-gilson/Variable-Dumper.git
```

Insert the following line to you CMakeLists.txt:

```cmake
add_subdirectory(Variable-Dumper)
```

and

```cmake
target_link_libraries(<your-project-name> PUBLIC VariableDumper)
```

# Usage

Simply add the Variable-Dumper header file to your code:

```C++
#include "VariableDumper.h"
```

Set the path for the dumpers with the command below. Otherwise the current path will be used. The path is set only the dumpers in the current thread.

```C++
SET_DUMPERS_PATH(OUTPUT_PATH);
```

To then write to a file use

```C++
std::vector<int> myVector = {1, 2, 3, 4};
DUMP_VAR("myVector", myVector);
```

For the case where you are dealing with a c style array or when you don't want to print the entirety of a container, you can set up a size.

```C++
int myArray[] = {1,2,3};
DUMP_VAR("myArray", myArray, 3);
```

It is also possible to limit the number of times a variable is printed to file. With the following example, only the first row of myLongVector will be written to file.

```C++
std::vector<std::vector<int>> myLongVector = { { 1, 2, 3, 4}, {5, 6, 7, 8 } };
SET_DUMPER_MAX_DUMPS("myLongVector", 1);
DUMP_VAR("myLongVector", myLongVector[0]);
DUMP_VAR("myLongVector", myLongVector[1]);
DESTROY_DUMPERS();
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

This command will create a directory in the specified path and each thread will dump the contents of the same variable to their respective files. The following example demostrates how to run two threads for the same algorithm and have the output of the same variable in two different folders.

```C++
#include "VariableDumper.h"
#include <vector>
#include <thread>
#include <string>

void runThreadTask(const std::string path, const std::string dumperName, int array[], int size)
{
  // Sets the path of the dumpers associated to this thread
  SET_DUMPERS_PATH(path);
  
  DUMP_VAR(dumperName, array, size);

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
  int array2[size] = { 5, 6, 7, 8 };

  std::thread t1(runThreadTask, path1, dumperName1, array1, size);
  std::thread t2(runThreadTask, path2, dumperName2, array2, size);
  t1.join();
  t2.join();
  
  return false;
}
```

Variable-Dumper also supports setting the precision of the doubles to be printed.

```C++
SET_DUMPER_PRECISION("myVectorOfDoubles", 12);
SET_DUMPERS_PRECISION(13);
```

You can also customize the delimiters used for the csv file for each individual dumper or all of them.

```C++
SET_DUMPER_CSV_DELIMITERS("myVector", ';', '\n');
SET_DUMPERS_CSV_DELIMITERS('|', ';');
```

Any time you modify a dumper by name, it will only affect the one with the same name and in the current thread. However, when modifying all of them it will affect all the dumpers within all threads and will set a default for new dumpers created in all threads.


To easily disable Variable-Dumper just add this line above the inclusion of the DumperManager header file.

```C++
#define VARIABLE_DUMPER_DISABLED
```

To print custom classes that you have created simply overload the << operator returning an std::ostream&.
