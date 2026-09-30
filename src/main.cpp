#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

#include "imageprovider.h"
#include "backend.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("Deepframe3D");
    app.setOrganizationName("Deepframe3D");
    app.setApplicationVersion("1.0.0");

    // Create image provider
    FrameImageProvider imageProvider;
    
    // Create backend
    Backend backend(&imageProvider);

    QQmlApplicationEngine engine;
    
    // Register image provider
    engine.addImageProvider("mpo", &imageProvider);
    
    // Expose backend to QML
    engine.rootContext()->setContextProperty("backend", &backend);

    engine.load(QUrl("qrc:/main.qml"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    // Process command line arguments
    QStringList args = app.arguments();
    if (args.size() > 1) {
        backend.loadFiles(args.mid(1));
    }

    return app.exec();
}

#include "main.moc"

