#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <gst/gst.h>

#include "videoelement.h"

int main(int argc, char *argv[])
{
  gst_init(&argc, &argv);

  QGuiApplication app(argc, argv);

  qmlRegisterType<VideoElement>("VideoElement", 1, 0, "VideoElement");

  GError* error = nullptr;
  if(!gst_init_check(&argc, &argv, &error)) {
    qCritical() << "gst_init_check() failed: " << error->message;
    g_error_free(error);
  }

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  int ret = app.exec();

  gst_deinit();

  return ret;
}
