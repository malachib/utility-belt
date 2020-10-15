#include "service.h"
#include <entt/entt.hpp>

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


void service_runtime::start()
{
    try
    {
        status(ServiceStatuses::Starting);

        std::thread worker(&service_runtime::run, this);

        status(ServiceStatuses::Started);

        this->worker.swap(worker);
    }
    catch(const std::system_error& e)
    {
        status(ServiceStatuses::Error);
    }
}

void synthetic_service_runtime::run()
{
    status(ServiceStatuses::Running);

    status(ServiceStatuses::Stopped);
}
