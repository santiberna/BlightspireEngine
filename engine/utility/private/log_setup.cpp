#include "log_setup.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <time.h>

#if BB_PLATFORM == BB_WINDOWS
#include <windows.h>
#elif BB_PLATFORM == BB_LINUX
#include <sys/utsname.h>
#else
#error "Unimplemented"
#endif

std::string bb::getOsName()
{
#if BB_PLATFORM == BB_WINDOWS
    double majorVersion = 0.0;
    double minorVersion = 0.0;

    OSVERSIONINFOEX info;

    // Get the function pointer to RtlGetVersion
    void* procAddress = reinterpret_cast<void*>(GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion"));

    // If successfull then read the function, if not, we're doomed to not know the version
    if (procAddress != NULL)
    {
        using GetVersionProcType = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);
        auto call = reinterpret_cast<GetVersionProcType>(procAddress);
        call((PRTL_OSVERSIONINFOW)&info);
    }

    majorVersion = static_cast<double>(info.dwMajorVersion);
    minorVersion = static_cast<double>(info.dwMinorVersion);
    return "Windows " + std::to_string(majorVersion) + "-" + std::to_string(minorVersion);

#elif BB_PLATFORM == BB_LINUX
    utsname name {};
    uname(&name);

    return std::string(name.sysname) + " " + std::string(name.release);

#else
#error "Unimplemented"
#endif
}

std::string SerializeTimePoint(const std::chrono::system_clock::time_point& time, const std::string& format)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(time);
    std::tm tm {};

#if BB_PLATFORM == BB_WINDOWS
    gmtime_s(&tm, &tt); // GMT (UTC)
#elif BB_PLATFORM == BB_LINUX
    gmtime_r(&tt, &tm); // GMT (UTC)
#else
#error "Unimplemented"
#endif

    std::stringstream ss {};
    ss << std::put_time(&tm, format.c_str());

    return ss.str();
}

void bb::setupDefaultLogger()
{
    // No config needed yet
}

void bb::setupFileLogger()
{
    const std::string logFileDir = "logs/";
    const std::string logFileExtension = ".bblog";

    // TODO: Probably good to put the version of the game here as well when we have access to that
    const auto now = std::chrono::system_clock::now();
    const std::string logFileName = SerializeTimePoint(now, "%dd-%mm-%Yy_%Hh-%Mm-%OSs");

    const std::string fullName = logFileDir + logFileName + logFileExtension;

    constexpr size_t maxFileSize = 1048576 * 5;
    constexpr size_t maxFiles = 3;

    auto fileLogger = spdlog::rotating_logger_mt("bblog", fullName, maxFileSize, maxFiles);

    spdlog::set_default_logger(fileLogger);
    spdlog::flush_on(spdlog::level::level_enum::trace); // Flush on everything
}