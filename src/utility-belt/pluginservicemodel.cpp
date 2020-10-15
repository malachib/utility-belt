#include "pluginservicemodel.h"

PluginServiceModel::PluginServiceModel()
{

}


void plugins_init(entt::registry& registry)
{
    auto entity = registry.create();

    registry.emplace<service>(entity, "test", SemVer{1, 1, 0, ""});

    entity = registry.create();

    registry.emplace<service>(entity, "test2", SemVer{0, 1, 0, ""});
}
