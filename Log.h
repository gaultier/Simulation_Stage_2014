#ifndef LOG_H
#define LOG_H

#include <sstream>
#include <iostream>
#include <ostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <cstdint>

using namespace std;

enum Severity
{
    trace = 1,
    debug = 2,
    info = 4,
    error = 8
};

/*
namespace Severity
{
    int16_t trace;
    int16_t debug;
    int16_t info;
    int16_t error;

}
*/

class Logger
{
public:
    Logger(bool logToConsole, uint severity):
        logFileName_ {""},
        errorFileName_ {""},
        logToFile_ {false},
        logToConsole_ {logToConsole},
        startTime_ {std::chrono::high_resolution_clock::now()},
        lastLogTime_ {startTime_},
        logCount_ {0},
        severity_ {severity}
    {
    }

    Logger(string const & logFileName, string const & errorFileName, bool logToConsole, uint severity):
        logFileName_ {logFileName},
        errorFileName_ {errorFileName},
        logFile_ {logFileName},
        errorFile_ {errorFileName},
        logToFile_ {true},
        logToConsole_ {logToConsole},
        startTime_ {std::chrono::high_resolution_clock::now()},
        lastLogTime_ {startTime_},
        logCount_ {0},
        severity_ {severity}
    {
    }

    ~Logger()
    {
        if(logFile_)
        {
            logFile_.close();
        }
        if(errorFile_)
        {
            errorFile_.close();
        }
    }

    stringstream & get ()
    {
        return buffer_;
    }

    stringstream & operator() ()
    {
        return get();
    }

    void log (ostream & message, string const & severityLevel, ostream & consoleOutLog, ofstream & fileOutLog)
    {
        logCount_++;

        stringstream res;
        auto now = std::chrono::high_resolution_clock::now();


        res << "<" << severityLevel << "> "
            << "(" << logCount_ << ") "
            << "[" << std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_).count() << " micros] "
            << message.rdbuf()
            << " (" << std::chrono::duration_cast<std::chrono::microseconds>(now - lastLogTime_).count()  << " micros) "
            << endl;

        lastLogTime_ = now;

        if(logToConsole_)
        {
            consoleOutLog << res.str();
        }
        if(logToFile_)
        {
            fileOutLog << res.str();
        }
    }

    void trace(ostream & stream)
    {
        Severity severity = Severity::trace;

        if(severity_ & severity)
        {
            log(stream, "trace", clog, logFile_);
        }

        clear();
    }

    void debug(ostream & stream)
    {
        Severity severity = Severity::debug;

        if(severity_ & severity)
        {
            log(stream, "debug", clog, logFile_);
        }

        clear();
    }

    void info(ostream & stream)
    {
        Severity severity = Severity::info;

        if(severity_ & severity)
        {
            log(stream, "info", cout, logFile_);
        }

        clear();
    }

    void error(ostream & stream)
    {
        Severity severity = Severity::error;

        if(severity_ & severity)
        {
            log(stream, "error", cerr, errorFile_);
        }

        clear();
    }

    void setLogFile(string const & logFileName)
    {
        logFileName_ = logFileName;
        logToFile_ = true;
        logFile_.open(logFileName);
    }

    void clear()
    {
        buffer_.str("");
    }

    uint severity() const;
    void setSeverity(const uint& severity);

private:
    stringstream buffer_;
    string logFileName_;
    string errorFileName_;
    ofstream logFile_;
    ofstream errorFile_;
    bool logToFile_;
    bool logToConsole_;
    chrono::high_resolution_clock::time_point startTime_;
    chrono::high_resolution_clock::time_point lastLogTime_;

    /**
     * @brief The number of logs since the start of the program
     */
    long long logCount_;

     uint severity_;
};

extern unique_ptr<Logger> logger;

#endif // LOG_H
