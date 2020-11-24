#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <gst/gst.h>

#include "videoelement.h"

class SetPlaying : public QRunnable
{
public:
  SetPlaying(GstElement* pipeline) {
    this->pipeline_ = pipeline ? static_cast<GstElement*>(gst_object_ref(pipeline)) : NULL;
  }
  ~SetPlaying() {
    if(!this->pipeline_) {
      return;
    }
    gst_object_unref(this->pipeline_);
  }

  void run() {
    if(!this->pipeline_) {
      return;
    }
    gst_element_set_state(this->pipeline_, GST_STATE_PLAYING);
  }

private:
  GstElement* pipeline_;
};

int main(int argc, char *argv[])
{
  gst_init(&argc, &argv);

  QGuiApplication app(argc, argv);

  qmlRegisterType<VideoElement>("VideoElement", 1, 0, "VideoElement");

  QStringList pipeSink{
    "qmlglsrc name=window do-timestamp=true",
    "queue",
    "gldownload ! video/x-raw,format=RGBA ! videoconvert ! videoflip method=vertical-flip ! queue ! fpsdisplaysink name=sink sync=false async=false",
  };
  GstElement* pipelineSink = gst_parse_launch(pipeSink.join(" ! ").toStdString().c_str(), NULL);
  GstElement* window = gst_bin_get_by_name(GST_BIN(pipelineSink), "window");

  GError* error = nullptr;
  if(!gst_init_check(&argc, &argv, &error)) {
    qCritical() << "gst_init_check() failed: " << error->message;
    g_error_free(error);
  }

  GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipelineSink));
  gst_bus_add_watch(bus, [](GstBus* bus, GstMessage* message, gpointer data) -> gboolean
  {
    Q_UNUSED(bus)
    Q_UNUSED(data)
    GError* err;
    gchar* debug;
    switch(GST_MESSAGE_TYPE(message)) {
      case GST_MESSAGE_ERROR:
      {
          gst_message_parse_error(message, &err, &debug);
          g_print("Err: %s\n", err->message);
          g_print("Deb: %s\n", debug);
          g_error_free(err);
          g_free(debug);
          break;
      }
      case GST_MESSAGE_WARNING:
      {
          gst_message_parse_warning(message, &err, &debug);
          g_print("War: %s\n", err->message);
          g_print("Deb: %s\n", debug);
          g_error_free(err);
          g_free(debug);
          break;
      }
      case GST_MESSAGE_INFO:
      {
          gst_message_parse_info(message, &err, &debug);
          g_print("Inf: %s\n", err->message);
          g_print("Deb: %s\n", debug);
          g_error_free(err);
          g_free(debug);
          break;
      }
      default:
        /* unhandled message */
        break;
    }
    /* we want to be notified again the next time there is a message
     * on the bus, so returning TRUE (FALSE means we want to stop watching
     * for messages on the bus and our callback should not be called again)
     */
    return true;
  }, nullptr);
  gst_object_unref(bus);
  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipelineSink), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  QQuickWindow* rootObject;

  // find and set the videoItem on the sink
  rootObject = static_cast<QQuickWindow*>(engine.rootObjects().first());
  g_object_set(window, "window", rootObject, NULL);
  g_object_set(window, "use-default-fbo", true, NULL);

  rootObject->scheduleRenderJob(new SetPlaying(pipelineSink), QQuickWindow::BeforeSynchronizingStage);

  int ret = app.exec();

  gst_element_set_state(pipelineSink, GST_STATE_NULL);
  gst_object_unref(pipelineSink);

  gst_deinit();

  return ret;
}
