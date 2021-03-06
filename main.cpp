#include "musicplayer.h"

#include <QApplication>
#include <QFileInfo>
#include <QSettings>
#include <QIcon>
#include <QDir>

static void associateFileTypes(const QStringList &fileTypes)
{
    QString displayName = QGuiApplication::applicationDisplayName();
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();

    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\" + fileName, QSettings::NativeFormat);
    settings.setValue("FriendlyAppName", displayName);

    settings.beginGroup("SupportedTypes");
    foreach (const QString& fileType, fileTypes)
        settings.setValue(fileType, QString());
    settings.endGroup();

    settings.beginGroup("shell");
    settings.beginGroup("open");
    settings.setValue("FriendlyAppName", displayName);
    settings.beginGroup("Command");
    settings.setValue(".", QChar('"') + QDir::toNativeSeparators(filePath) + QString("\" \"%1\""));
}
int main(int argc,char *argv[])
{
    QApplication app(argc,argv);
    app.setApplicationName("MusicPlayer");
    app.setOrganizationName("QtWinExtras");
    app.setOrganizationDomain("qt-project.org");

    app.setApplicationDisplayName(QObject::tr("Music Palyer"));
    app.setWindowIcon(QIcon(":/musicplayer.png"));

    associateFileTypes(QStringList(".mp3"));
    MusicPlayer player;
    const QStringList arguments=QCoreApplication::arguments();
    if(arguments.size()>1)
        player.playFile(arguments.at(1));
    player.resize(300,60);
    player.show();

    return app.exec();

}
