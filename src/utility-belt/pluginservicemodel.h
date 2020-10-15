#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

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
    Q_PROPERTY(QObject* surface READ surface CONSTANT)

signals:

private:
    QObject* const surface_;

    QString name() const { return QString::fromStdString(underlying.name()); }
    QString version() const
    {
        QString v = QString("%1.%2.%3").
                arg(underlying.version().major).
                arg(underlying.version().minor).
                arg(underlying.version().patch);

        return v;
    }

    QObject* surface() const
    {
        return surface_;
    }

public:
    ServiceObject(const service& underlying, QObject* surface) :
        underlying(underlying), surface_(surface)
    {}
};


void plugins_init(entt::registry& registry, QQmlEngine& engine);
