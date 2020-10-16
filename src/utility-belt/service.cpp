#include "service.h"
#include <entt/entt.hpp>

#include <chrono>
#include <thread>

service::service(std::string name, SemVer version) :
    name_(name), version_(version)
{

}

service_runtime::service_runtime(entt::registry& registry, entt::entity entity) :
       entity_helper(registry, entity)
{
    registry.emplace<ServiceStatuses>(entity, ServiceStatuses::Unstarted);
}

inline void service_runtime::status(ServiceStatuses s)
{
    registry.replace<ServiceStatuses>(entity, s);
}

inline void service_runtime::status(std::string s)
{
    registry.replace<services::status>(entity, s);
}



void threaded_service_runtime::start()
{
    try
    {
        status(ServiceStatuses::Starting);

        std::thread worker(&threaded_service_runtime::_run, this);

        status(ServiceStatuses::Started);

        this->worker.swap(worker);
    }
    catch(const std::system_error& e)
    {
        status(ServiceStatuses::Error);
    }
}

// guidance from
// https://stackoverflow.com/questions/42226269/how-to-say-to-stdthread-to-stop
// DEBT: Optionally wait on status update and if it doesn't occur within a certain amount of time, return
// a succeed/fail
void threaded_service_runtime::stop()
{
    // FIX: Sometimes crashes here because this is called after registry itself is removed from memory
    status(ServiceStatuses::Stopping);

    std::unique_lock<std::mutex> lock(stopServiceMutex);
    stopService = true;

    // DEBT: Do a SIGABRT after a certain timeout
}

void threaded_service_runtime::_run()
{
    using namespace std::chrono_literals;

    status(ServiceStatuses::Running);

    for(;;)
    {
        {
            std::unique_lock<std::mutex> lock(stopServiceMutex);
            if(stopService) break;
        }

        run();

        // DEBT: To partially alleviate very nasty "no load" thread loop
        // DEBT: Do this with a semaphore instead so we can abort right away
        std::this_thread::sleep_for(100ms);
    }

    status(ServiceStatuses::Stopped);
}

void synthetic_service_runtime::run()
{
    using namespace std::chrono_literals;

    status("count: " + std::to_string(counter++));

    //std::this_thread::sleep_for(1000ms);
}


void synthetic_service_runtime2::run()
{
}
