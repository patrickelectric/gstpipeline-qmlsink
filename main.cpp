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
    this->pipeline_ = pipeline ? static_cast<GstElement *>(gst_object_ref(pipeline)) : NULL;
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
  GstElement * pipeline_;
};

int main(int argc, char *argv[])
{
  gst_init(&argc, &argv);

  QGuiApplication app(argc, argv);

  GstElement *pipeline = gst_parse_launch("udpsrc port=5600 close-socket=false multicast-iface=false auto-multicast=true ! application/x-rtp, payload=96 ! rtph264depay ! avdec_h264 ! videoconvert ! glupload ! qmlglsink name=sink", NULL);
  GstElement *sink = gst_bin_get_by_name(GST_BIN (pipeline), "sink");

  g_assert(pipeline && sink);

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  QQuickItem *videoItem;
  QQuickWindow *rootObject;

  /* find and set the videoItem on the sink */
  rootObject = static_cast<QQuickWindow *>(engine.rootObjects().first());
  videoItem = rootObject->findChild<QQuickItem *>("videoItem");
  g_assert(videoItem);
  g_object_set(sink, "widget", videoItem, NULL);

  rootObject->scheduleRenderJob(new SetPlaying(pipeline),
      QQuickWindow::BeforeSynchronizingStage);

  int ret = app.exec();

  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  gst_deinit();

  return ret;
}
