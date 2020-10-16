#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <thread>
#include <mutex>
#include <utility>

#include <entt/entity/fwd.hpp>
#include <entt/process/process.hpp>
#include <entt/entity/registry.hpp>

struct SemVer
{
    const unsigned major;
    const unsigned minor;
    const unsigned patch;

    const std::string prerelease;
};

namespace services {

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

}

class entity_helper
{
public:
    entt::registry& registry;
    const entt::entity entity;

public:
    entity_helper(entt::registry& registry, entt::entity entity) :
        registry(registry),
        entity(entity)
    {}

    template <class T, typename ...TArgs>
    void replace(TArgs&&... args)
    {
        registry.replace<T>(entity, std::forward<TArgs>(args)...);
    }

    // FIX: Somehow, this doesn't work right, probably because we're not doing
    // parameter pack
    template <class TComponent>
    TComponent* try_get() const
    {
        return registry.try_get<TComponent>(entity);
    }
};

namespace services {

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
    Halted,     // Forcefully stopped
    Stopped,
    Error       // Stopped, with an error
};

struct progress
{
    typedef float numeric_type;

    numeric_type percentage;
};


struct status
{
    // other human readable status for what service is actually up to while it's running
    std::string description;
};


}



class service_runtime : protected entity_helper
{
    // DEBT: Kludgey, but on the right track.  The destruct process attempts to launch signals
    // out to the registry after the registry has been destroyed.  This sentinel signals that
    // no further registry interaction should take place (shutting down).
    bool registry_available = true;
    void sentinel_on_destroy(entt::registry&, entt::entity entity);

public:
    typedef services::ServiceStatuses ServiceStatuses;

protected:
    virtual void run() = 0;

    /// After run receives the stop command, cleanup is (allegedly) run
    /// (not working so well right now)
    virtual void cleanup() {};

    void status(ServiceStatuses s);
    void status(std::string s);
    void progress(float percent);

public:
    service_runtime(entt::registry& registry, entt::entity entity);
    virtual ~service_runtime() {}
};

// NOTE: Unsure if MI is gonna play nice with CRTP here
class entt_service_runtime :
        public service_runtime,
        public entt::process<entt_service_runtime, std::uint32_t>
{
protected:
    entt_service_runtime(entity_helper& eh) :
        service_runtime(eh.registry, eh.entity)
    {}

public:
    using delta_type = std::uint32_t;

    void init()
    {
        status(ServiceStatuses::Started);
    }

    void update(delta_type delta, void*)
    {
        run();
    }
};


class threaded_service_runtime : public service_runtime
{
    std::thread worker;
    bool stopService = false;
    std::mutex stopServiceMutex;

    // wrapper for run which sets started/stopped status
    void _run();

public:
    threaded_service_runtime(entt::registry& registry, entt::entity entity) :
        service_runtime(registry, entity)
    {}

    virtual ~threaded_service_runtime()
    {
        stop();
        // DEBT: Presumes a whole lot, that thread will finish up on its own.  Neither detach nor
        // join are ideal here.  join is useful for debugging but could easily hang on exit,
        // detach is useful for fast-exiting but it's *too* fast and cleanups don't occur
        //worker.detach();
        worker.join();
    }

    void start();
    void stop();
};

class synthetic_service_runtime : public threaded_service_runtime
{
    int counter = 0;

protected:
    void run() override;
    void cleanup() override;

public:
    synthetic_service_runtime(entity_helper& eh) :
        threaded_service_runtime(eh.registry, eh.entity) {}
};

class synthetic_service_runtime2 : public entt_service_runtime
{
protected:
    void run() override;

public:
    synthetic_service_runtime2(entity_helper& eh) :
        entt_service_runtime(eh) {}
};


#endif // SERVICE_H
