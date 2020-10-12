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
    std::string name;
    SemVer version;

public:
    service();
};

#endif // SERVICE_H
