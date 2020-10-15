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
    Q_PROPERTY(QString version READ version CONSTANT)

signals:

private:
    QString name() const { return QString::fromStdString(underlying.name()); }
    QString version() const
    {
        QString v = QString("%1.%2.%3").
                arg(underlying.version().major).
                arg(underlying.version().minor).
                arg(underlying.version().patch);

        return v;
    }

public:
    ServiceObject(const service& underlying) :
        underlying(underlying) {}
};


void plugins_init(entt::registry& registry);
