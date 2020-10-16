#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <entt/entt.hpp>

#include "pluginservicemodel.h"

int entt_test(entt::registry& registry);

static entt::registry registry;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    plugins_init(registry, engine);
    entt_test(registry);

#if FEATURE_ENABLE_UNIQUE_PTR_COMPONENT
    auto services = registry.view<service, std::unique_ptr<QQmlComponent>>();
#else
    auto services = registry.view<service, QQmlComponent*>();
#endif

    QList<QObject*> serviceObjects;

    for(const auto& entity : services)
    {
        entity_helper eh(registry, entity);
        const auto& s = services.get<service>(entity);
        ServiceObject* so;
#if FEATURE_ENABLE_UNIQUE_PTR_COMPONENT
        auto& o = services.get<std::unique_ptr<QQmlComponent>>(entity);
        serviceObjects.append(so = new ServiceObject(s, o.get(), eh));
#else
        auto c = services.get<QQmlComponent*>(entity);
        serviceObjects.append(so = new ServiceObject(s, c, eh));
#endif
        // DEBT: Confusing place to keep this, we want this more in the plugin_init area
        registry.emplace<ServiceObject*>(entity, so);
    }

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    // Trickery from
    // http://www.bim-times.com/qt/Qt-5.11.1/qtquick/qtquick-modelviewsdata-cppmodels.html
    engine.rootContext()->setContextProperty("PluginServicesModel",
                                             QVariant::fromValue(serviceObjects));
    engine.load(url);

    int retVal = app.exec();

    plugins_deinit(registry);

    return retVal;
}
