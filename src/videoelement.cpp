#include <QQmlEngine>
#include <QQuickWindow>

#include "videoelement.h"

const QString VideoElement::m_pipelineSufix =
    " ! videoconvert !"
    " video/x-raw, format=RGBA, framerate=30/1 !"
    " queue !"
    " glupload !"
    " qmlglsink name=glsink sync=true";

VideoElement::VideoElement()
{
    connect(this, &QQuickItem::windowChanged, this, &VideoElement::scheduleRenderJob);
    connect(this, &VideoElement::descriptionChanged, this, [this]() {
        scheduleRenderJob(window());
    });
}

void VideoElement::gstElementDeleter(GstElement* element)
{
    gst_element_set_state(GST_ELEMENT(element), GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(element));
}

void VideoElement::scheduleRenderJob(QQuickWindow* window)
{
    if(!window) {
        qDebug() << "Window does not contain a valid pointer.";
        return;
    }

    if(m_gstPipeline) {
        qDebug() << "Pipeline already exist.";
        return;
    }

    if(m_pipelineDescription.isEmpty())  {
        qDebug() << "Pipeline is empty.";
        return;
    }

    m_gstPipeline.reset(gst_parse_launch((m_pipelineDescription + m_pipelineSufix).toStdString().c_str(), nullptr), gstElementDeleter);
    GstElement* gstSink = gst_bin_get_by_name(GST_BIN(m_gstPipeline.get()), "glsink");
    if(!gstSink) {
        qDebug() << "Problem in pipeline:" << m_pipelineDescription;
        return;
    }
    qDebug() << m_pipelineDescription;

    createVideoItem();
    g_object_set(gstSink, "widget", m_quickitem.get(), nullptr);

    connect(window, &QQuickWindow::beforeSynchronizing, this, &VideoElement::gstElementPlayer);
}

void VideoElement::gstElementPlayer()
{
    if(!window()) {
        return;
    }

    disconnect(window(), &QQuickWindow::beforeSynchronizing, 0, 0);
    gst_element_set_state(GST_ELEMENT(m_gstPipeline.get()), GST_STATE_PLAYING);
}

void VideoElement::createVideoItem()
{
    QQmlEngine* engine = qmlEngine(this);
    if(!engine) {
        qDebug() << "No qml engine to load visualization.";
        return;
    }
    QQmlComponent component(engine, "qrc:/qml/GstVideo.qml", this);
    m_quickitem.reset(qobject_cast<QQuickItem*>(component.create()));
    if(m_quickitem.isNull()) {
        qDebug() << "Failed to load QML component.";
        qDebug() << "Component status:" << component.status();
        if(component.isError()) {
            qDebug() << "Error list:" << component.errors();
        }
        return;
    }

    m_quickitem->setParentItem(qobject_cast<QQuickItem*>(this));
    qvariant_cast<QObject*>(m_quickitem.get()->property("anchors"))->setProperty("fill", QVariant::fromValue<VideoElement*>(this));
    qDebug() << qvariant_cast<QObject*>(m_quickitem.get()->property("anchors"))->property("fill");
}
