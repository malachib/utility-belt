#include "pluginservicemodel.h"

#include <QQmlComponent>
#include <entt/entt.hpp>

PluginServiceModel::PluginServiceModel()
{

}


void ServiceObject::enttStatusChanged(entt::registry& registry, entt::entity entity)
{
    // DEBT: Easy to read, but clumsy.  An external dispatcher looking up service object
    // by entity itself is probably the better choice, and then calling enttStatusChanged from there
    if(eh.entity != entity) return;

    emit statusChanged(status());
}

void ServiceObject::enttStatusChanged2(entt::registry& registry, entt::entity entity)
{
    if(eh.entity != entity) return;

    emit status2Changed(status2());
}

void ServiceObject::connectup()
{
    eh.registry.on_update<ServiceStatuses>().connect<&ServiceObject::enttStatusChanged>(*this);
    eh.registry.on_update<services::status>().connect<&ServiceObject::enttStatusChanged2>(*this);
}

static entt::entity sentinel;

void plugins_deinit(entt::registry& registry)
{
    registry.destroy(sentinel);
}

void plugins_init(entt::registry& registry, QQmlEngine& engine)
{
    auto component = new QQmlComponent(&engine,
                            //QUrl::fromLocalFile("pocket.qml")
                            QUrl("qrc:/tool.qml")
                            );
    auto component2 = new QQmlComponent(&engine,
                            //QUrl::fromLocalFile("pocket.qml")
                            QUrl("qrc:/tool.qml")
                            );

    // TODO: Set user data on component to track entity

    sentinel = registry.create();

    // DEBT: sentinel value, use a specific struct instead
    registry.emplace<float>(sentinel);

    auto entity = registry.create();

    entity_helper eh(registry, entity);

    auto sr = new synthetic_service_runtime(eh);

    registry.emplace<service>(entity, "test", SemVer{1, 1, 0, ""});
#if FEATURE_ENABLE_UNIQUE_PTR_COMPONENT
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component);
#else
    registry.emplace<QQmlComponent*>(entity, component);
#endif
    registry.emplace<std::unique_ptr<service_runtime>>(entity, sr);
    registry.emplace<services::progress>(entity);
    registry.emplace<services::status>(entity, "");

    sr->start();

    entity = registry.create();

    entity_helper eh2(registry, entity);

    sr = new synthetic_service_runtime(eh2);

    registry.emplace<service>(entity, "test2", SemVer{0, 1, 0, ""});
#if FEATURE_ENABLE_UNIQUE_PTR_COMPONENT
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component2);
#else
    registry.emplace<QQmlComponent*>(entity, component2);
#endif
    registry.emplace<std::unique_ptr<service_runtime>>(entity, sr);
    registry.emplace<services::progress>(entity);
    registry.emplace<services::status>(entity, "");

    sr->start();

#if UNUSED
    entity = registry.create();
    entity_helper eh3(registry, entity);

    // FIX: Obviously these are memory leakers big time, just doing this to test thread/timer
    // behavior
    auto scheduler = new EnttQtScheduler::scheduler_type();
    // Does not work, CRTP trickery gets mad at MI.  Just as well, I'm thinking
    // we need to make the scheduler/task portion a "has a" vs "is a" anyway
    //scheduler->attach<synthetic_service_runtime2>(eh3);
    new EnttQtScheduler(*scheduler, true);
#endif
}


EnttQtScheduler::EnttQtScheduler(scheduler_type& scheduler, bool autoStart, QObject* parent) :
    QObject(parent),
    scheduler(scheduler)
{
    if(autoStart) start();
}

void EnttQtScheduler::timerEvent(QTimerEvent *event)
{
    scheduler.update(ms_interval);
}


void EnttQtScheduler::start()
{
    moveToThread(&workerThread);

    // guidance from https://stackoverflow.com/questions/22399868/qobjectstarttimer-timers-can-only-be-used-with-threads-started-with-qthread
    // not 100% sure we have to do this with QObject-based timers, but 90% sure we do
    connect(&workerThread, SIGNAL(started()), this, SLOT(started()));

    workerThread.start();
}

void EnttQtScheduler::started()
{
    startTimer(ms_interval);
}
