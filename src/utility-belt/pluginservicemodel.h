#pragma once

#include <QAbstractListModel>
#include <entt/entity/registry.hpp>

class ServiceModel : public QAbstractItemModel
{
    Q_OBJECT
public:
};

class PluginServiceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    PluginServiceModel();
};

void plugins_init(entt::registry& registry);
