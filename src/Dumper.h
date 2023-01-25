#pragma once
#include <fstream>
#include <iomanip>
#include <unordered_map>

namespace VariableDumper
{

template <typename T>
class isComplex
{
private:
	typedef char YesType[1];
	typedef char NoType[2];

	template <typename C> static YesType& test(decltype(&C::r));
	template <typename C> static NoType& test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

template <typename T>
class isContainer
{
private:
	typedef char YesType[1];
	typedef char NoType[2];

	template <typename C> static YesType& test(decltype(&C::size));
	template <typename C> static NoType& test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

class Dumper
{

	const std::string fileName_;
	const int dumpSize_;
	const int maxCount_;
	int count_;
	std::ofstream outFile_;
	int precisionDigits_;

public:
	Dumper(std::string fileName, int dumpSize, int maxCount);
	Dumper(Dumper& other) = delete;
	~Dumper();
	void setPrecision(int precision);
	template<typename T>
	void dumpVal(T& val)
	{
		using remove_pointer_t = typename std::remove_pointer<T>::type;
		if constexpr (isComplex<remove_pointer_t>::value)
		{
			outFile_ << val.r << ";";
		}
		else
		{
			outFile_ << val << ";";
		}
	}
	// Default dumper
	template<typename T>
	void dump(T& buf)
	{
		using remove_pointer_t = typename std::remove_pointer<T>::type;

		if (count_ > maxCount_ && maxCount_ != -1)
		{
			return;
		}

		if constexpr (isContainer<remove_pointer_t>::value)
		{
			int i = 0;
			for (auto& val : buf)
			{
				if (!(dumpSize_ == -1 || i < dumpSize_))
				{
					break;
				}
				dumpVal(val);
				i++;
			}
		}
		else
		{
			for (int i = 0; i < dumpSize_; i++)
			{
				dumpVal(buf[i]);
			}
		}
		outFile_ << "\n";
		count_++;
	}
};

} // namespace VariableDumper
