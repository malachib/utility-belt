#ifndef SERVICE_H
#define SERVICE_H

#include <string>

struct SemVer
{
    unsigned major;
    unsigned minor;
    unsigned patch;

    std::string prerelease;
};

class service
{
    std::string name_;
    SemVer version_;

public:
    service(std::string name, SemVer version);

    std::string name() const { return name_; }
    const SemVer& version() const { return version_; }
};

#endif // SERVICE_H
