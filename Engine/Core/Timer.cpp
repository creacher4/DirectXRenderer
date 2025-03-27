#include "Timer.h"

Timer::Timer()
    : isRunning(false), deltaTime(0.0f), totalTime(0.0f),
      frameCount(0), timeElapsed(0.0f), fps(0)
{
    currTime = std::chrono::high_resolution_clock::now();
    prevTime = currTime;
    baseTime = currTime;
    pausedTime = std::chrono::high_resolution_clock::duration::zero();
}

void Timer::Reset()
{
    auto now = std::chrono::high_resolution_clock::now();
    baseTime = now;
    prevTime = now;
    pausedTime = std::chrono::high_resolution_clock::duration::zero();
    frameCount = 0;
    timeElapsed = 0.0f;
    fps = 0;
}

void Timer::Start()
{
    if (!isRunning)
    {
        auto now = std::chrono::high_resolution_clock::now();
        pausedTime += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(now - prevTime);
        prevTime = now;
        isRunning = true;
    }
}

void Timer::Stop()
{
    if (isRunning)
    {
        auto now = std::chrono::high_resolution_clock::now();
        prevTime = now;
        isRunning = false;
    }
}

void Timer::Tick()
{
    if (!isRunning)
    {
        deltaTime = 0.0f;
        return;
    }

    currTime = std::chrono::high_resolution_clock::now();

    deltaTime = std::chrono::duration<float>(currTime - prevTime).count();

    if (deltaTime > 0.25f)
        deltaTime = 0.25f;

    prevTime = currTime;

    auto runningTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(currTime - baseTime) - pausedTime;
    totalTime = std::chrono::duration<float>(runningTime).count();

    frameCount++;
    timeElapsed += deltaTime;

    if (timeElapsed >= 1.0f)
    {
        fps = frameCount;
        frameCount = 0;
        timeElapsed = 0.0f;
    }
}

float Timer::GetDeltaTime() const
{
    return deltaTime;
}

float Timer::GetTotalTime() const
{
    return totalTime;
}

int Timer::GetFPS() const
{
    return fps;
}