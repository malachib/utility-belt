#include "service.h"
#include <entt/entt.hpp>

#include <chrono>
#include <iostream>
#include <thread>

services::service::service(std::string name, SemVer version) :
    name_(name), version_(version)
{

}

void service_runtime::sentinel_on_destroy(entt::registry&, entt::entity)
{
    registry_available = false;
}

service_runtime::service_runtime(entt::registry& registry, entt::entity entity) :
       entity_helper(registry, entity)
{
    registry.emplace<ServiceStatuses>(entity, ServiceStatuses::Unstarted);
    registry.on_destroy<float>().connect<&service_runtime::sentinel_on_destroy>(*this);
}


inline void service_runtime::progress(float percent)
{
    if(!registry_available) return;

    replace<services::progress>(percent);
}


inline void service_runtime::status(ServiceStatuses s)
{
    if(!registry_available) return;

    replace<ServiceStatuses>(s);
}

inline void service_runtime::status(std::string s)
{
    if(!registry_available) return;

    replace<services::status>(s);
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
            if(stopService)
                break;
        }

        run();

        // DEBT: To partially alleviate very nasty "no load" thread loop
        // DEBT: Do this with a semaphore instead so we can abort right away
        std::this_thread::sleep_for(100ms);
    }

    progress(0);
    // FIX: polymorphic call not happening, mysterious, since run() polymorphism does work
    cleanup();
    progress(100);

    status(ServiceStatuses::Stopped);
}

void synthetic_service_runtime::run()
{
    using namespace std::chrono_literals;

    status("count: " + std::to_string(counter++));

    //std::this_thread::sleep_for(1000ms);
}

void synthetic_service_runtime::cleanup()
{
    std::clog << "synthetic_service_runtime: " << "Cleaning up" << std::endl;
}

void synthetic_service_runtime2::run()
{
}
