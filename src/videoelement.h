#pragma once

#include <QObject>
#include <QQuickItem>
//#include <QRunnable>

#include <gst/gst.h>
#include <gst/gstplugin.h>

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
     * @brief Get sink from pipeline and set it in our qml item
     *
     * @param pipeline
     */
    void setSink(GstElement* pipeline);

    /**
     * @brief Return gstreamer pipeline as string
     *
     * @return QString
     */
    QString description() {return m_pipelineDescription;}

    /**
     * @brief Set pipeline description
     *
     * @param pipelineDescription
     */
    void setDescription(QString pipelineDescription) {
        if(m_pipelineDescription != pipelineDescription) {
            m_pipelineDescription = pipelineDescription;
            Q_EMIT descriptionChanged();
        }
    }
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)

Q_SIGNALS:
    void descriptionChanged();

private:
    /**
     * @brief Update render job scheduler
     *
     */
    void scheduleRenderJob(QQuickWindow* window);

    /**
     * @brief Create a Video item
     *
     */
    void createVideoItem();

    /**
     * @brief GstElement deleter
     *
     */
    static void gstElementDeleter(GstElement* element);

    /**
     * @brief Set play state in GstElement
     *
     */
    void gstElementPlayer();

    static const QString m_pipelineSufix;
    QString m_pipelineDescription;
    QSharedPointer<QQuickItem> m_quickitem;
    QSharedPointer<GstElement> m_gstPipeline;
};
