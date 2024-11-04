#pragma once

namespace DYE
{
    class FPSCounter
    {
    private:
        double m_SampleInterval = 0.25f;
        double m_TimeStepAccumulator = 0;
        long long int m_FramesCounter = 0;
        double m_LastCalculatedFPS = 0;

    public:
        FPSCounter() = delete;
        explicit FPSCounter(double sampleInterval) : m_SampleInterval(sampleInterval) {}

        inline double GetLastCalculatedFPS() const { return m_LastCalculatedFPS; }

        /// Call this every frame and pass in the delta time for that frame.
        /// \param timeStep
        /// \return true if FPS has been updated after this call, else false.
        bool NewFrame(double timeStep);
    };
}