// Brian Ruiz Showcase File
// Created 9/25/23
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "bruiz.h"
#include "log.h"
#include "global.h"
#include <thread>
#include "skumar.h"
#include "bstitt.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>


using namespace std;

extern Enemy enemy;
extern Platform platform;
extern Player player;
extern OpenALPlayer openALPlayer;
extern int snehalTest;

// Time since last key was pressed function
int time_since_key_press(const bool get)
{
    static int firstTime = 1;
    static int lastKeyPressTime;
    if (firstTime) {
        firstTime = 0;
        lastKeyPressTime = time(NULL);
    }
    if (get) {
        return time(NULL) - lastKeyPressTime;
    }
    lastKeyPressTime = time(NULL);
    return 0;
}

// Adding enemies to the game
Enemy::Enemy()
{
    pos[0] = 250.0f;
    pos[1] = 250.0f;
    vel[0] = 0.0f;
    vel[1] = 0.0f;

    width = height = 15.0f;
    hitCount = 0;
    velocity = 2.0f;
    isDestroyed = false;

    verts[0][0] = -15.0f;
    verts[0][1] = 0.0f;
    verts[1][0] = -15.0f;
    verts[1][1] = 30.0f;
    verts[2][0] = 15.0f;
    verts[2][1] = 30.0f;
    verts[3][0] = 15.0f;
    verts[3][1] = 0.0f;
}

Enemy::~Enemy()
{
}

// Update the enemy's position and velocity.
void Enemy::enemyPhysics()
{
    // enemy.pos[1] -= 6.0f;
    // platform.enemy.pos[0] -= velocity;
}

void Enemy::drawEnemyFixed(float x, float y)
{
    glPushMatrix();
    glColor3ub(255, 0, 0);

    glTranslatef(x, y, 0.0f);
    // glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glVertex2f(verts[0][0], verts[0][1]); // Bottom left vertex
    glVertex2f(verts[1][0], verts[1][1]); // Top left vertex
    glVertex2f(verts[2][0], verts[2][1]); // Top right vertex
    glVertex2f(verts[3][0], verts[3][1]); // Bottom right vertex
    glEnd();

    glPopMatrix();
}

void Enemy::drawEnemy()
{
    if (!isDestroyed) {
        glPushMatrix();
        glColor3ub(255, 0, 0);

        glTranslatef(pos[0], pos[1], 0.0f);
        // glRotatef(angle, 0.0f, 0.0f, 1.0f);

        glBegin(GL_QUADS);
        glVertex2f(verts[0][0], verts[0][1]); // Bottom left vertex
        glVertex2f(verts[1][0], verts[1][1]); // Top left vertex
        glVertex2f(verts[2][0], verts[2][1]); // Top right vertex
        glVertex2f(verts[3][0], verts[3][1]); // Bottom right vertex
        glEnd();

        glPopMatrix();
    }
    else {
        pos[1] = -100.0f;
    }
}

// Print Score without Spam
static int previousScore = -1;
void updateAndPrintScore()
{
    int currentScore = print_score();

    // Check if the score has changed
    if (currentScore != previousScore) {
        // Update the previousScore variable
        previousScore = currentScore;

        // Print the updated score
        cout << "Score: " << currentScore << endl;
    }
}

// Adding audio to the game

// Implementation of SoundManager class
SoundManager::SoundManager() {
    alutInit(0, NULL);
}

SoundManager::~SoundManager() {
    for (ALuint buffer : soundBuffers) {
        alDeleteBuffers(1, &buffer);
    }
    alutExit();
}

void SoundManager::loadSound(const string& filePath) {
    ALuint buffer = alutCreateBufferFromFile(filePath.c_str());
    if (buffer == AL_NONE) {
        cerr << "Error loading sound file: " << filePath << endl;
        return;
    }
    soundBuffers.push_back(buffer);
}

thread soundThread;
void SoundManager::playSound(int soundIndex) {
    if (soundIndex >= 0 && soundIndex < static_cast<int>(soundBuffers.size())) {
        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, soundBuffers[soundIndex]);
        alSourcePlay(source);

        // Start a new thread to handle sound playback and deletion
        soundThread = thread(&SoundManager::checkSoundStateAndDelete, this, source);
        soundThread.detach(); // Detach the thread to allow it to run independently
    }
}

void SoundManager::checkSoundStateAndDelete(ALuint source) {
    // Wait for the sound to finish playing
    ALint state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

    // Delete the source once it's done playing
    alDeleteSources(1, &source);
}

size_t SoundManager::getNumSounds() const {
    return soundBuffers.size();
}

// Implementation of OpenALPlayer class
OpenALPlayer::OpenALPlayer() 
{
    initOpenAL();
    setupListener();
    setupSources();

    // Load sounds using the sound manager
    soundManager.loadSound("./Audio/start.wav");
    soundManager.loadSound("./Audio/jump.wav");
    soundManager.loadSound("./Audio/arcadeLaser.wav");
    soundManager.loadSound("./Audio/eggCrack.wav");
    soundManager.loadSound("./Audio/shot-shotgun.wav");
    soundManager.loadSound("./Audio/shot-underwater.wav");
    soundManager.loadSound("./Audio/win.wav");
    soundManager.loadSound("./Audio/shot-snowball.wav");
    soundManager.loadSound("./Audio/shot-skull.wav");
    soundManager.loadSound("./Audio/monster-crash.wav");
    // Add more sounds if needed
}

