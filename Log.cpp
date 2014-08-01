#include "Log.h"







uint Logger::severity() const
{
    return severity_;
}

void Logger::setSeverity(const uint& severity)
{
    severity_ = severity;
}
