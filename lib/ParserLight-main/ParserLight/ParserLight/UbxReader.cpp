#include "stdafx.h"
#include "UbxReader.h"
#include "myConstants.h"
#include <string>
#include <cstring>
#include <vector>
#include <bitset>

UbxReader::UbxReader()
{
	file_size = 0;
}

UbxReader::UbxReader(double rate)
{
	file_size = 0;
}

UbxReader::~UbxReader()
{

}

bool UbxReader::open_file(std::string file_name){
	bool check = false;

	// Read input binary file
	int length = 0;
	int k = 0;

	std::cout << "\n*** load file\n";

	file.open(file_name, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		std::cout << "File open\n";
		file.seekg(0, std::ios::end);
		file_size = file.tellg();
		file.seekg(0, std::ios::beg);
		check = true;
	}
	else
	{
		std::cout << "Unable to open file\n";
		check = false;
	}
	return check;
}

void UbxReader::close_file(){
	file.close();
}


void UbxReader::read_any_message(std::string output_file_name)
{
	double progress;
	double progress_old = -1;

	// open all output files
	for (int i = 0; i < registered_writers.size(); i++)
	{
		registered_writers.at(i)->open_file(output_file_name);
	}

	while (true)
	{		
		if (file.tellg() >= file_size)
		{
			break;
		}
		char message[myConstants::MAX_MESSAGE_LENGTH];
		
		bool found = false;
		UbloxSpecs::UBX_HEAD ub_head_out;
		UbxReader::extract_next_message(message, &ub_head_out, &found);
		unsigned char head[4] = {0xb5, 0x62, ub_head_out.cl, ub_head_out.id}; // message header

		if (found)
		{
			// go through all writers
			for (int i = 0; i < registered_writers.size(); i++)
			{
				// if a registered writer is found, go on
				// compare the header of the current message to the reference header of the writers
				int cl = registered_writers.at(i)->get_reference_header().cl;
				int id = registered_writers.at(i)->get_reference_header().id;



				if (ub_head_out.cl == cl && ub_head_out.id == id)
				{
					// std::cout << std::showbase << std::internal << std::setfill('0');
					// std::cout << "Message found : " << std::hex << static_cast<int>(ub_head_out.cl) << " " << static_cast<int>(ub_head_out.id) << "\n";
					registered_writers.at(i)->parse_and_write(message);
				}
			}

			// std::cout << std::showbase << std::internal << std::setfill('0');
			// std::cout << "Message found : " << std::hex << static_cast<int>(ub_head_out.cl) << " " << static_cast<int>(ub_head_out.id) << "\n";

		}
	}
	
	// close all output files
	for (int i = 0; i < registered_writers.size(); i++)
	{
		registered_writers.at(i)->close_file();
	}
	
	return;
}




void UbxReader::extract_next_message(char *message, UbloxSpecs::UBX_HEAD *head_out, bool *found)
{
	UbloxSpecs::UBX_HEAD ub_head;
	UbloxSpecs::UBX_CHECKSUM ub_chksum;
	*found = false;
	unsigned char ub[2] = {0xb5, 0x62};

	// search 
	while (file.tellg() < (size_t)file_size - 6)
	{
		char file_section[6];
		file.read(file_section, 6);
		file.seekg(-6, std::ios::cur);

		if (memcmp(&file_section[0], &ub, 2) == 0) // any message found
		{
			memcpy(&ub_head.cl, &file_section[2], 1);	// get the class
			memcpy(&ub_head.id, &file_section[3], 1);	// get the id
			memcpy(&ub_head.len, &file_section[4], 2);	// get the length of the message	

			if (ub_head.len > myConstants::MAX_MESSAGE_LENGTH) // too long messages are ignored
			{
				file.seekg(1, std::ios::cur); // increment cursor position
				continue;
			}

			if ((size_t)file_size - file.tellg() >= 8 + ub_head.len)
			{
				// Checksum
				char payload[myConstants::MAX_MESSAGE_LENGTH + 2];
				file.read(payload, ub_head.len + 8);
				file.seekg(-ub_head.len - 8, std::ios::cur);

				ub_chksum.a = payload[6 + ub_head.len];
				ub_chksum.b = payload[6 + ub_head.len + 1];

				if (UbloxSpecs::ub_checksum(&payload[6], ub_head, ub_chksum)) // checksum valid
				{
					memcpy(message, payload, ub_head.len + 8); // message is copied
					
					file.seekg(6 + ub_head.len + 2, std::ios::cur); // cursor position in file is advanced
					*found = true; // flag is set to true
					head_out->cl = ub_head.cl;
					head_out->id = ub_head.id;
					return;
				}
				else // checksum invalid
				{
					file.seekg(1, std::ios::cur);
					continue;
				}
			}
			else // end of bitstring reached
			{
				break;
			}
		}

		// no message found at all
		file.seekg(1, std::ios::cur);
	}

	std::cout << " EOF reached\n";
	file.seekg(0, std::ios::end);
	message = NULL;
	return;
}

void UbxReader::register_reader(OutputWriter * my_writer)
{
	registered_writers.push_back(my_writer);
}