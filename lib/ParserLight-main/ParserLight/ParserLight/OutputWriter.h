#pragma once
#include <string>
#include "UbloxSpecs.h"
#include "myConstants.h"
#include <sstream>
#include <fstream>

class OutputWriter
{
public:
	/* Constructor */
	OutputWriter();
	~OutputWriter();

	std::ofstream file;
	static double rate; // rate valid for all members
	static double threshold;

	/* Methods */
	virtual void parse_file(char *inbuffer);
	
	virtual void open_file(std::string file_name);
	virtual void close_file();

	virtual UbloxSpecs::UBX_HEAD get_reference_header();
	virtual void parse_and_write(char *inbuffer);
};