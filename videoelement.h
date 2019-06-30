#pragma once

#include <QObject>
#include <QQuickItem>
#include <QRunnable>

#include <gst/gst.h>
#include <gst/gstplugin.h>
//#include <gstqtsink.h>

class GstPipelinePlayer : public QRunnable
{
public:
  GstPipelinePlayer(GstElement* pipeline) {
    _pipeline = pipeline ? static_cast<GstElement*>(gst_object_ref(pipeline)) : nullptr;
  }
  ~GstPipelinePlayer() {
    if(!_pipeline) {
      return;
    }
    gst_object_unref(_pipeline);
  }

  void run() {
    if(!_pipeline) {
      return;
    }
    gst_element_set_state(_pipeline, GST_STATE_PLAYING);
  }

private:
  GstElement* _pipeline;
};

/**
 * @brief VideoElement widget
 *
 */
class VideoElement : public QQuickItem
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new VideoElement object
     *
     * @param parent
     */
    VideoElement();

    /**
     * @brief Return gstreamer pipeline as string
     *
     * @return QString
     */
    QString pipeline() {return _pipelineDescription;}

    /**
     * @brief Set pipeline description
     *
     * @param pipelineDescription
     */
    void setPipeline(QString pipelineDescription) {
        if(_pipelineDescription != pipelineDescription) {
            _pipelineDescription = pipelineDescription;
            emit pipelineDescriptionChanged();
        }
    }
    Q_PROPERTY(QString pipeline READ pipeline WRITE setPipeline NOTIFY pipelineDescriptionChanged)

    void createVideoItem();

signals:
    void pipelineDescriptionChanged();

private:
    QString _pipelineDescription;

    /**
     * @brief Update render job scheduler
     *
     */
    void scheduleRenderJob(QQuickWindow* window);

    QSharedPointer<QQuickItem> _controlPanel;
};
