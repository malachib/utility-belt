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

class ServiceObject : public QObject
{
    const service& underlying;

    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)

signals:

private:
    QString name() const { return QString::fromStdString(underlying.name()); }

public:
    ServiceObject(const service& underlying) :
        underlying(underlying) {}
};


void plugins_init(entt::registry& registry);
