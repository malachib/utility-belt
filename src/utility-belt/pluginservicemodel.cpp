#include "pluginservicemodel.h"

#include <QQmlComponent>

PluginServiceModel::PluginServiceModel()
{

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

    registry.emplace<service>(entity, "test", SemVer{1, 1, 0, ""});
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component);
    //registry.emplace<std::unique_ptr<service_runtime>(new synthetic_service_runtime())

    entity = registry.create();

    registry.emplace<service>(entity, "test2", SemVer{0, 1, 0, ""});
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component2);
}
