#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QTimer>
#include <gst/gst.h>

#include "helper.h"
#include "videoelement.h"

int main(int argc, char *argv[])
{
  qputenv("GST_DEBUG_DUMP_DOT_DIR", Helper::self()->temporaryPath().toLatin1());

  gst_init(&argc, &argv);

  QGuiApplication app(argc, argv);

  qmlRegisterType<VideoElement>("VideoElement", 1, 0, "VideoElement");

  GError* error = nullptr;
  if(!gst_init_check(&argc, &argv, &error)) {
    qCritical() << "gst_init_check() failed: " << error->message;
    g_error_free(error);
  }

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  QTimer::singleShot(2000, &app, [&]() {
    QObject *rootObject = engine.rootObjects().constFirst();
    VideoElement *videoElement =  static_cast<VideoElement*>(rootObject->findChild<QObject*>("videoElement"));
    if (videoElement) {
      videoElement->setDescription("videotestsrc pattern=snow ! video/x-raw,width=640,height=480");
    } else {
      qDebug() << "Failed to find videoElement";
    }
  });

  int ret = app.exec();
  return ret;
}
