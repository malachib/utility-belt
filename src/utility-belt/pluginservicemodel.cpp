#include "pluginservicemodel.h"

#include <QQmlComponent>
#include <entt/entt.hpp>

PluginServiceModel::PluginServiceModel()
{

}


void ServiceObject::statusChanged(entt::registry& registry, entt::entity entity)
{

}

void ServiceObject::connectup()
{
    eh.registry.on_update<ServiceStatuses>().connect<&ServiceObject::statusChanged>(*this);
}


void plugins_init(entt::registry& registry, QQmlEngine& engine)
{
    auto component = new QQmlComponent(&engine,
                            //QUrl::fromLocalFile("pocket.qml")
                            QUrl("qrc:/tool.qml")
                            );
    auto component2 = new QQmlComponent(&engine,
                            //QUrl::fromLocalFile("pocket.qml")
                            QUrl("qrc:/tool.qml")
                            );

    // TODO: Set user data on component to track entity

    auto entity = registry.create();

    entity_helper eh(registry, entity);

    auto sr = new synthetic_service_runtime(eh);

    registry.emplace<service>(entity, "test", SemVer{1, 1, 0, ""});
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component);
    registry.emplace<std::unique_ptr<service_runtime>>(entity, sr);

    sr->start();

    entity = registry.create();

    entity_helper eh2(registry, entity);

    sr = new synthetic_service_runtime(eh2);

    registry.emplace<service>(entity, "test2", SemVer{0, 1, 0, ""});
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component2);
    registry.emplace<std::unique_ptr<service_runtime>>(entity, sr);

    sr->start();
}
