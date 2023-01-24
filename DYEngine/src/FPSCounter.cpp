#include "Util/FPSCounter.h"

namespace DYE
{
	bool FPSCounter::NewFrame(double timeStep)
	{
		m_FramesCounter++;
		m_TimeStepAccumulator += timeStep;
		if (m_TimeStepAccumulator >= m_SampleInterval)
		{
			m_LastCalculatedFPS = (double) m_FramesCounter / m_TimeStepAccumulator;
			m_FramesCounter = 0;
			m_TimeStepAccumulator = 0;
			return true;
		}

		return false;
	}
}