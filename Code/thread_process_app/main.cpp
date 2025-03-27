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

class SystemInfo {
public:
    static void print() {
        struct sysinfo sys_info;
        struct utsname uts;

        if (sysinfo(&sys_info) != 0) {
            throw std::runtime_error("Failed to get system info");
        }

        if (uname(&uts) != 0) {
            throw std::runtime_error("Failed to get uname info");
        }

        std::cout << "Number of processors: " << get_nprocs() << "\n";
        std::cout << "Hostname: " << uts.nodename << "\n";
        std::cout << "Hardware platform: " << uts.machine << "\n";
        std::cout << "Total memory: " << sys_info.totalram / (1024 * 1024) << " MB\n";
    }
};

class ProcessManager {
public:
    static void forkProcesses(int count) {
        std::vector<pid_t> pids;
        int sum = 0;

        for (int i = 0; i < count; ++i) {
            pid_t pid = fork();
            if (pid < 0) {
                throw std::runtime_error("Fork failed");
            }
            else if (pid == 0) {
                // Child process
                std::cout << "Child PID: " << getpid() << std::endl;
                exit(getpid() % 1000);  // Return PID modulo 1000
            }
            else {
                pids.push_back(pid);
            }
        }

        // Parent process
        for (pid_t pid : pids) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                sum += WEXITSTATUS(status);
            }
        }

        std::cout << "Sum of PIDs (mod 1000): " << sum << std::endl;
    }
};

class ThreadManager {
public:
    static void createThreads(int count) {
        std::vector<std::thread> threads;
        int sum = 0;
        std::mutex sum_mutex;

        for (int i = 0; i < count; ++i) {
            threads.emplace_back([&sum, &sum_mutex]() {
                std::thread::id id = std::this_thread::get_id();
                std::cout << "Thread ID: " << id << std::endl;

                std::lock_guard<std::mutex> lock(sum_mutex);
                sum += hashThreadId(id);
                });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        std::cout << "Sum of thread IDs (mod 1000): " << sum % 1000 << std::endl;
    }

private:
    static int hashThreadId(const std::thread::id& id) {
        std::hash<std::thread::id> hasher;
        return static_cast<int>(hasher(id) % 1000);
    }
};

class ArgumentParser {
public:
    static void parse(int argc, char* argv[]) {
        if (argc < 2) {
            throw std::invalid_argument("Usage: " + std::string(argv[0]) + " [-i] [-f X] [-t X]");
        }

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "-i") {
                SystemInfo::print();
            }
            else if (arg == "-f" && i + 1 < argc) {
                int count = parseCount(argv[++i]);
                ProcessManager::forkProcesses(count);
            }            else if (arg == "-t" && i + 1 < argc) {
                int count = parseCount(argv[++i]);
                ThreadManager::createThreads(count);
            }
            else {
                throw std::invalid_argument("Unknown option: " + arg);
            }
        }
    }

private:
    static int parseCount(const char* str) {
        try {
            int count = std::stoi(str);
            if (count <= 0) throw std::invalid_argument("Count must be positive");
            return count;
        }
        catch (const std::exception& e) {
            throw std::invalid_argument("Invalid count: " + std::string(str));
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        ArgumentParser::parse(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}