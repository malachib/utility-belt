#ifndef SERVICE_H
#define SERVICE_H

#include <string>

struct SemVer
{
    const unsigned major;
    const unsigned minor;
    const unsigned patch;

    const std::string prerelease;
};

class service
{
    std::string name_;
    SemVer version_;

public:
    service(std::string name, SemVer version);
    service(const service& copy_from) :
        name_(copy_from.name_),
        version_(copy_from.version_)
    {

    }

    service& operator=(service&& move_from)
    {
        name_ = move_from.name_;
        new (&version_) SemVer(move_from.version_);
        return *this;
    }

    const std::string& name() const { return name_; }
    const SemVer& version() const { return version_; }
};


enum class ServiceStatuses
{
    Unstarted,
    Starting,
    Started,
    Running,
    Pausing,
    Paused,
    Resuming,
    Stopping,
    Stopped,
    Error       // Stopped, with an error
};

#endif // SERVICE_H
