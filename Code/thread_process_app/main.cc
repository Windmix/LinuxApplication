/*
//
//Copyright (c) 2025 Windmix
// License: bruh
// Project: thread_process_app
//*/

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <stdint.h>
#include <mutex>
#include <chrono>

class MathUtil {
public:
    static uint64_t calculate_nested_sqrt_sum() 
    {
        uint64_t total = 0.0;
        for (int i = 1; i <= 50000; ++i) // Outer loop: i goes from 1 to 5000
        {
            for (int j = 1; j <= i; ++j) // Inner loop: j goes from 1 to i
            {
                total += std::sqrt(i * j); 
            }
        }
        return total;
    }
};

class SystemInfo {
public:
    static void print()
    {
        struct sysinfo sys_info;
        struct utsname uts;

        if (sysinfo(&sys_info) != 0)
        {
            throw std::runtime_error("Failed to get system info");
        }

        if (uname(&uts) != 0)
        {
            throw std::runtime_error("Failed to get uname info");
        }

        std::cout << "Number of processors: " << get_nprocs() << "\n";
        std::cout << "Hostname: " << uts.nodename << "\n";
        std::cout << "Hardware platform: " << uts.machine << "\n";
        std::cout << "Total virtual memory: " << sys_info.totalram / (1024 * 1024) << " MB\n";
    }
};

class ProcessManager {
public:
    static void forkProcesses(int count)
    {
        uint64_t pid_sum = 0;
        std::vector<pid_t> children;

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < count; ++i) 
        {
            pid_t pid = fork();
            if (pid == 0) 
            { // Child process
                std::cout << "Child "<< pid <<" PID: " << getpid()
                    << " | Math sum: " << MathUtil::calculate_nested_sqrt_sum()
                    << std::endl;
                exit(getpid()); // Return actual PID
            }
            children.push_back(pid);
        }

        // Parent sums PIDs
        for (pid_t child : children) 
        {
            int status;
            waitpid(child, &status, 0);
            if (WIFEXITED(status))
            {
                pid_sum += WEXITSTATUS(status);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Total PID sum: " << pid_sum << std::endl <<"Time taken : " << duration.count() << " seconds" << std::endl;
    }
};

class ThreadManager {
public:
    static void createThreads(int count)
    {
        uint64_t tid_sum = 0;
        uint64_t math_sum_total = 0.0;
        std::mutex mtx;
        std::vector<std::thread> threads;

        auto start = std::chrono::high_resolution_clock::now();
        auto thread_task = [&]() {
            // Calculate math sum
             uint64_t math_result = MathUtil::calculate_nested_sqrt_sum();

            // Get thread ID as number
            auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());

            
            std::lock_guard<std::mutex> lock(mtx);
            tid_sum += tid;
            math_sum_total += math_result;
            

            std::cout << "Thread ID: " << tid
                << " | Math sum: " << math_result
                << std::endl;
            };

        for (int i = 0; i < count; ++i)
        {
            threads.emplace_back(thread_task);
        }

        for (auto& t : threads)
        {
            t.join();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        std::cout << "Total Thread ID sum: " << tid_sum << std::endl << "Time taken : " << duration.count() << " seconds" << std::endl;;
        std::cout << "Combined math sum: " << math_sum_total << std::endl;
    }
};

class ArgumentParser {
public:
    static void parse(int argc, char* argv[])
    {
        if (argc < 2)
        {
            throw std::runtime_error("Usage: ./app [-i] [-f X] [-t X]");
        }

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "-i") 
            {
                SystemInfo::print();
            }
            else if (arg == "-f" && i + 1 < argc)
            {
                ProcessManager::forkProcesses(std::stoi(argv[++i]));
            }
            else if (arg == "-t" && i + 1 < argc)
            {
                ThreadManager::createThreads(std::stoi(argv[++i]));
            }
        }
    }
};
int main(int argc, char* argv[])
{
    double yes;
    double ok;
    double meme;
    try 
    {
        ArgumentParser::parse(argc, argv);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}