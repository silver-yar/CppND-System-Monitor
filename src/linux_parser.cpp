#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <cassert>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  	int index = 0;
  	int value;
  	string key, size = "";
  	std::array<float, 2> values {0, 0};
  	float& mem_total = values[0];
    float& mem_free = values[1];
  	string line;
  	std::ifstream mem_stream(kProcDirectory + kMeminfoFilename);
  	if (mem_stream.is_open()) {
      while(std::getline(mem_stream, line)) {
        std::istringstream linestream(line);     
        while(linestream >> key >> value >> size && values.back() == 0) {
        	values[index] = value;
          	index++;
        }  
      }
      mem_stream.close();
    }
  	return (mem_total - mem_free) / mem_total;
//   	return mem_free;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  	long up_time, idle_time;
	std::ifstream ut_stream(kProcDirectory + kUptimeFilename);
  	if (ut_stream.is_open()) {
      	string line;
      	std::getline(ut_stream, line);
        std::istringstream linestream(line);
        linestream >> up_time >> idle_time;    
    }
  	return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  	long total_jiffies = 0;
	auto cpu_str = LinuxParser::CpuUtilization();
  	vector<long> cpu_jiffs;
  	for (auto& j : cpu_str) 
    	cpu_jiffs.emplace_back(std::stoi(j));
    
  	for (auto& v : cpu_jiffs)
      total_jiffies += v;

	return total_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  	string skip;
  	long utime, stime, cutime, cstime;
  	int index = 0;
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  	if (stream.is_open()) {
    	string line;
      	std::getline(stream, line);
		std::istringstream linestream(line);
      	while (index < 13) {
          linestream >> skip;
          index++;
        }
      	linestream >> utime >> stime >> cutime >> cstime;
      	return utime + stime + cutime + cstime;
    }
	return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
	return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
	auto cpu_str = LinuxParser::CpuUtilization();
  	vector<long> cpu_jiffs;
  	for (int i = 0; i < cpu_str.size(); i++) {
    	cpu_jiffs.emplace_back(std::stoi(cpu_str[i]));
    }
  
  	return cpu_jiffs[kIdle_] + cpu_jiffs[kIOwait_];
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
	vector<string> cpu_aggr;
  	std::ifstream cpu_stream(kProcDirectory + kStatFilename);
  	if(cpu_stream.is_open()) {
     	string line;
      	string value;
      	std::getline(cpu_stream, line);
      	std::istringstream linestream(line);
        while(linestream >> value) {
        	cpu_aggr.emplace_back(value);
        }
      	cpu_aggr.erase(cpu_aggr.begin());
    }
  	return cpu_aggr;
}

int LinuxParser::PProc(std::string choice) {
	string key;
  	int value;
	std::ifstream proc_stream(kProcDirectory + kStatFilename);
  	if (proc_stream.is_open()) {
    	for (string line; std::getline(proc_stream, line);) {
        	std::istringstream linestream(line);
          	linestream >> key >> value;
          	if (key == choice)
              return value;
        }
    }
  	return 0;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return PProc("processes"); }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return PProc("procs_running"); }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  	if (stream.is_open()) {
    	string line;
      	std::getline(stream, line);
      	return line;
    }
  	return "n/a";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  	float kb;
  	string key;
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  
  	if (stream.is_open()) {
    	for (string line; std::getline(stream, line);) {
        	std::istringstream linestream(line);
          	linestream >> key;
          	if (key == "VmSize:") {
            	linestream >> kb;
              	int mb = static_cast<int>(floor(kb*.001f));
              	return std::to_string(mb); // convert to mb
            }
        }
    }
  	return "n/a";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
	std::ifstream pid_stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  	string key, uid;
  	if (pid_stream.is_open()) {
    	for (string aline; std::getline(pid_stream, aline);) {
        	std::istringstream alinestream(aline);
          	alinestream >> key >> uid;
          	if (key == "Uid:")
              return uid;
        }
    }
  	return "";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
	std::ifstream pass_stream(kPasswordPath);
  	string uid = LinuxParser::Uid(pid);
  
    if (pass_stream.is_open()) {
        string user, spacer, uid_comp;
      	for (string line; std::getline(pass_stream, line);) {
        	std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            linestream >> user >> spacer >> uid_comp;
            if (uid == uid_comp) {
            	return user;
            }
        }
    }
	return uid;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  	string skip;
  	long start_time;
  	int index = 0;
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  	if (stream.is_open()) {
    	string line;
      	std::getline(stream, line);
		std::istringstream linestream(line);
      	while (index < 21) {
          linestream >> skip;
          index++;
        }
      	linestream >> start_time;
      	return LinuxParser::UpTime() - (start_time / sysconf(_SC_CLK_TCK)); // convert to seconds
    }
  	return 0;
}