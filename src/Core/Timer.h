#pragma once

#include <chrono>

class Timer
{
public:
    Timer();

    void Reset();
    void Start();
    void Stop();
    void Tick();

    float GetDeltaTime() const;
    float GetTotalTime() const;
    int GetFPS() const;

private:
    bool isRunning;
    std::chrono::high_resolution_clock::time_point baseTime;
    std::chrono::high_resolution_clock::duration pausedTime;
    std::chrono::high_resolution_clock::time_point prevTime;
    std::chrono::high_resolution_clock::time_point currTime;

    float deltaTime;
    float totalTime;

    int frameCount;
    float timeElapsed;
    int fps;
};