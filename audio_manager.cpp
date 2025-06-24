#include "audio_manager.h"
#include <QDebug>

AudioManager* AudioManager::instance = nullptr;

AudioManager* AudioManager::getInstance()
{
    if (!instance) {
        instance = new AudioManager();
    }
    return instance;
}

void AudioManager::destroyInstance()
{
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

AudioManager::AudioManager(QObject *parent)
    : QObject(parent)
    , musicPlayer(nullptr)
    , musicAudioOutput(nullptr)
    , soundEffect(nullptr)
    , currentMusicType(MusicType::Background)
    , musicPlaying(false)
    , musicPaused(false)
    , musicVolume(0.3f)
    , soundVolume(0.5f)
    , masterVolume(1.0f)
{
    initializeAudioResources();
    setupMusicPlayer();
    setupSoundEffects();
}

AudioManager::~AudioManager()
{
    if (musicPlayer) {
        musicPlayer->stop();
        delete musicPlayer;
    }
    if (musicAudioOutput) {
        delete musicAudioOutput;
    }
    if (soundEffect) {
        delete soundEffect;
    }
}

void AudioManager::initializeAudioResources()
{
    // 初始化音乐资源路径
    musicPaths[MusicType::Background] = "qrc:/Sounds/Main_sound.wav";
    musicPaths[MusicType::Mode1Game] = "qrc:/Sounds/Game_sound.wav";
    musicPaths[MusicType::Mode2Game] = "qrc:/Sounds/Game_sound.wav";
    musicPaths[MusicType::Victory] = "qrc:/Sounds/Win.wav";
    musicPaths[MusicType::Defeat] = "qrc:/Sounds/Lose_1.wav";
    
    // 初始化音效资源路径
    soundPaths[SoundType::PlayerAttack] = "qrc:/Sounds/TapButton.wav";
    soundPaths[SoundType::PlayerHurt] = "qrc:/Sounds/Cough_sound.wav";
    soundPaths[SoundType::EnemyHurt] = "qrc:/Sounds/Cough_sound.wav";
    soundPaths[SoundType::EnemyDeath] = "qrc:/Sounds/Lose_1.wav";
    soundPaths[SoundType::ItemPickup] = "qrc:/Sounds/Bean_sound_short.wav";
    soundPaths[SoundType::ButtonClick] = "qrc:/Sounds/TapButton.wav";
    soundPaths[SoundType::SpecialEffect] = "qrc:/Sounds/Double_Kill.wav";
}

void AudioManager::setupMusicPlayer()
{
    musicPlayer = new QMediaPlayer(this);
    musicAudioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(musicAudioOutput);
    
    // 设置音乐循环播放和状态监听
    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged, this, 
            [this](QMediaPlayer::MediaStatus status) {
                qDebug() << "音乐状态变化:" << status;
                if (status == QMediaPlayer::EndOfMedia && musicPlaying) {
                    musicPlayer->setPosition(0);
                    musicPlayer->play();
                }
            });
    
    connect(musicPlayer, &QMediaPlayer::playbackStateChanged, this,
            [this](QMediaPlayer::PlaybackState state) {
                qDebug() << "播放状态变化:" << state;
            });
    
    connect(musicPlayer, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error error, const QString &errorString) {
                qDebug() << "音乐播放错误:" << error << errorString;
            });
    
    // 设置初始音量
    musicAudioOutput->setVolume(musicVolume * masterVolume);
}

void AudioManager::setupSoundEffects()
{
    soundEffect = new QSoundEffect(this);
    soundEffect->setVolume(soundVolume * masterVolume);
}

void AudioManager::playBackgroundMusic()
{
    if (!musicPlayer) return;
    
    // 如果当前已经在播放背景音乐，则不重复播放
    if (musicPlaying && currentMusicType == MusicType::Background) {
        return;
    }
    
    stopCurrentMusic();
    
    currentMusicType = MusicType::Background;
    musicPlayer->setSource(QUrl(musicPaths[MusicType::Background]));
    musicPlayer->setLoops(QMediaPlayer::Infinite);
    musicPlayer->play();
    musicPlaying = true;
    musicPaused = false;
    
    qDebug() << "开始播放背景音乐";
}

