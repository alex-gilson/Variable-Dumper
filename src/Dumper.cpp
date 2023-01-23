#include "Dumper.h"

namespace VariableDumper
{

Dumper::Dumper(std::string fileName, int* audio_ptr, int dumpSize, int maxCount, int auPMax)
	: fileName_(fileName)
	, audioPtr_(audio_ptr)
	, dumpSize_(dumpSize)
	, maxCount_(maxCount)
	, auPMax_(auPMax)
	, count_(0)
	, data_()
	, precisionDigits_(15)
	, outFile_(fileName_, std::ios::out)
{
	outFile_ << std::setprecision(precisionDigits_);
}

void Dumper::setPrecision(int precision)
{
	precisionDigits_ = precision;
	outFile_ << std::setprecision(precisionDigits_);
}

Dumper::~Dumper()
{
	if (storeDataAndDump_)
	{
		outFile_ << data_;
	}
	outFile_.close();
}

} // namespace VariableDumper
