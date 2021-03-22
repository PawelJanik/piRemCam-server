/*
 * Server to remote control digital cameras
 * 
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

#include <getopt.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>

#include <nlohmann/json.hpp>
#include <mosquitto.h>
#include <daemoncpp.h>

#include "cameralist.h"
#include "process.h"

DaemonCpp * daemon;

struct mosquitto * mosq = NULL;

static char *app_name;
char * confFileName;
char * logFileName;

CameraList * cameraList;

nlohmann::json jsonConfig;

/**
 * \brief Print help for this application
 */
void print_help()
{
	printf("\n Usage: %s [OPTIONS]\n\n", app_name);
	printf("  Options:\n");
	printf("   -h --help                 Print this help\n");
	printf("   -c --conf_file filename   Read configuration from the file\n");
	printf("   -l --log_file  filename   Write logs to the file\n");
	printf("\n");
}

/**
* @brief Read and analize arguments
* 
* @param argc p_argc
* @param argv p_argv
* @return int: exit code
*/
int readArguments(int argc, char *argv[])
{
	app_name = argv[0];
	
	static struct option long_options[] = 
	{
		{"conf_file", required_argument, 0, 'c'},
		{"log_file", required_argument, 0, 'l'},
		{"help", no_argument, 0, 'h'},
		{NULL, 0, 0, 0}
	};

	int value, option_index = 0;
	while ((value = getopt_long(argc, argv, "c:l:h", long_options, &option_index)) != -1) 
	{
		switch (value) 
		{
			case 'c':
				confFileName = strdup(optarg);
				break;
			case 'l':
				logFileName = strdup(optarg);
				break;
			case 'h':
				print_help();
				return EXIT_SUCCESS;
			default:
				print_help();
				return EXIT_SUCCESS;
				break;
		}
	}
	return 1;
}

int readConfigFile()
{
	daemon->log("Try to open config file");

	if(confFileName != NULL)
	{
		std::ifstream configFile(confFileName);
		
		if (configFile.is_open())
		{
			daemon->log("Config file is open");
			
			daemon->log("Reading config file");
			configFile >> jsonConfig;
			daemon->log("Readed config file");
			
			configFile.close();
			daemon->log("Config file is close");
		}
		else 
		{
			daemon->log("Unable to open config file [ERROR]");
			return -1;
		}
	}
	return 1;
}

void mosquittoMessage(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
	
}

void loop()
{
	std::this_thread::sleep_for (std::chrono::seconds(1));
	mosquitto_loop(mosq, -1, 1);
	mosquitto_publish(mosq,NULL,"piRemCam/status",2,"OK",0,false);
	
	std::string jsonString = cameraList->getAvaibleCamera().dump();
	mosquitto_publish(mosq,NULL,"piRemCam/avaibleCamera",jsonString.size(), jsonString.c_str(),0,false);
	daemon->log(jsonString);
}

/* Main function */
int main(int argc, char *argv[])
{
	if(readArguments(argc, argv) == EXIT_SUCCESS)
	{
		return EXIT_SUCCESS;
	}
	
	daemon = new DaemonCpp(app_name, logFileName);
	daemon->demonize();
	daemon->setLoopFunction(loop);
	
	if(readConfigFile() < 0)
	{
		delete daemon;
		return EXIT_FAILURE;
	}
	
	daemon->log("Connecting to mosquitto");
	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, true, NULL);
	
	if(!mosq)
	{
		daemon->log("Mosquitto ERROR");
		return EXIT_FAILURE;
	}
	
	mosquitto_message_callback_set(mosq, mosquittoMessage);

	if(mosquitto_connect(mosq, jsonConfig["MQTT"]["HOST"].get<std::string>().c_str(), jsonConfig["MQTT"]["PORT"].get<int>(), 60))
	{
		daemon->log("Unable to connect MQTT [ERROR]");
		return EXIT_FAILURE;
	}
	daemon->log("Connected to mosquitto");
	
	daemon->log("Check gPhoto2");
	std::vector<std::string> gpArguments;
	gpArguments.push_back("--version");
	std::string gpTestResult = Process::exec("gphoto2",gpArguments);
	if(gpTestResult.compare(0,7,"gphoto2") == 0)
	{
		daemon->log("gPhoto2 [OK]");
	}
	else
	{
		daemon->log("gPhoto3 [ERROR]");
		return EXIT_FAILURE;
	}
	
	//cameraList = new CameraList();
	
	daemon->exec();
	
	delete daemon;
	
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	
	return EXIT_SUCCESS;
}
