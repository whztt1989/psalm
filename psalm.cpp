/*!
*	@file	psalm.cpp
*	@brief	Main file for "psalm"
*
*	"psalm" (Pretty Subdivision ALgorithms on Meshes) is a CLI tool that
*	implements the Doo-Sabin and Catmull-Clark subdivision schemes.
*/

#include <iostream>
#include <sstream>

#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

#include "mesh.h"

using namespace std;

mesh scene_mesh;
string input;
string output;

/*!
*	Shows usage information for the program.
*/

void show_usage()
{
	cout	<< "psalm\n\n"
		<< "Usage: psalm [arguments] [file...]\n\n"
		<< "Arguments:\n\n"
		<< "--algorithm <algorithm>\tSelect subdivision algorithm (default: Catmull-Clark)\n"
		<< "-a\n"
		<< "--type <type>\tSelect type of input data\n"
		<< "-t\n"
		<< "--output <file>\tSet output file\n"
		<< "-o\n"
		<< "--steps <n>\tSet nunmber of subdivision steps to perform (default: 0)\n"
		<< "-n\n"
		<< "--help\tShow this screen\n"
		<< "-h\n\n";
}

/*!
*	Handles user interaction.
*
*	@param argc Number of command-line arguments
*	@param argv Vector of command-line arguments
*/

int main(int argc, char* argv[])
{
	static option cmd_line_opts[] =
	{
		{"output",	required_argument,	NULL,	'o'},
		{"steps",	required_argument,	NULL,	'n'},
		{"type",	required_argument,	NULL,	't'},
		{"algorithm",	required_argument,	NULL,	'a'},

		{"help",	no_argument,		NULL,	'h'},

		{NULL, 0, NULL, 0}
	};

	short type	= mesh::TYPE_EXT;
	short algorithm	= mesh::ALG_CATMULL_CLARK;

	size_t steps	= 0;

	int option = 0;
	while((option = getopt_long(argc, argv, "o:n:t:a:h", cmd_line_opts, NULL)) != -1)
	{
		switch(option)
		{
			case 'o':
				output = optarg;
				break;

			case 't':
			{
				string type_str = optarg;
				transform(type_str.begin(), type_str.end(), type_str.begin(), (int(*)(int)) tolower);

				if(type_str == "ply")
					type = mesh::TYPE_PLY;
				else if(type_str == "obj")
					type = mesh::TYPE_OBJ;
				else if(type_str == "off")
					type = mesh::TYPE_OFF;
				else
				{
					cerr << "Error: \"" << type_str << "\" is an unknown mesh data type.\n";
					show_usage();
					return(-1);
				}

				break;
			}

			case 'a':
			{
				string algorithm_str = optarg;
				transform(algorithm_str.begin(), algorithm_str.end(), algorithm_str.begin(), (int(*)(int)) tolower);

				if(	algorithm_str == "catmull-clark"	||
					algorithm_str == "catmull"		||
					algorithm_str == "clark"		||
					algorithm_str == "cc")
					algorithm = mesh::ALG_CATMULL_CLARK;
				else if(algorithm_str == "doo-sabin"		||
					algorithm_str == "doo"			||
					algorithm_str == "sabin"		||
					algorithm_str == "ds")
					algorithm = mesh::ALG_DOO_SABIN;
				else if(algorithm_str == "loop"	||
					algorithm_str == "l")
					algorithm = mesh::ALG_LOOP;
				else
				{
					cerr << "Error: \"" << algorithm_str << "\" is an unknown algorithm.\n";
					show_usage();
					return(-1);
				}

				break;
			}

			case 'n':
			{
				istringstream converter(optarg);
				converter >> steps;
				if(converter.fail())
				{
					cerr << "Error: Unable to  convert \"" << optarg << "\" to a number.\n";
					show_usage();
					return(-1);
				}
				break;
			}

			case 'h':
			case '?':
				show_usage();
				return(0);
		}
	}

	// Read further command-line parameters; these are all supposed to be
	// input files. If the user already specified an output file, only one
	// input file will be accepted.

	vector<string> files;
	while(optind < argc)
	{
		files.push_back(argv[optind++]);
		if(output.length() != 0 && files.size() > 1)
		{
			cerr << "Error: Output file specified, but more than one input file present.\n";
			return(-1);
		}
	}

	// Replace the special file "-" by an empty string, thereby signalling
	// that standard input and standard output are to be used as file
	// streams.

	for(vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		if(it->length() == 1 && (*it)[0] == '-')
			*it = "";
	}

	bool output_set = (output.length() > 0);
	if(output.length() == 1 && output[0] == '-')
		output = "";

	// Try to read from STDIN if no input files have been specified
	if(files.size() == 0)
		files.push_back("");

	// Apply subdivision algorithm to all files

	for(vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		scene_mesh.load(*it, type);
		scene_mesh.subdivide(algorithm, steps);

		// If an output file has been set (even if it is empty), it
		// will be used.
		if(output_set)
			scene_mesh.save(output, type);

		// If no output file has been set and the input file name is
		// not empty, the output will be written to a file.
		else if(it->length() > 0)
		{
			size_t ext_pos = (*it).find_last_of(".");
			if(ext_pos == string::npos)
				scene_mesh.save(*it+".subdivided", type);
			else
				scene_mesh.save( (*it).substr(0, ext_pos) + "_subdivided"
						+(*it).substr(ext_pos));
		}

		// If no output file has been set and the input file name is
		// empty, the output will be written to STDOUT.
		else
			scene_mesh.save("", type);
	}

	return(0);
}
