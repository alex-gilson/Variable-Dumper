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

class Dumper
{
	const std::string fileName_;
	const int* audioPtr_;
	const int dumpSize_;
	const int maxCount_;
	const int auPMax_;
	int count_;
	std::string data_;
	std::ofstream outFile_;
	const bool storeDataAndDump_ = false;
	int precisionDigits_;

public:
	Dumper(std::string fileName, int* audio_ptr, int dumpSize, int maxCount, int auPMax);
	Dumper(Dumper& other) = delete;
	~Dumper();
	void setPrecision(int precision);
	// Default dumper
	template<typename T>
	void dump(T buf)
	{
		if (auPMax_ != -1 && !audioPtr_) { return; }
		using remove_pointer_t = typename std::remove_pointer<T>::type;

		static_assert(std::is_pointer<T>::value);
		//static_assert(std::is_floating_point<remove_pointer_t>::value || isComplex<remove_pointer_t>::value);

		if ((count_ != -1 && count_ > maxCount_ && maxCount_ != -1) ||
			(audioPtr_ != nullptr && *audioPtr_ > auPMax_ && auPMax_ != -1)) {
			return;
		}

		// If it's the first dump and we know the number of samples, reserve memory
		if (storeDataAndDump_ && !count_ && auPMax_ != -1)
		{
			data_.reserve(static_cast<long long>(auPMax_) * precisionDigits_);
		}

		// Store in a format understandable by numpy in python
		for (int i = 0; i < dumpSize_; i++)
		{
			std::stringstream ss;
			ss.precision(precisionDigits_);
			if (storeDataAndDump_)
			{
				if constexpr (isComplex<remove_pointer_t>::value)
				{
					ss << buf[i].r << ";";
				}
				else
				{
					ss << buf[i] << ";";
				}
				data_ += std::move(ss.str());
			}
			else
			{
				if constexpr (isComplex<remove_pointer_t>::value)
				{
					outFile_ << buf[i].r << ";";
				}
				else
				{
					outFile_ << buf[i] << ";";
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

} // namespace VariableDumper
