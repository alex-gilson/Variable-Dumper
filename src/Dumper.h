#pragma once
#include <fstream>
//#include <iomanip>
#include <unordered_map>

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

class Dumper
{
public:
	const std::string fileName_;
	const int* audioPtr_;
	const int dumpSize_;
	const int bufferSize_;
	const int maxCount_;
	const int auPMax_;
	int count_;
	std::string data_;
	std::ofstream outFile_;
	const bool storeDataAndDump_ = false;

public:
	Dumper(std::string fileName, int* audio_ptr, int dumpSize,
		int bufferSize, int maxCount, int auPMax);
	Dumper(std::string fileName);
	Dumper(Dumper& other) = delete;
	~Dumper();
	// Default dumper
	template<typename T>
	void dump(T buf)
	{
		if (!audioPtr_) { return; }
		using remove_pointer_t = typename std::remove_pointer<T>::type;

		static_assert(std::is_pointer<T>::value);
		static_assert(std::is_floating_point<remove_pointer_t>::value || isComplex<remove_pointer_t>::value);

		if ((count_ > maxCount_ && maxCount_ != -1) ||
			(*audioPtr_ > auPMax_ && auPMax_ != -1)) {
			return;
		}

		// If it's the first dump and we know the number of samples, reserve memory
		if (storeDataAndDump_ && !count_ && auPMax_ != -1)
		{
			constexpr int numberOfCharactersPerDouble = 15;
			data_.reserve(static_cast<long long>(auPMax_) * numberOfCharactersPerDouble);
		}

		// Store in a format understandable by numpy in python
		for (int i = 0; i < dumpSize_; i++)
		{
			std::stringstream ss;
			if (storeDataAndDump_)
			{
				if constexpr (std::is_floating_point<remove_pointer_t>::value)
				{
					//ss << std::setprecision(15) << buf[i] << ";";
				}
				else
				{
					//ss << std::setprecision(15) << buf[i].r << ";";
				}
				data_ += std::move(ss.str());
			}
			else
			{
				if constexpr (std::is_floating_point<remove_pointer_t>::value)
				{
					//outFile_ << std::setprecision(15) << buf[i] << ";";
				}
				else
				{
					//outFile_ << std::setprecision(15) << buf[i].r << ";";
				}
			}
		}
		if (storeDataAndDump_)
		{
			data_ += "\n";
		}
		else
		{
			outFile_ << "\n";
		}
		count_++;
	}
};

