#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QMap>
#include <QString>
#include <QUrl>

class AudioManager : public QObject
{
    Q_OBJECT

public:
    enum class MusicType {
        Background,     // 主界面背景音乐
        Mode1Game,      // 模式1游戏音乐
        Mode2Game,      // 模式2游戏音乐
        Victory,        // 胜利音乐
        Defeat          // 失败音乐
    };

    enum class SoundType {
        PlayerAttack,   // 玩家攻击音效
        PlayerHurt,     // 玩家受伤音效
        EnemyHurt,      // 敌人受伤音效
        EnemyDeath,     // 敌人死亡音效
        ItemPickup,     // 道具拾取音效
        ButtonClick,    // 按钮点击音效
        SpecialEffect   // 特殊效果音效
    };

    static AudioManager* getInstance();
    static void destroyInstance();

    // 音乐控制
    void playBackgroundMusic();
    void playGameMusic(MusicType type);
    void stopCurrentMusic();
    void pauseCurrentMusic();
    void resumeCurrentMusic();
    
    // 音效控制
    void playSound(SoundType type, const QString& soundFile = "");
    
    // 音量控制
    void setMusicVolume(float volume);
    void setSoundVolume(float volume);
    void setMasterVolume(float volume);
    
    // 状态查询
    bool isMusicPlaying() const;
    bool isMusicPaused() const;
    MusicType getCurrentMusicType() const;
    
    // 音量获取
    float getMusicVolume() const;
    float getSoundVolume() const;
    bool isSoundEnabled() const;
    
    // 音效开关
    void setSoundEnabled(bool enabled);

private:
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager();
    
    void initializeAudioResources();
    void setupMusicPlayer();
    void setupSoundEffects();
    
    static AudioManager* instance;
    
    // 音乐播放器
    QMediaPlayer* musicPlayer;
    QAudioOutput* musicAudioOutput;
    
    // 音效播放器
    QSoundEffect* soundEffect;
    
    // 音频资源映射
    QMap<MusicType, QString> musicPaths;
    QMap<SoundType, QString> soundPaths;
    
    // 当前状态
    MusicType currentMusicType;
    bool musicPlaying;
    bool musicPaused;
    
    // 音量设置
    float musicVolume;
    float soundVolume;
    float masterVolume;
    
    // 音效开关
    bool soundEnabled;
};

#endif // AUDIO_MANAGER_H