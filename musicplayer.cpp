#include "musicplayer.h"
#include "volumebutton.h"

#include <QtWidgets>
#include <QtWinExtras>

MusicPlayer::MusicPlayer(QWidget *parent):QWidget(parent),
    taskbarButton(0),taskbarProgress(0),thumbnailToolBar(0),
    playToolButton(0),forwardToolButton(0),backwardToolButton(0),
    mediaPlayer(0),playButton(0),volumeButton(0),
    positionSlider(0),positionLabel(0),infoLabel(0)
{
    createWidgets();
    createShortcuts();
    createJumpList();
    createTaskbar();
    createThumbnailToolBar();

    connect(&mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(updatePosition(qint64)));
    connect(&mediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(updateDuration(qint64)));
    connect(&mediaPlayer,SIGNAL(metaDataAvailableChanged(bool)),this,SLOT(updateInfo()));
    connect(&mediaPlayer,SIGNAL(error(QMediaPlayer::Error)),this,SLOT(handleError()));
    connect(&mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(updateState(QMediaPlayer::State)));

    stylize();
}
void MusicPlayer::openFile()
{
    const QStringList musicPaths=QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    const QString filePath=
            QFileDialog::getOpenFileName(this,tr("Open File"),
                                         musicPaths.isEmpty()?QDir::homePath():musicPaths.first(),
                                         tr("MP3 files(*.mp3);;All files(*.*)"));
    if(!filePath.isEmpty())
        playFile(filePath);
}
void MusicPlayer::playFile(const QString &filePath)
{
    playButton->setEnabled(true);
    infoLabel->setText(QFileInfo(filePath).fileName());

    mediaPlayer.setMedia(QUrl::fromLocalFile(filePath));
    mediaPlayer.play();
}
void MusicPlayer::togglePlayback()
{
    if(mediaPlayer.mediaStatus()==QMediaPlayer::NoMedia)
        openFile();
    else if (mediaPlayer.state()==QMediaPlayer::PlayingState)
        mediaPlayer.pause();
    else
        mediaPlayer.play();
}
void MusicPlayer::seekForward()
{
    positionSlider->triggerAction(QSlider::SliderPageStepAdd);
}
void MusicPlayer::seekBackward()
{
    positionSlider->triggerAction(QSlider::SliderPageStepSub);
}
bool MusicPlayer::event(QEvent *event)
{
    if(event->type()==QWinEvent::CompositionChange||event->type()==QWinEvent::ColorizationChange)
        stylize();
    return QWidget::event(event);
}
void MusicPlayer::mousePressEvent(QMouseEvent *event)
{
    offset=event->globalPos()-pos();
    event->accept();
}
void MusicPlayer::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPos()-offset);
    event->accept();
}
void MusicPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    offset=QPoint();
    event->accept();
}
void MusicPlayer::stylize()
{
    if(QtWin::isCompositionEnabled())
    {
        QtWin::extendFrameIntoClientArea(this,-1,-1,-1,-1);
        setAttribute(Qt::WA_TranslucentBackground,true);
        setAttribute(Qt::WA_NoSystemBackground,false);
        setStyleSheet("MusicPlayer{background:transparent;}");

    }
    else
    {
        QtWin::resetExtendedFrame(this);
        setAttribute(Qt::WA_TranslucentBackground,false);
        setStyleSheet(QString("MusicPlayer{background:%1;}").arg(QtWin::realColorizationColor().name()));
    }
    volumeButton->stylize();
}
void MusicPlayer::updateState(QMediaPlayer::State state)
{
    if(state==QMediaPlayer::PlayingState)
    {
        playButton->setToolTip(tr("Pause"));
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
    else
    {
        playButton->setToolTip(tr("Play"));
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}
void MusicPlayer::updatePosition(qint64 position)
{
    positionSlider->setValue(position);

    QTime duration(0,position/60000,qRound((position%60000)/1000.0));
    positionLabel->setText(duration.toString(tr("mm:ss")));
}
void MusicPlayer::updateDuration(qint64 duration)
{
    positionSlider->setRange(0,duration);
    positionSlider->setEnabled(duration>0);
    positionSlider->setPageStep(duration/10);
}
void MusicPlayer::setPosition(int position)
{
    if(qAbs(mediaPlayer.position()-position)>99)
        mediaPlayer.setPosition(position);
}
void MusicPlayer::updateInfo()
{
    QStringList info;
    QString title=mediaPlayer.metaData("Title").toString();
    if(!title.isEmpty())
        info+=title;
    QString author=mediaPlayer.metaData("Author").toString();
    if(!author.isEmpty())
        info+=author;

    if(!info.isEmpty())
        infoLabel->setText(info.join(tr("-")));
}
void MusicPlayer::handleError()
{
    playButton->setEnabled(false);\
    infoLabel->setText(tr("Error:%1").arg(mediaPlayer.errorString()));
}
void MusicPlayer::updateTaskbar()
{
    switch (mediaPlayer.state()) {
    case QMediaPlayer::PlayingState:
        taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        taskbarProgress->show();
        taskbarProgress->resume();
        break;
    case QMediaPlayer::PausedState:
        taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPause));
        taskbarProgress->show();
        taskbarProgress->pause();
        break;
    case QMediaPlayer::StoppedState:
        taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaStop));
        taskbarProgress->hide();
        break;
    }
}
void MusicPlayer::updateThumbnailToolBar()
{
    playToolButton->setEnabled(mediaPlayer.duration()>0);
    backwardToolButton->setEnabled(mediaPlayer.position()>0);
    forwardToolButton->setEnabled(mediaPlayer.position()<mediaPlayer.duration());
    if(mediaPlayer.state()==QMediaPlayer::PlayingState)
    {
        playToolButton->setToolTip(tr("Pause"));
        playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
    else
    {
        playToolButton->setToolTip(tr("Play"));
        playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}
void MusicPlayer::createWidgets()
{
    playButton=new QToolButton(this);
    playButton->setEnabled(false);
    playButton->setToolTip(tr("Play"));
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playButton,SIGNAL(clicked()),this,SLOT(togglePlayback()));

    QAbstractButton *openButton=new QToolButton(this);
    openButton->setText(tr("..."));
    openButton->setToolTip(tr("Open a file..."));
    openButton->setFixedSize(playButton->sizeHint());
    connect(openButton,SIGNAL(clicked()),this,SLOT(openFile()));

    volumeButton=new VolumeButton(this);
    volumeButton->setToolTip(tr("Adjust volume"));
    volumeButton->setVolume(mediaPlayer.volume());
    connect(volumeButton,SIGNAL(volumeChanged(int)),&mediaPlayer,SLOT(setVolume(int)));

    positionSlider=new QSlider(Qt::Horizontal,this);
    positionSlider->setEnabled(false);
    positionSlider->setToolTip(tr("seek"));
    connect(positionSlider,SIGNAL(valueChanged(int)),this,SLOT(setPosition(int)));

    infoLabel=new QLabel(this);
    positionLabel=new QLabel(tr("00:00"),this);
    positionLabel->setMinimumWidth(positionLabel->sizeHint().width());

    QBoxLayout *controlLayout=new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(positionSlider);
    controlLayout->addWidget(positionLabel);
    controlLayout->addWidget(volumeButton);
    QBoxLayout *mainLayout=new QVBoxLayout(this);
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(controlLayout);
}
void MusicPlayer::createShortcuts()
{
    QShortcut *quitShortcut=new QShortcut(QKeySequence::Quit,this);
    connect(quitShortcut,SIGNAL(activated()),qApp,SLOT(quit()));

    QShortcut *openShortcut=new QShortcut(QKeySequence::Open,this);
    connect(openShortcut,SIGNAL(activated()),this,SLOT(openFile()));

    QShortcut *toggleShortcut=new QShortcut(Qt::Key_Space,this);
    connect(toggleShortcut,SIGNAL(activated()),this,SLOT(togglePlayback()));

    QShortcut *forwardShortcut=new QShortcut(Qt::Key_Right,this);
    connect(forwardShortcut,SIGNAL(activated()),this,SLOT(seekForward()));

    QShortcut *backwardShortcut=new QShortcut(Qt::Key_Left,this);
    connect(backwardShortcut,SIGNAL(activated()),this,SLOT(seekBackward()));

    QShortcut *increaseShortcut=new QShortcut(Qt::Key_Up,this);
    connect(increaseShortcut,SIGNAL(activated()),volumeButton,SLOT(increaseVolume()));

    QShortcut *decreaseShortcut=new QShortcut(Qt::Key_Down,this);
    connect(decreaseShortcut,SIGNAL(activated()),volumeButton,SLOT(decreaseVolume()));
}
void MusicPlayer::createJumpList()
{
    QWinJumpList jumplist;
    jumplist.recent()->setVisible(true);
}
void MusicPlayer::createTaskbar()
{
    taskbarButton=new QWinTaskbarButton(this);
    taskbarButton->setWindow(windowHandle());
    taskbarProgress=taskbarButton->progress();

    connect(positionSlider,SIGNAL(valueChanged(int)),taskbarProgress,SLOT(setValue(int)));
    connect(positionSlider,SIGNAL(rangeChanged(int,int)),taskbarProgress,SLOT(setRange(int,int)));
    connect(&mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(updateTaskbar()));
}
void MusicPlayer::createThumbnailToolBar()
{
    thumbnailToolBar=new QWinThumbnailToolBar(this);
    thumbnailToolBar->setWindow(windowHandle());

    playToolButton=new QWinThumbnailToolButton(thumbnailToolBar);
    playToolButton->setEnabled(false);
    playToolButton->setToolTip(tr("Play"));
    playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playToolButton,SIGNAL(clicked()),this,SLOT(togglePlayback()));

    forwardToolButton=new QWinThumbnailToolButton(thumbnailToolBar);
    forwardToolButton->setEnabled(false);
    forwardToolButton->setToolTip(tr("Fast forward"));
    forwardToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    connect(forwardToolButton,SIGNAL(clicked()),this,SLOT(seekForward()));

    backwardToolButton=new QWinThumbnailToolButton(thumbnailToolBar);
    backwardToolButton->setEnabled(false);
    backwardToolButton->setToolTip(tr("Rewind"));
    backwardToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    connect(backwardToolButton,SIGNAL(clicked()),this,SLOT(seekBackward()));

    thumbnailToolBar->addButton(backwardToolButton);
    thumbnailToolBar->addButton(playToolButton);
    thumbnailToolBar->addButton(forwardToolButton);

    connect(&mediaPlayer,SIGNAL(positionChanged(qint64)),this,SLOT(updateThumbnailToolBar()));
    connect(&mediaPlayer,SIGNAL(durationChanged(qint64)),this,SLOT(updateThumbnailToolBar()));
    connect(&mediaPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(updateThumbnailToolBar()));

}























