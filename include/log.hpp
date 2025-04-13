#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <mutex>
#include <optional>

enum class LogLevel
{
	Info,
	Error
};

struct LogEntry
{
	LogLevel level;
	std::string message;
	std::string timestamp;
	unsigned int calls;
};

inline std::string currentTimeString()
{
	auto now_tp = std::chrono::system_clock::now();

	auto now_c = std::chrono::system_clock::to_time_t(now_tp);

	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_tp.time_since_epoch()) % 1000;

	return fmt::format("{:%T}.{:03}", fmt::localtime(now_c), ms.count());
}

class log 
{
public:

	static void initFile(const std::string& filename)
    {
        auto& instance = getInstance();
    	std::lock_guard<std::mutex> lock(instance.mtx);
    	instance.fileStream.emplace(filename, std::ios::out | std::ios::trunc);
    }

	template<typename... Args>
	static void error(const std::string& formatStr, Args&&... args)
	{
		std::string msg = fmt::format(fmt::runtime(formatStr), std::forward<Args>(args)...);
		std::string now = currentTimeString();
		auto& instance = getInstance();
		{
			std::lock_guard<std::mutex> lock(instance.mtx);
			bool updated = false;
			for (auto& entry : instance.entries)
			{
				if (entry.level == LogLevel::Error && entry.message == msg)
				{
					entry.timestamp = now;
					entry.calls += 1;
					updated = true;
					break;
				}
			}
			if (!updated)
			{
				instance.entries.push_back({LogLevel::Error, msg, now});
			}

			if(instance.fileStream && instance.fileStream->is_open())
            {
                *instance.fileStream << "[ERROR] " << now << " - " << msg << "\n";
                instance.fileStream->flush();
            }
		}
	}

	template<typename... Args>
	static void info(const std::string& formatStr, Args&&... args)
	{
		std::string msg = fmt::format(fmt::runtime(formatStr), std::forward<Args>(args)...);
		std::string now = currentTimeString();
		auto& instance = getInstance();
		{
			std::lock_guard<std::mutex> lock(instance.mtx);
			instance.entries.push_back({LogLevel::Info, msg, now});
			if(instance.fileStream && instance.fileStream->is_open())
            {
                *instance.fileStream << "[INFO] " << now << " - " << msg << "\n";
                instance.fileStream->flush();
            }
		}
	}

	static void printAll()
    {
        auto& instance = getInstance();
        std::vector<LogEntry> entriesCopy;
        {
            std::lock_guard<std::mutex> lock(instance.mtx);
            entriesCopy = instance.entries;
        }

        std::string buffer;

        buffer.append("\033[H\033[J");

        for (const auto &entry : entriesCopy)
        {
            if (entry.level == LogLevel::Error)
            {
                fmt::format_to(std::back_inserter(buffer),
                    "\033[31m[ERROR]\033[0m [{}] {} [{}]\n", entry.timestamp, entry.message, entry.calls);
            }
            else
            {
                fmt::format_to(std::back_inserter(buffer),
                    "\033[32m[INFO]\033[0m [{}] {}\n", entry.timestamp, entry.message);
            }
        }

        fmt::print("{}", buffer);

        fflush(stdout);
        fflush(stderr);
    }
	
private:
	log() = default;
	~log() = default;
	
	log(const log&) = delete;
	log& operator=(const log&) = delete;
	
	static log& getInstance()
	{
		static log instance;
		return instance;
	}

	std::vector<LogEntry> entries;
	std::mutex mtx;
	std::optional<std::ofstream> fileStream;
};
