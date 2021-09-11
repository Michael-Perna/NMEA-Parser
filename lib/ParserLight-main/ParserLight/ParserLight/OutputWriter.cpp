#include "stdafx.h"
#include "OutputWriter.h"
#include "Utils.h"
#include "myConstants.h"
#include <string>
#include <time.h>
#include <cstring>
#include <algorithm>
#include <iostream>

#include <sstream>

double OutputWriter::rate = 0;
double OutputWriter::threshold = 0; // in seconds

OutputWriter::OutputWriter()
{
}

OutputWriter::~OutputWriter()
{
}

void OutputWriter::parse_file(char * inbuffer)
{
}

void OutputWriter::open_file(std::string file_name)
{
	file_name = file_name + ".out";
	file.open(file_name);
	return;
}

void OutputWriter::close_file()
{
	file.close();
	return;
}

UbloxSpecs::UBX_HEAD OutputWriter::get_reference_header()
{
	std::cout << "Base called \n";

	return UbloxSpecs::UBX_HEAD();
}

void OutputWriter::parse_and_write(char *inbuffer)
{
	return;
}

