#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <mutex>

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
					updated = true;
					break;
				}
			}
			if (!updated)
			{
				instance.entries.push_back({LogLevel::Error, msg, now});
			}
		}
		
		//fmt::print(stderr, "\033[31m[ERROR]\033[0m {} - {}\n", now, msg);
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
		}
		
		//fmt::print("\033[32m[INFO]\033[0m {} - {}\n", now, msg);
	}

	static void printAll()
    {
        auto& instance = getInstance();
        std::vector<LogEntry> entriesCopy;
        {
            std::lock_guard<std::mutex> lock(instance.mtx);
            // Copy the current log state to avoid holding the lock during printing.
            entriesCopy = instance.entries;
        }

        // Build the complete frame in a string buffer.
        std::string buffer;

        // --- Clear Screen (Optimized) & Move Cursor Home ---
        // \033[H : Move cursor to Home (top-left)
        // \033[J : Clear from cursor to end of screen
        buffer.append("\033[H\033[J");

        // --- Append the log entries ---
        for (const auto &entry : entriesCopy)
        {
            if (entry.level == LogLevel::Error)
            {
                fmt::format_to(std::back_inserter(buffer),
                    "\033[31m[ERROR]\033[0m {} - {}\n", entry.timestamp, entry.message);
            }
            else // LogLevel::Info
            {
                fmt::format_to(std::back_inserter(buffer),
                    "\033[32m[INFO]\033[0m {} - {}\n", entry.timestamp, entry.message);
            }
        }

        // Print the entire buffer at once.
        fmt::print("{}", buffer);

        // Flush output buffers.
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
};
