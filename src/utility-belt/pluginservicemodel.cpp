#include "pluginservicemodel.h"

#include <QQmlComponent>

PluginServiceModel::PluginServiceModel()
{

}


void plugins_init(entt::registry& registry, QQmlEngine& engine)
{
    QQmlComponent component(&engine,
                            //QUrl::fromLocalFile("pocket.qml")
                            QUrl("qrc:/pocket.qml")
                            );

    QObject* o = component.create();

    auto entity = registry.create();

    registry.emplace<service>(entity, "test", SemVer{1, 1, 0, ""});
    registry.emplace<std::unique_ptr<QObject>>(entity, o);

    o = component.create();
    entity = registry.create();

    registry.emplace<service>(entity, "test2", SemVer{0, 1, 0, ""});
    registry.emplace<std::unique_ptr<QObject>>(entity, o);
}
