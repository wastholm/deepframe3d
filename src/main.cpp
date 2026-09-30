#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QLoggingCategory>

#include "managedimageprovider.h"
#include "backend.h"

int main(int argc, char *argv[]) {
    // Filter out noisy Qt image warnings
    QLoggingCategory::setFilterRules("qt.gui.imageio.jpeg.debug=false");
    
    QGuiApplication app(argc, argv);
    app.setApplicationName("Deepframe3D");
    app.setOrganizationName("Deepframe3D");
    app.setApplicationVersion("1.0.0");

    QQmlApplicationEngine engine;
    
    // Create image provider on heap, parented to app so it outlives engine
    ManagedImageProvider *imageProvider = new ManagedImageProvider();
    imageProvider->setParent(&app);
    
    // Create backend on heap, parented to app
    Backend *backend = new Backend(imageProvider, &app);

    // Expose backend to QML
    engine.rootContext()->setContextProperty("backend", backend);
    
    // Register image provider
    engine.addImageProvider("mpo", imageProvider);

    engine.load(QUrl("qrc:/main.qml"));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    // Process command line arguments
    QStringList args = app.arguments();
    if (args.size() > 1) {
        backend->loadFiles(args.mid(1));
    }

    return app.exec();
}

#include "main.moc"

