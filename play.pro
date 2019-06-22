TEMPLATE = app

QT += qml quick widgets

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig debug
PKGCONFIG = \
    gstreamer-1.0 \
    gstreamer-video-1.0 \
    gstreamer-bad-audio-1.0 \
    gstreamer-controller-1.0 \
    gstreamer-mpegts-1.0 \
    gstreamer-plugins-bad-1.0 \
    gstreamer-rtsp-1.0 \
    gstreamer-allocators-1.0 \
    gstreamer-base-1.0 \
    gstreamer-fft-1.0 \
    gstreamer-net-1.0 \
    gstreamer-plugins-base-1.0 \
    gstreamer-sctp-1.0 \
    gstreamer-webrtc-1.0 \
    gstreamer-app-1.0 \
    gstreamer-check-1.0 \
    gstreamer-gl-1.0 \
    gstreamer-pbutils-1.0 \
    gstreamer-riff-1.0 \
    gstreamer-sdp-1.0 \
    gstreamer-audio-1.0 \
    gstreamer-codecparsers-1.0 \
    gstreamer-insertbin-1.0 \
    gstreamer-player-1.0 \
    gstreamer-rtp-1.0 \
    gstreamer-tag-1.0


SOURCES += \
    src/main.cpp \
    src/videoelement.cpp

HEADERS += \
    src/helper.h \
    src/videoelement.h

RESOURCES += resources.qrc
