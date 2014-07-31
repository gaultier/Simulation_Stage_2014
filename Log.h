#ifndef LOG_H
#define LOG_H

#include <sstream>
#include <iostream>
#include <ostream>
#include <fstream>
#include <memory>
#include <chrono>

using namespace std;

enum Severity
{
    all,
    trace,
    debug,
    info,
    error,
    none
};

class Logger
{
public:
    Logger(bool logToConsole, Severity const & severity):
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

    Logger(string const & logFileName, string const & errorFileName, bool logToConsole, Severity const & severity):
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

        if(severity >= severity_)
        {
            log(stream, "trace", clog, logFile_);
        }

        clear();
    }

    void debug(ostream & stream)
    {
        Severity severity = Severity::debug;

        if(severity >= severity_)
        {
            log(stream, "debug", clog, logFile_);
        }

        clear();
    }

    void info(ostream & stream)
    {
        Severity severity = Severity::info;

        if(severity >= severity_)
        {
            log(stream, "info", cout, logFile_);
        }

        clear();
    }

    void error(ostream & stream)
    {
        Severity severity = Severity::error;

        if(severity >= severity_)
        {
            log(stream, "error", cerr, errorFile_);
        }

        clear();
    }

    void setLogFile(string logFileName)
    {
        logFileName_ = logFileName;
        logToFile_ = true;
        logFile_.open(logFileName);
    }

    void clear()
    {
        buffer_.str("");
    }

    Severity severity() const
    {
        return severity_;
    }

    void setSeverity(const Severity& severity)
    {
        severity_ = severity;
    }

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

    Severity severity_;
};

extern unique_ptr<Logger> logger;

#endif // LOG_H
