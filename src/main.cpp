#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
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

  int ret = app.exec();
  return ret;
}
