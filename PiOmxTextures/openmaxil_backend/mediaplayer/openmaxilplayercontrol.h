/*
 * Author:  Luca Carlon
 * Company: -
 * Date:    04.14.2013
 * Project: OpenMAXIL QtMultimedia Plugin
 */

#ifndef QOPENMAXILPLAYERCONTROL_H
#define QOPENMAXILPLAYERCONTROL_H

/*------------------------------------------------------------------------------
|    includes
+-----------------------------------------------------------------------------*/
#include <QtCore/qobject.h>
#include <QtCore/qstack.h>
#include <QtCore/qsemaphore.h>
#include <QtCore/qmutex.h>
#include <QtMultimedia/qmediacontent.h>

#include <qmediaplayercontrol.h>
#include <qmediaplayer.h>

#include <limits.h>

#include <omx_textureprovider.h>

QT_BEGIN_NAMESPACE

/*------------------------------------------------------------------------------
|    definitions
+-----------------------------------------------------------------------------*/
class QMediaPlaylist;
class QMediaPlaylistNavigator;
class QSocketNotifier;

class OMX_MediaProcessor;

enum PlayerCommandType {
   PLAYER_COMMAND_TYPE_SET_MEDIA,
   PLAYER_COMMAND_TYPE_PLAY,
   PLAYER_COMMAND_TYPE_PAUSE,
   PLAYER_COMMAND_TYPE_STOP,
   PLAYER_COMMAND_TYPE_FREE_TEXTURE_DATA
};


/*------------------------------------------------------------------------------
|    PlayerCommand class
+-----------------------------------------------------------------------------*/
struct PlayerCommand
{
   virtual ~PlayerCommand() {}

   PlayerCommandType m_playerCommandType;
};

/*------------------------------------------------------------------------------
|    PlayerCommandSetMedia class
+-----------------------------------------------------------------------------*/
struct PlayerCommandSetMedia : public PlayerCommand
{
   virtual ~PlayerCommandSetMedia() {}

   QMediaContent m_mediaContent;
};

/*------------------------------------------------------------------------------
|    PlayerCommandPlay class
+-----------------------------------------------------------------------------*/
struct PlayerCommandPlay : public PlayerCommand
{
   virtual ~PlayerCommandPlay() {}

   QSize m_mediaSize;
};

/*------------------------------------------------------------------------------
|    PlayerCommandPause class
+-----------------------------------------------------------------------------*/
struct PlayerCommandPause : public PlayerCommand
{
   virtual ~PlayerCommandPause() {}
};

/*------------------------------------------------------------------------------
|    PlayerCommandStop class
+-----------------------------------------------------------------------------*/
struct PlayerCommandStop : public PlayerCommand
{
   virtual ~PlayerCommandStop() {}
};

/*------------------------------------------------------------------------------
|    PlayerCommandStop class
+-----------------------------------------------------------------------------*/
struct PlayerCommandFreeTextureData : public PlayerCommand
{
   virtual ~PlayerCommandFreeTextureData() {}

   OMX_TextureData* m_textureData;
};

/*------------------------------------------------------------------------------
|    OpenMAXILPlayerControl class
+-----------------------------------------------------------------------------*/
class OpenMAXILPlayerControl : public QMediaPlayerControl, public OMX_TextureProvider
{
    Q_OBJECT
public:
    OpenMAXILPlayerControl(QObject* parent = 0);
    ~OpenMAXILPlayerControl();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    qint64 position() const;
    qint64 duration() const;

    int bufferStatus() const;

    int volume() const;
    bool isMuted() const;

    bool isAudioAvailable() const;
    bool isVideoAvailable() const;

    bool isSeekable() const;
    QMediaTimeRange availablePlaybackRanges() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QMediaContent media() const;
    const QIODevice* mediaStream() const;

    void setMedia(const QMediaContent&, QIODevice*);
    void setMediaInt(const QMediaContent& mediaContent);

    // OMX_TextureProvider interface.
    OMX_TextureData* instantiateTexture(QSize size);
    void freeTexture(OMX_TextureData* textureData);

public Q_SLOTS:
    void setPosition(qint64 pos);

    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

    void onSceneGraphInitialized();
    void onBeforeRendering();

signals:
    void textureReady(const OMX_TextureData* textureData);
    void textureInvalidated();

private slots:
    void requestProcessPendingCommands();
    void playInt();
    void pauseInt();
    void stopInt();
    void freeTextureInt(OMX_TextureData* textureData);

private:
    void appendCommand(PlayerCommand* command);
    void processCommands();

    bool m_ownStream;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QStack<QMediaPlayer::State> m_stateStack;
    QStack<QMediaPlayer::MediaStatus> m_mediaStatusStack;

    bool m_seekToStartPending;
    qint64 m_pendingSeekPosition;
    QMediaContent m_currentResource;

    OMX_TextureProvider* m_texProvider;
    OMX_MediaProcessor*  m_mediaProcessor;
    OMX_TextureData*     m_textureData;

    QList<PlayerCommand*> m_pendingCommands;
    QMutex                m_pendingCommandsMutex;
    bool                  m_sceneGraphInitialized;
};

QT_END_NAMESPACE

#endif // QOPENMAXILPLAYERCONTROL_H
