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

    plugins_init(registry);
    entt_test(registry);

    auto services = registry.view<service>();

    QList<QObject*> serviceObjects;

    for(const auto& entity : services)
    {
        auto& s = services.get<service>(entity);
        serviceObjects.append(new ServiceObject(s));
    }

    QQmlApplicationEngine engine;
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

    return app.exec();
}
