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
    entity_helper eh;

    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QQmlComponent* component READ component CONSTANT)
    //Q_PROPERTY(QString status READ status NOTIFY statusChanged)

signals:

private:
    QQmlComponent* const surface_;

    /*
    QString status() const
    {
        eh.registry.try_get<ServiceStatuses>(eh.entity);
    }*/

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

    void statusChanged(entt::registry& registry, entt::entity entity);

    void connectup();

public:
    ServiceObject(const service& underlying, QQmlComponent* component, entity_helper eh) :
        underlying(underlying),
        eh(eh),
        surface_(component)
    {
        connectup();
    }
};


void plugins_init(entt::registry& registry, QQmlEngine& engine);
