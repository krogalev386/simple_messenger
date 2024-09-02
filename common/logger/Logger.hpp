#pragma once

#include <sys/time.h>

#include <memory>
#include <mutex>
#include <string>

#include "CrtpSingleton.hpp"

#define LOG(x...) Logger::getInstance().log(__func__, x);

class Logger : public CrtpSingleton<Logger> {
    friend class CrtpSingleton<Logger>;

   protected:
    Logger() { log_file_handle = std::fopen("runtime.log", "w"); };

    ~Logger() { std::fclose(log_file_handle); };
    Logger(const Logger&) = delete;
    Logger(Logger&&)      = delete;

   public:
    template <class... Args>
    void log(const char* func_name, Args... args) {
        std::unique_lock<std::mutex> lock(log_mtx);
        char datestamp_str[std::size("yyyy-mm-ddThh:mm:ssZ")];

        timeval curTime;
        gettimeofday(&curTime, NULL);
        int usecs = curTime.tv_usec;
        strftime(datestamp_str, std::size(datestamp_str), "%FT%TZ",
                 localtime(&curTime.tv_sec));

        fprintf(log_file_handle, "%s", datestamp_str);
        fprintf(log_file_handle, "%6.d: ", usecs);
        fprintf(log_file_handle, "%s: ", func_name);
        fprintf(log_file_handle, args...);
        fprintf(log_file_handle, "\n");
        fflush(log_file_handle);
    }

   private:
    std::mutex log_mtx;
    std::FILE* log_file_handle;
};
