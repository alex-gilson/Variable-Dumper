#include "Dumper.h"

namespace VariableDumper
{

Dumper::Dumper(std::string fileName, int dumpSize, int maxCount)
	: fileName_(fileName)
	, dumpSize_(dumpSize)
	, maxCount_(maxCount)
	, count_(0)
	, outFile_(fileName_ + ".csv", std::ios::out)
	, precisionDigits_(15)
	, valueDelimiter_(',')
	, lineDelimiter_('\n')
{
	outFile_ << std::setprecision(precisionDigits_);
}

void Dumper::setPrecision(int precision)
{
	precisionDigits_ = precision;
	outFile_ << std::setprecision(precisionDigits_);
}

void Dumper::setCSVDelimiters(char valueDelimiter, char lineDelimiter)
{
	valueDelimiter_ = valueDelimiter;
	lineDelimiter_  = lineDelimiter;
}

Dumper::~Dumper()
{
	outFile_.close();
}

} // namespace VariableDumper