void AudioManager::playGameMusic(MusicType type)
{
    if (!musicPlayer) return;
    
    // 如果当前已经在播放相同的游戏音乐，则不重复播放
    if (musicPlaying && currentMusicType == type) {
        return;
    }
    
    stopCurrentMusic();
    
    currentMusicType = type;
    if (musicPaths.contains(type)) {
        QString musicPath = musicPaths[type];
        qDebug() << "准备播放音乐，类型:" << static_cast<int>(type) << "路径:" << musicPath;
        
        musicPlayer->setSource(QUrl(musicPath));
        
        // 游戏音乐和背景音乐循环播放，胜利/失败音乐播放一次
        if (type == MusicType::Mode1Game || type == MusicType::Mode2Game) {
            musicPlayer->setLoops(QMediaPlayer::Infinite);
            qDebug() << "设置游戏音乐循环播放";
        } else {
            musicPlayer->setLoops(1);
        }
        
        musicPlayer->play();
        musicPlaying = true;
        musicPaused = false;
        
        qDebug() << "开始播放游戏音乐，类型:" << static_cast<int>(type) << "音量:" << musicAudioOutput->volume();
    } else {
        qDebug() << "错误：找不到音乐类型" << static_cast<int>(type) << "的路径配置";
    }
}

void AudioManager::stopCurrentMusic()
{
    if (musicPlayer && musicPlaying) {
        musicPlayer->stop();
        musicPlaying = false;
        musicPaused = false;
        qDebug() << "停止当前音乐";
    }
}

void AudioManager::pauseCurrentMusic()
{
    if (musicPlayer && musicPlaying && !musicPaused) {
        musicPlayer->pause();
        musicPaused = true;
        qDebug() << "暂停当前音乐";
    }
}

void AudioManager::resumeCurrentMusic()
{
    if (musicPlayer && musicPlaying && musicPaused) {
        musicPlayer->play();
        musicPaused = false;
        qDebug() << "恢复当前音乐";
    }
}

void AudioManager::playSound(SoundType type, const QString& soundFile)
{
    if (!soundEffect) return;
    
    QString filePath;
    if (!soundFile.isEmpty()) {
        filePath = soundFile;
    } else if (soundPaths.contains(type)) {
        filePath = soundPaths[type];
    } else {
        qDebug() << "未找到音效文件，类型:" << static_cast<int>(type);
        return;
    }
    
    soundEffect->setSource(QUrl(filePath));
    soundEffect->play();
    
    qDebug() << "播放音效:" << filePath;
}

void AudioManager::setMusicVolume(float volume)
{
    musicVolume = qBound(0.0f, volume, 1.0f);
    if (musicAudioOutput) {
        musicAudioOutput->setVolume(musicVolume * masterVolume);
    }
    qDebug() << "设置音乐音量:" << musicVolume;
}

void AudioManager::setSoundVolume(float volume)
{
    soundVolume = qBound(0.0f, volume, 1.0f);
    if (soundEffect) {
        soundEffect->setVolume(soundVolume * masterVolume);
    }
    qDebug() << "设置音效音量:" << soundVolume;
}

void AudioManager::setMasterVolume(float volume)
{
    masterVolume = qBound(0.0f, volume, 1.0f);
    
    // 更新所有音频组件的音量
    if (musicAudioOutput) {
        musicAudioOutput->setVolume(musicVolume * masterVolume);
    }
    if (soundEffect) {
        soundEffect->setVolume(soundVolume * masterVolume);
    }
    
    qDebug() << "设置主音量:" << masterVolume;
}

bool AudioManager::isMusicPlaying() const
{
    return musicPlaying && !musicPaused;
}

bool AudioManager::isMusicPaused() const
{
    return musicPaused;
}

AudioManager::MusicType AudioManager::getCurrentMusicType() const
{
    return currentMusicType;
}