/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2020  Paweł Janik <email>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "process.h"

Process::Process(std::string command, std::vector<std::string> arguments)
{

}

Process::Process()
{

	
}

Process::~Process()
{

}

std::string Process::exec()
{
	std::string output;
	
	output = exec(command, arguments);
	
	return output;
}

std::string Process::exec(std::string command, std::vector<std::string> arguments)
{
	command.append(" ");
	for(int i = 0; i < arguments.size(); i++)
	{
		command.append(arguments.at(i));
		command.append(" ");
	}
	
	std::string output;
	
	char buffer[128];

	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) 
	{
		return "popen failed!";
	}

	while (!feof(pipe)) 
	{
		if (fgets(buffer, 128, pipe) != NULL)
		output += buffer;
	}
	pclose(pipe);
	
	return output;
}


