#ifndef MPVVIDEOITEM_H
#define MPVVIDEOITEM_H

#include <MpvAbstractItem>

class PlayerComponent;

class MpvVideoItem : public MpvAbstractItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit MpvVideoItem(QQuickItem *parent = nullptr);
    void setPlayerComponent(PlayerComponent* player);

    MpvController* controller() { return mpvController(); }

private:
    void reportSwaps();

    PlayerComponent* m_player = nullptr;
};

#endif // MPVVIDEOITEM_H
