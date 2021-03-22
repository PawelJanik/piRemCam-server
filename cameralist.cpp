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

#include "cameralist.h"

#include <string>
#include <sstream>
#include <istream>

#include "process.h"


CameraList::CameraList()
{

}

CameraList::~CameraList()
{

}

nlohmann::json CameraList::getAvaibleCamera()
{
	std::vector <std::string> arguments;
	arguments.push_back("--auto-detect");
	
	std::string result = Process::exec("gphoto2", arguments);
	result.erase(0,107);
	
	std::stringstream resultStream;
	resultStream << result;
	
	nlohmann::json cameraJsonArray = nlohmann::json::array();;
	std::string line;
	
	while(std::getline(resultStream, line, '\n'))
	{
		if(resultStream.eof()) 
			break;
	
		std::size_t foundPort = line.find("usb:");
	
		if (foundPort!=std::string::npos)
		{
			nlohmann::json j;
			
			std::string port = line.substr(foundPort,11);
			j["port"] = port;

			std::string name = line;
			name.erase(foundPort,11);
			
			name = trim(name);
			
			j["name"] = name;
			
			cameraJsonArray.push_back(j);
		}
	}
	
	nlohmann::json output;
	output["avaibleCamera"] = cameraJsonArray;
	
	return output;
}

nlohmann::json CameraList::getActiveCameraList()
{
	
}

void CameraList::addActiveCamera(std::string adress)
{
	Camera camera;
	camera.cameraAdress = adress;
	activeCameraList.push_back(camera);
}

void CameraList::removeActiveCamea(std::string adress)
{
	
}

std::string CameraList::trim(const std::string &s)
{
	auto start = s.begin();
	while (start != s.end() && std::isspace(*start)) {
		start++;
	}

	auto end = s.end();
	do {
		end--;
	} while (std::distance(start, end) > 0 && std::isspace(*end));

	return std::string(start, end + 1);
}

