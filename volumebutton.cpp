#include "volumebutton.h"
#include <QtWidgets>
#include <QtWinExtras>

VolumeButton::VolumeButton(QWidget *parent):
    QToolButton(parent),menu(0),label(0),slider(0)
{
    setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    setPopupMode(QToolButton::InstantPopup);

    QWidget *popup=new QWidget(this);
    slider=new QSlider(Qt::Horizontal,popup);
    slider->setRange(0,100);
    connect(slider,SIGNAL(valueChanged(int)),this,SIGNAL(volumeChanged(int)));

    label =new QLabel(popup);
    label->setAlignment(Qt::AlignCenter);
    label->setNum(100);
    label->setMinimumWidth(label->sizeHint().width());
    connect(slider,SIGNAL(valueChanged(int)),label,SLOT(setNum(int)));

    QBoxLayout *popupLayout=new QHBoxLayout(popup);
    popupLayout->setMargin(2);
    popupLayout->addWidget(slider);
    popupLayout->addWidget(label);

    QWidgetAction *action=new QWidgetAction(this);
    action->setDefaultWidget(popup);
    menu=new QMenu(this);
    menu->addAction(action);
    setMenu(menu);

    stylize();
}
void VolumeButton::increaseVolume()
{
    slider->triggerAction(QSlider::SliderPageStepAdd);
}
void VolumeButton::decreaseVolume()
{
    slider->triggerAction(QSlider::SliderPageStepSub);
}
int VolumeButton::volume()const
{
    return slider->value();
}
void VolumeButton::setVolume(int volume)
{
    slider->setValue(volume);
}
void VolumeButton::stylize()
{
    if(QtWin::isCompositionEnabled())
    {
        QtWin::enableBlurBehindWindow(menu);
        QString css("QMenu{border:1px solid %1;boarder-radius:2px;background:transparent;}");
        menu->setStyleSheet(css.arg(QtWin::realColorizationColor().name()));

    }
    else
    {
        QtWin::disableBlurBehindWindow(menu);
        QString css("QMenu{border:1px solid black;backgrond:%1;}");
        menu->setStyleSheet(css.arg(QtWin::realColorizationColor().name()));
    }
}
