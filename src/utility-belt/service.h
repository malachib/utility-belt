#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <thread>
#include <entt/entity/fwd.hpp>

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


class entity_helper
{
public:
    const entt::entity entity;
    entt::registry& registry;

public:
    entity_helper(entt::registry& registry, entt::entity entity) :
        registry(registry),
        entity(entity)
    {}
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


class service_runtime : protected entity_helper
{
    std::thread worker;

protected:
    virtual void run() = 0;

    void status(ServiceStatuses s);

public:
    service_runtime(entt::registry& registry, entt::entity entity);
    virtual ~service_runtime()
    {
        // DEBT: Presumes a whole lot, that thread will finish up on its own
        worker.detach();
    }

    void start();
    void stop();
};

class synthetic_service_runtime : public service_runtime
{
protected:
    void run() override;

public:
    synthetic_service_runtime(entity_helper& eh) :
        service_runtime(eh.registry, eh.entity) {}
};


#endif // SERVICE_H