OpenALPlayer::~OpenALPlayer() 
{
    cleanup();
}

void OpenALPlayer::playSound(const string& filePath) 
{
    // Find the index of the sound in the sound manager
    int soundIndex = -1;
    for (size_t i = 0; i < soundManager.getNumSounds(); ++i) {
        if (filePath == "./Audio/start.wav" && i == 0) {
            soundIndex = i;
            break;
        } else if (filePath == "./Audio/jump.wav" && i == 1) {
            soundIndex = i;
            break;
        } else if (filePath == "./Audio/arcadeLaser.wav" && i == 2) { 
            soundIndex = i;
            break;
        }else if (filePath == "./Audio/eggCrack.wav" && i == 3) {
            soundIndex = i;
            break;
        }else if (filePath == "./Audio/shot-shotgun.wav" && i == 4) {
            soundIndex = i;
            break;
        }else if (filePath == "./Audio/shot-underwater.wav" && i == 5) {
            soundIndex = i;
            break;
        }else if (filePath == "./Audio/win.wav" && i == 6) {
            soundIndex = i;
            break;
        }else if (filePath == "./Audio/shot-snowball.wav" && i == 7) {
            soundIndex = i;
            break;
        }else if (filePath == "./Audio/shot-skull.wav" && i == 8) {
            soundIndex = i;
            break;
        }
        else if (filePath == "./Audio/monster-crash.wav" && i == 9) {
            soundIndex = i;
            break;
        }
    // Add more conditions for additional sounds
    }

    // Play the sound using the sound manager
    soundManager.playSound(soundIndex);
}

void OpenALPlayer::initOpenAL()
{
    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
        cerr << "ERROR: alutInit()" << endl;
        // Handle initialization error
    }
    alGetError(); // Clear error state
}

void OpenALPlayer::setupListener()
{
    float vec[6] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);
}

void OpenALPlayer::setupBuffers()
{
    alBuffer[0] = alutCreateBufferFromFile("./Audio/start.wav");
    alBuffer[1] = alutCreateBufferFromFile("./Audio/jump.wav");
    alBuffer[2] = alutCreateBufferFromFile("./Audio/arcadeLaser.wav");
    alBuffer[3] = alutCreateBufferFromFile("./Audio/eggCrack.wav");
    alBuffer[4] = alutCreateBufferFromFile("./Audio/shot-shotgun.wav");
    alBuffer[5] = alutCreateBufferFromFile("./Audio/shot-underwater.wav");
    alBuffer[6] = alutCreateBufferFromFile("./Audio/win.wav");
    alBuffer[7] = alutCreateBufferFromFile("./Audio/shot-snowball.wav");
    alBuffer[8] = alutCreateBufferFromFile("./Audio/shot-skull.wav");
    alBuffer[9] = alutCreateBufferFromFile("./Audio/monster-crash.wav");

    // Add more buffers if needed for additional sounds
}

void OpenALPlayer::setupSources()
{
    alGenSources(9, alSource);
    alSourcei(alSource[0], AL_BUFFER, alBuffer[0]);
    alSourcei(alSource[1], AL_BUFFER, alBuffer[1]);
    alSourcei(alSource[1], AL_BUFFER, alBuffer[2]);

    alSourcef(alSource[0], AL_GAIN, 1.0f);
    alSourcef(alSource[0], AL_PITCH, 1.0f);
    alSourcei(alSource[0], AL_LOOPING, AL_FALSE);
    
    alSourcef(alSource[1], AL_GAIN, 0.5f);
    alSourcef(alSource[1], AL_PITCH, 1.0f);
    alSourcei(alSource[1], AL_LOOPING, AL_FALSE);

    alSourcef(alSource[2], AL_GAIN, 1.0f);
    alSourcef(alSource[2], AL_PITCH, 1.0f);
    alSourcei(alSource[2], AL_LOOPING, AL_FALSE);
}

void OpenALPlayer::cleanup()
{
for (int i = 0; i < 9; ++i) {
        alDeleteSources(1, &alSource[i]);
        alDeleteBuffers(1, &alBuffer[i]);
    }

    ALCcontext *Context = alcGetCurrentContext();
    ALCdevice *Device = alcGetContextsDevice(Context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(Context);
    alcCloseDevice(Device);
}

void switchGunSound(){
    if (snehalTest) {
        openALPlayer.playSound("./Audio/arcadeLaser.wav");
    }
    else {
        if (player.trophyDetected == 0) {
        openALPlayer.playSound("./Audio/arcadeLaser.wav"); 
        } else if (player.trophyDetected == 1) {
        openALPlayer.playSound("./Audio/shot-shotgun.wav"); 
        } else if (player.trophyDetected == 2) {
        openALPlayer.playSound("./Audio/shot-underwater.wav"); 
        } else if (player.trophyDetected == 3) {
        openALPlayer.playSound("./Audio/shot-snowball.wav"); 
        } else if (player.trophyDetected == 4){
        openALPlayer.playSound("./Audio/shot-skull.wav"); 
        }
    }
}