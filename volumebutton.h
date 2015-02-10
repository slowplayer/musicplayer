#ifndef VOLUMEBUTTON_H
#define VOLUMEBUTTON_H

#include <QToolButton>
class QMenu;
class QLabel;
class QSlider;

class VolumeButton:public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
public:
    VolumeButton(QWidget *parent=0);
    int volume()const;
public slots:
    void increaseVolume();
    void decreaseVolume();
    void setVolume(int volume);
    void stylize();
signals:
    void volumeChanged(int volume);
private:
    QMenu *menu;
    QLabel *label;
    QSlider *slider;

};

#endif // VOLUMEBUTTON_H
