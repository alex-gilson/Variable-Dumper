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

public:
	Dumper(std::string name, int precision, char valueDelimiter, char lineDelimiter);
	Dumper(Dumper& other) = delete;
	~Dumper();
	void setPrecision(int precision);
	void setCSVDelimiters(char valueDelimiter, char lineDelimiter);
	void setMaxDumps(int maxDumps);
	template<typename T>
	void dumpVal(T& val)
	{
		if constexpr (is_complex_t<T>::value)
		{
			outFile_ << val.real() << valueDelimiter_ << val.imag() << valueDelimiter_;
		}
		else
		{
			outFile_ << val << valueDelimiter_;
		}
	}
	// Default dumper
	template<typename T>
	void dump(T& buf, int dumpSize)
	{
		using remove_pointer_t = typename std::remove_pointer<T>::type;

		if (count_ >= maxDumps_ && maxDumps_ != -1)
		{
			return;
		}

		if constexpr (is_container_t<remove_pointer_t>::value)
		{
			int i = 0;
			for (auto& val : buf)
			{
				if (!(dumpSize == -1 || i < dumpSize))
				{
					break;
				}
				dumpVal(val);
				i++;
			}
		}
		else
		{
			for (int i = 0; i < dumpSize; i++)
			{
				dumpVal(buf[i]);
			}
		}
		outFile_ << lineDelimiter_;
		count_++;
	}
};

} // namespace VariableDumper
