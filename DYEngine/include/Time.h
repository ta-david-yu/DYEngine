#pragma once

#include <cstdint>

#define TIME Time::GetInstance()

namespace DYE
{
    class Application;

    class Time
    {
        friend class Application;
    public:
        static Time& GetInstance() { return *s_pInstance; }

        Time() = delete;

        Time(const Time &) = delete;

        /// The time in second that has passed since the last frame
        /// \return m_DeltaTicks / 1000.0
        double DeltaTime() const { return m_DeltaTicks / 1000.0; }

        /// A fixed delta time in second for Physics Simulation, const value
        /// \return 1 / m_Fps
        double FixedDeltaTime() const { return 1.0 / m_FixedFramePerSecond; }

        uint32_t FixedFramePerSecond() const { return m_FixedFramePerSecond; }

    private:
        static Time* s_pInstance;

        /// The created Time instance is assigned as the singleton instance
        /// \param fixedFps The number of frames per second in FixedUpdate event, used to determine FixedDeltaTime()
        explicit Time(uint32_t fixedFps) : m_FixedFramePerSecond(fixedFps), m_LastTicks(0), m_TicksSinceStart(0), m_DeltaTicks(0)
        {
            s_pInstance = this;
        }

        /// The number of frames per second, used to determine FixedDeltaTime()
        uint32_t m_FixedFramePerSecond;

        /// The ticks when the last tickUpdate() or tickInit() is called
        uint32_t m_LastTicks;

        /// The number of ticks since tickInit is called
        uint32_t m_TicksSinceStart;

        /// Get updated everytime tickUpdate() is called
        uint32_t m_DeltaTicks;

        /// Initialize tick variable, called at the start of the game loop
        void tickInit();

        /// Update deltaTime, called at the end of each frame
        void tickUpdate();
    };
}