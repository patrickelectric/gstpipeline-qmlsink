#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <gst/gst.h>

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

  GstElement* pipeline = gst_parse_launch("v4l2src device=/dev/video0 ! image/jpeg, width=1920, height=1080, framerate=30/1  ! jpegparse ! jpegdec ! videoconvert ! video/x-raw,format=RGBA,sync=false! queue ! glupload ! qmlglsink name=sink", NULL);
  //GstElement* pipeline = gst_parse_launch("udpsrc port=5600 close-socket=false multicast-iface=false auto-multicast=true ! application/x-rtp, payload=96 ! rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=RGBA ! glupload ! qmlglsink name=sink sync=false", NULL);
  GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

  g_assert(pipeline && sink);

  GError* error = nullptr;
  if(!gst_init_check(&argc, &argv, &error)) {
    qCritical() << "gst_init_check() failed: " << error->message;
    g_error_free(error);
  }

  GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_add_watch(bus, [](GstBus* bus, GstMessage* message, gpointer data) -> gboolean
  {
    Q_UNUSED(bus)
    Q_UNUSED(data)
    // Debug message
    //g_print("Got %s message\n", GST_MESSAGE_TYPE_NAME(message));
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
  GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "pipeline");

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  QQuickItem* videoItem;
  QQuickWindow* rootObject;

  /* find and set the videoItem on the sink */
  rootObject = static_cast<QQuickWindow*>(engine.rootObjects().first());
  videoItem = rootObject->findChild<QQuickItem*>("videoItem");
  g_assert(videoItem);
  g_object_set(sink, "widget", videoItem, NULL);

  rootObject->scheduleRenderJob(new SetPlaying(pipeline), QQuickWindow::BeforeSynchronizingStage);

  int ret = app.exec();

  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  gst_deinit();

  return ret;
}
