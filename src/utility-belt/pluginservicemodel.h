#pragma once

#include <QAbstractListModel>
#include <entt/entity/registry.hpp>
#include "service.h"

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
