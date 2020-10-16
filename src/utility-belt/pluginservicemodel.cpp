#include "pluginservicemodel.h"

#include <QQmlComponent>
#include <entt/entt.hpp>

PluginServiceModel::PluginServiceModel()
{

}


void ServiceObject::statusChanged(entt::registry& registry, entt::entity entity)
{

}

void ServiceObject::connectup()
{
    eh.registry.on_update<ServiceStatuses>().connect<&ServiceObject::statusChanged>(*this);
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

    auto entity = registry.create();

    entity_helper eh(registry, entity);

    auto sr = new synthetic_service_runtime(eh);

    registry.emplace<service>(entity, "test", SemVer{1, 1, 0, ""});
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component);
    registry.emplace<std::unique_ptr<service_runtime>>(entity, sr);

    sr->start();

    entity = registry.create();

    entity_helper eh2(registry, entity);

    sr = new synthetic_service_runtime(eh2);

    registry.emplace<service>(entity, "test2", SemVer{0, 1, 0, ""});
    registry.emplace<std::unique_ptr<QQmlComponent>>(entity, component2);
    registry.emplace<std::unique_ptr<service_runtime>>(entity, sr);

    sr->start();

    entity = registry.create();
    entity_helper eh3(registry, entity);

    // FIX: Obviously these are memory leakers big time, just doing this to test thread/timer
    // behavior
    auto scheduler = new EnttQtScheduler::scheduler_type();
    // Does not work, CRTP trickery gets mad at MI.  Just as well, I'm thinking
    // we need to make the scheduler/task portion a "has a" vs "is a" anyway
    //scheduler->attach<synthetic_service_runtime2>(eh3);
    new EnttQtScheduler(*scheduler, true);
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
