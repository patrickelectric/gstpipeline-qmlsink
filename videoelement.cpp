#include <QQmlEngine>
#include <QQuickWindow>

#include "videoelement.h"

VideoElement::VideoElement()
{
    _pipelineDescription =
        "videotestsrc pattern=ball !"
        //"image/jpeg, width=1920, height=1080, framerate=30/1"
        //"jpegparse"
        //"jpegdec"
        "videoconvert n-threads=16 !"
        "video/x-raw, format=RGBA, framerate=30/1 !"
        "queue !"
        "glupload !"
        "qmlglsink name=sink sync=true"
        ;

    connect(this, &QQuickItem::windowChanged, this, &VideoElement::scheduleRenderJob);
}

void VideoElement::scheduleRenderJob(QQuickWindow* window)
{
    if(!window) {
        qCritical() << "Window does not contain a valid pointer.";
        return;
    }

    GstElement* gstPipeline = gst_parse_launch(_pipelineDescription.toStdString().c_str(), nullptr);
    GstElement* gstSink = gst_bin_get_by_name(GST_BIN(gstPipeline), "sink");
    g_assert(gstPipeline && gstSink);

    createVideoItem();
    qDebug() << _controlPanel.get();
    auto videoItem = _controlPanel.get()->findChild<QQuickItem*>("videoItem");
    qDebug() << videoItem;
    //if(videoItem) {
        g_object_set(gstSink, "widget", _controlPanel.get(), nullptr);
    //}

    window->scheduleRenderJob(new GstPipelinePlayer(gstPipeline), QQuickWindow::BeforeSynchronizingStage);
}


void VideoElement::createVideoItem()
{
    QQmlEngine* engine = qmlEngine(this);
    if(!engine) {
        qDebug() << "No qml engine to load visualization.";
        return;
    }
    QQmlComponent component(engine, "qrc:/GstVideo.qml", this);
    _controlPanel.reset(qobject_cast<QQuickItem*>(component.create()));
    if(_controlPanel.isNull()) {
        qDebug() << "Failed to load QML component.";
        qDebug() << "Component status:" << component.status();
        if(component.isError()) {
            qDebug() << "Error list:" << component.errors();
        }
        return;
    }

    _controlPanel->setParentItem(qobject_cast<QQuickItem*>(this));
    qvariant_cast<QObject*>(_controlPanel.get()->property("anchors"))->setProperty("fill", QVariant::fromValue<VideoElement*>(this));
    qDebug() << qvariant_cast<QObject*>(_controlPanel.get()->property("anchors"))->property("fill");
}
