#pragma once

#include <cstdint>


namespace DYE
{
    class Application;
    class Time
    {
            friend class Application;
        public:
            Time() = delete;
            Time(const Time&) = delete;

            explicit Time(uint32_t fps) : m_FPS(fps), m_LastTicks(0), m_TicksSinceStart(0), m_DeltaTicks(0) { }

            /// The time in second that has passed since the last frame
            /// \return m_DeltaTicks / 1000.0
            double DeltaTime() const { return m_DeltaTicks / 1000.0; }

            /// A fixed delta time in second for Physics Simulation, const value
            /// \return 1 / m_Fps
            double FixedDeltaTime() const { return 1.0 / m_FPS; }
        private:
            /// The number of frames per second, used to determine FixedDeltaTime()
            uint32_t m_FPS;

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