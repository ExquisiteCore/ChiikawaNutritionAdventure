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
        // 安全停止所有音频播放
        try {
            instance->stopCurrentMusic();
            if (instance->soundEffect && instance->soundEffect->isPlaying()) {
                instance->soundEffect->stop();
            }
        } catch (...) {
            qDebug() << "销毁音频管理器时停止播放发生异常";
        }
        
        // 延迟删除以确保所有音频操作完成
        instance->deleteLater();
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
    , soundEnabled(true)
{
    initializeAudioResources();
    setupMusicPlayer();
    setupSoundEffects();
}

AudioManager::~AudioManager()
{
    // 安全停止和清理音频资源
    if (musicPlayer) {
        // 断开所有信号连接，防止析构时触发回调
        disconnect(musicPlayer, nullptr, this, nullptr);
        if (musicPlayer->playbackState() != QMediaPlayer::StoppedState) {
            musicPlayer->stop();
        }
        musicPlayer->deleteLater();
        musicPlayer = nullptr;
    }
    if (musicAudioOutput) {
        musicAudioOutput->deleteLater();
        musicAudioOutput = nullptr;
    }
    if (soundEffect) {
        if (soundEffect->isPlaying()) {
            soundEffect->stop();
        }
        soundEffect->deleteLater();
        soundEffect = nullptr;
    }
    
    // 重置状态标志
    musicPlaying = false;
    musicPaused = false;
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
    
    // 设置音乐状态监听
    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged, this, 
            [this](QMediaPlayer::MediaStatus status) {
                qDebug() << "音乐状态变化:" << status;
                if (status == QMediaPlayer::EndOfMedia && musicPlaying) {
                    // 只有背景音乐和游戏音乐才循环播放，胜利/失败音乐播放一次后停止
                    if (currentMusicType == MusicType::Background || 
                        currentMusicType == MusicType::Mode1Game || 
                        currentMusicType == MusicType::Mode2Game) {
                        musicPlayer->setPosition(0);
                        musicPlayer->play();
                    } else {
                        // 胜利/失败音乐播放完毕后停止
                        musicPlaying = false;
                        musicPaused = false;
                        qDebug() << "胜利/失败音乐播放完毕，停止播放";
                    }
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
    if (!musicPlayer || !musicAudioOutput) {
        qDebug() << "音乐播放器未初始化";
        return;
    }
    
    // 如果当前已经在播放背景音乐，则不重复播放
    if (musicPlaying && currentMusicType == MusicType::Background) {
        return;
    }
    
    stopCurrentMusic();
    
    try {
        currentMusicType = MusicType::Background;
        QString musicPath = musicPaths[MusicType::Background];
        
        if (musicPath.isEmpty()) {
            qDebug() << "背景音乐路径为空";
            return;
        }
        
        musicPlayer->setSource(QUrl(musicPath));
        musicPlayer->setLoops(QMediaPlayer::Infinite);
        musicPlayer->play();
        musicPlaying = true;
        musicPaused = false;
        
        qDebug() << "开始播放背景音乐:" << musicPath;
    } catch (...) {
        qDebug() << "播放背景音乐时发生异常";
        musicPlaying = false;
        musicPaused = false;
    }
}

void AudioManager::playGameMusic(MusicType type)
{
    if (!musicPlayer || !musicAudioOutput) {
        qDebug() << "音乐播放器未初始化";
        return;
    }
    
    // 如果当前已经在播放相同的游戏音乐，则不重复播放
    if (musicPlaying && currentMusicType == type) {
        return;
    }
    
    stopCurrentMusic();
    
    try {
        currentMusicType = type;
        if (musicPaths.contains(type)) {
            QString musicPath = musicPaths[type];
            
            if (musicPath.isEmpty()) {
                qDebug() << "音乐路径为空，类型:" << static_cast<int>(type);
                return;
            }
            
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
    } catch (...) {
        qDebug() << "播放游戏音乐时发生异常，类型:" << static_cast<int>(type);
        musicPlaying = false;
        musicPaused = false;
    }
}

void AudioManager::stopCurrentMusic()
{
    if (musicPlayer && musicPlaying) {
        // 安全停止音乐播放
        try {
            if (musicPlayer->playbackState() != QMediaPlayer::StoppedState) {
                musicPlayer->stop();
            }
            musicPlaying = false;
            musicPaused = false;
            qDebug() << "停止当前音乐";
        } catch (...) {
            qDebug() << "停止音乐时发生异常";
            musicPlaying = false;
            musicPaused = false;
        }
    }
}

void AudioManager::pauseCurrentMusic()
{
    if (musicPlayer && musicPlaying && !musicPaused) {
        try {
            if (musicPlayer->playbackState() == QMediaPlayer::PlayingState) {
                musicPlayer->pause();
                musicPaused = true;
                qDebug() << "暂停当前音乐";
            }
        } catch (...) {
            qDebug() << "暂停音乐时发生异常";
        }
    }
}

void AudioManager::resumeCurrentMusic()
{
    if (musicPlayer && musicPlaying && musicPaused) {
        try {
            if (musicPlayer->playbackState() == QMediaPlayer::PausedState) {
                musicPlayer->play();
                musicPaused = false;
                qDebug() << "恢复当前音乐";
            }
        } catch (...) {
            qDebug() << "恢复音乐时发生异常";
        }
    }
}

void AudioManager::playSound(SoundType type, const QString& soundFile)
{
    if (!soundEffect || !soundEnabled) {
        qDebug() << "音效播放器未初始化或音效已禁用";
        return;
    }
    
    QString filePath;
    if (!soundFile.isEmpty()) {
        filePath = soundFile;
    } else if (soundPaths.contains(type)) {
        filePath = soundPaths[type];
    } else {
        qDebug() << "未找到音效文件，类型:" << static_cast<int>(type);
        return;
    }
    
    try {
        // 安全设置音效源和播放
        soundEffect->setSource(QUrl(filePath));
        if (soundEffect->status() == QSoundEffect::Ready || 
            soundEffect->status() == QSoundEffect::Loading) {
            soundEffect->play();
            qDebug() << "播放音效:" << filePath;
        } else {
            qDebug() << "音效文件加载失败:" << filePath;
        }
    } catch (...) {
        qDebug() << "播放音效时发生异常:" << filePath;
    }
}

void AudioManager::setMusicVolume(float volume)
{
    musicVolume = qBound(0.0f, volume, 1.0f);
    if (musicAudioOutput) {
        try {
            musicAudioOutput->setVolume(musicVolume * masterVolume);
            qDebug() << "设置音乐音量:" << musicVolume;
        } catch (...) {
            qDebug() << "设置音乐音量时发生异常";
        }
    } else {
        qDebug() << "音乐输出设备未初始化，无法设置音量";
    }
}

void AudioManager::setSoundVolume(float volume)
{
    soundVolume = qBound(0.0f, volume, 1.0f);
    if (soundEffect) {
        try {
            soundEffect->setVolume(soundVolume * masterVolume);
            qDebug() << "设置音效音量:" << soundVolume;
        } catch (...) {
            qDebug() << "设置音效音量时发生异常";
        }
    } else {
        qDebug() << "音效播放器未初始化，无法设置音量";
    }
}

void AudioManager::setMasterVolume(float volume)
{
    masterVolume = qBound(0.0f, volume, 1.0f);
    
    // 更新所有音频组件的音量
    try {
        if (musicAudioOutput) {
            musicAudioOutput->setVolume(musicVolume * masterVolume);
        }
        if (soundEffect) {
            soundEffect->setVolume(soundVolume * masterVolume);
        }
        qDebug() << "设置主音量:" << masterVolume;
    } catch (...) {
        qDebug() << "设置主音量时发生异常";
    }
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

float AudioManager::getMusicVolume() const
{
    return musicVolume;
}

float AudioManager::getSoundVolume() const
{
    return soundVolume;
}

bool AudioManager::isSoundEnabled() const
{
    return soundEnabled;
}

void AudioManager::setSoundEnabled(bool enabled)
{
    soundEnabled = enabled;
    qDebug() << "设置音效开关:" << enabled;
}