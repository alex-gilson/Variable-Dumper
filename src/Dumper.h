#pragma once
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <complex>

namespace VariableDumper
{

class Dumper
{
	template<typename T>
	struct is_complex_t : public std::false_type {};

	template<typename T>
	struct is_complex_t<std::complex<T>> : public std::true_type {};

	template <typename T>
	class is_container_t
	{
	private:
		typedef char YesType[1];
		typedef char NoType[2];

		template <typename C> static YesType& test(decltype(&C::size));
		template <typename C> static NoType& test(...);

	public:
		enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
	};

	const std::string name_;
	int maxDumps_;
	int count_;
	std::ofstream outFile_;
	int precisionDigits_;
	char valueDelimiter_;
	char lineDelimiter_;
	int currentDimension_;

public:
	Dumper(std::string name, int precision, char valueDelimiter, char lineDelimiter);
	Dumper(Dumper& other) = delete;
	~Dumper();
	void setPrecision(int precision);
	void setCSVDelimiters(char valueDelimiter, char lineDelimiter);
	void setMaxDumps(int maxDumps);
	template<typename T>
	void dumpVal(T& val, bool lastElementOfLine)
	{
		static_assert(!is_container_t<std::remove_pointer<T>>::value);
		if constexpr (is_complex_t<T>::value)
		{
			outFile_ << val.real() << valueDelimiter_ << val.imag();
		}
		else
		{
			outFile_ << val;
		}
		if (!lastElementOfLine)
		{
			outFile_ << valueDelimiter_;
		}
	}
	template<typename T>
	void dump(T& buf, int dumpSize)
	{
		currentDimension_++;
		if (currentDimension_ == 1)
		{
			count_++;
		}
		using element_of_container_t = std::remove_reference_t<decltype(std::declval<T>()[0])>;

		// Check that the elements of the container are also containers but not of type std::string
		if constexpr (is_container_t<element_of_container_t>::value
			&& !std::is_same<element_of_container_t, std::string>::value)
		{
			// Unwrap the uppermost container
			for (auto& subContainer : buf)
			{
				dump(subContainer, dumpSize);
			}
		}
		else
		{
			if (maxDumps_ != -1 && count_ >= maxDumps_)
			{
				currentDimension_--;
				return;
			}

			if constexpr (is_container_t<typename std::remove_pointer<T>::type>::value)
			{
				int i = 0;
				for (auto& val : buf)
				{
					bool lastElementOfLine = (i == (buf.size() - 1) || i == (dumpSize - 1));
					dumpVal(val, lastElementOfLine);
					if (lastElementOfLine)
					{
						break;
					}
					i++;
				}
			}
			else
			{
				for (int i = 0; i < dumpSize; i++)
				{
					dumpVal(buf[i], i == (dumpSize - 1));
				}
			}
			outFile_ << lineDelimiter_;
		}
		currentDimension_--;
	}
};

} // namespace VariableDumper
