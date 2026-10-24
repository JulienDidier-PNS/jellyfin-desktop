#include "MpvVideoItem.h"
#include "PlayerComponent.h"
#include <MpvController>
#include <QDebug>
#include <QQuickWindow>

#ifdef MPVQT_STATIC_BUILD
#include "mpvabstractitem_p.h"
#endif

MpvVideoItem::MpvVideoItem(QQuickItem *parent)
    : MpvAbstractItem(parent)
{
    qDebug() << "MpvVideoItem constructed";
    // Critical: Set vo=libmpv for Qt integration
    Q_EMIT setProperty("vo", "libmpv");

#ifdef Q_OS_WIN32
    // Force desktop OpenGL and disable advanced features for compatibility with older GPUs
    Q_EMIT setProperty("gpu-api", "opengl");
    Q_EMIT setProperty("opengl-es", "no");
#endif

    // ready() is emitted once the mpv render context exists
    connect(this, &MpvAbstractItem::ready, this, &MpvVideoItem::reportSwaps);
}

void MpvVideoItem::reportSwaps()
{
#ifdef MPVQT_STATIC_BUILD
    mpv_render_context* renderContext = d_ptr->m_mpv_gl;
    if (!renderContext || !window())
        return;

    // Tell mpv when the window actually presented a frame. Without this mpv
    // assumes a frame is on screen as soon as it is rendered, so its vsync
    // timing (display sync modes, frame timing stats) is guessed instead of
    // measured. Every window swap is reported, including ones that only
    // updated the web UI: mpv only counts swaps, so extra ones are harmless.
    // frameSwapped is emitted on the render thread, as report_swap expects.
    connect(window(), &QQuickWindow::frameSwapped, this, [renderContext]() {
        mpv_render_context_report_swap(renderContext);
    }, Qt::DirectConnection);
#endif
}

void MpvVideoItem::setPlayerComponent(PlayerComponent* player)
{
    qDebug() << "MpvVideoItem::setPlayerComponent called, mpvController():" << mpvController();
    m_player = player;

    // When mpv is ready, give controller to PlayerComponent
    connect(this, &MpvAbstractItem::ready, this, [this]() {
        qDebug() << "MpvVideoItem ready() signal fired!";
        if (m_player && mpvController()) {
            qDebug() << "Setting mpv controller and initializing";
            m_player->setMpvController(mpvController());
            m_player->initializeMpv();
        } else {
            qWarning() << "ready() fired but m_player:" << m_player << "mpvController():" << mpvController();
        }
    });

    // Check if already ready
    if (mpvController()) {
        qDebug() << "MpvVideoItem already ready, initializing now";
        m_player->setMpvController(mpvController());
        m_player->initializeMpv();
    } else {
        qDebug() << "MpvVideoItem not ready yet, waiting for ready() signal";
    }
}
