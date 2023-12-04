// Brian Ruiz Header File
// Created 9/27/23
#ifndef BRUIZ_H
#define BRUIZ_H

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vector>
#include <string>
using namespace std;

extern int time_since_key_press(const bool get);
extern void updateAndPrintScore();
extern void switchGunSound();

class Enemy {
public:
    float pos[2];
    float vel[2];
    float velocity;
    float verts[4][2];
    float width, height;
    int hitCount;
    bool isDestroyed;

    Enemy();
    ~Enemy();
    void enemyPhysics();
    void drawEnemyFixed(float x, float y);
    void drawEnemy();
};

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    void loadSound(const string& filePath);
    void playSound(int soundIndex);
    const vector<ALuint>& getSoundBuffers() const {
        return soundBuffers;
    }
    size_t getNumSounds() const;
    vector<ALuint> soundBuffers;
    void checkSoundStateAndDelete(ALuint source);

};

class OpenALPlayer {
public:
    OpenALPlayer();
    ~OpenALPlayer();

    void playSound(const string& filePath);
    void setupBuffers();

private:
    void initOpenAL();
    void setupListener();
    void setupSources();
    void cleanup();

private:
    ALuint alBuffer[8];
    ALuint alSource[8];
    SoundManager soundManager;
};

#endif
