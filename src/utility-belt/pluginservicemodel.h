#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QThread>

#include <entt/entity/registry.hpp>
#include <entt/process/scheduler.hpp>
#include "service.h"

// Looks like somehow Qt/QML may be grabbing ownership of QQmlComponent and
// deleting it itself
#define FEATURE_ENABLE_UNIQUE_PTR_COMPONENT 0

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
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QQmlComponent* component READ component CONSTANT)
    Q_PROPERTY(ServiceStatuses status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString status2 READ status2 NOTIFY status2Changed)

public:

    typedef services::ServiceStatuses ServiceStatuses;

signals:
    void statusChanged(ServiceStatuses);
    void status2Changed(QString);

private:
    const service& underlying;
    entity_helper eh;
    QQmlComponent* const surface_;
    const services::status& status2_;

    ServiceStatuses status() const
    {
        // NOTE: Not ready for prime time
        //auto s = eh.try_get<ServiceStatuses>();
        auto s = eh.registry.try_get<ServiceStatuses>(eh.entity);
        if(s == nullptr)
            return ServiceStatuses::Unstarted;
        else
            return *s;
    }

    QString status2() const
    {
        return QString::fromStdString(status2_.description);
    }

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

    void enttStatusChanged(entt::registry& registry, entt::entity entity);
    static void enttStatusChanged2(entt::registry& registry, entt::entity entity);

    void connectup();

public:
    ServiceObject(const service& underlying, QQmlComponent* component, entity_helper eh) :
        underlying(underlying),
        eh(eh),
        surface_(component),
        status2_(eh.registry.get<services::status>(eh.entity))
    {
        connectup();
    }
};


class EnttQtScheduler : public QObject
{
    Q_OBJECT

    static constexpr int ms_interval = 100;

    QThread workerThread;

public:
    typedef entt::scheduler<std::uint32_t> scheduler_type;

private:
    scheduler_type& scheduler;

private slots:
    void started();

public:
    EnttQtScheduler(scheduler_type& scheduler, bool autoStart = true, QObject* parent = nullptr);

    void timerEvent(QTimerEvent* event) override;
    void start();
};


void plugins_init(entt::registry& registry, QQmlEngine& engine);
void plugins_deinit(entt::registry& registry);
