#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QQmlComponent>

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
    Q_PROPERTY(QQmlComponent* component READ component CONSTANT)

signals:

private:
    QQmlComponent* const surface_;

    QString name() const { return QString::fromStdString(underlying.name()); }
    QString version() const
    {
        QString v = QString("%1.%2.%3").
                arg(underlying.version().major).
                arg(underlying.version().minor).
                arg(underlying.version().patch);

        return v;
    }

    QQmlComponent* component() const
    {
        return surface_;
    }

public:
    ServiceObject(const service& underlying, QQmlComponent* component) :
        underlying(underlying), surface_(component)
    {}
};


void plugins_init(entt::registry& registry, QQmlEngine& engine);
