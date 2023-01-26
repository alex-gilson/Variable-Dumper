#include "Dumper.h"

namespace VariableDumper
{

Dumper::Dumper(std::string name, int precision, char valueDelimiter, char lineDelimiter)
	: name_(name)
	, maxDumps_(-1)
	, count_(0)
	, outFile_(name_ + ".csv", std::ios::out)
	, precisionDigits_(precision)
	, valueDelimiter_(valueDelimiter)
	, lineDelimiter_(lineDelimiter)
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

void Dumper::setMaxDumps(int maxDumps)
{
	maxDumps_ = maxDumps;
}

Dumper::~Dumper()
{
	outFile_.close();
}

} // namespace VariableDumper
