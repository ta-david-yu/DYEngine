#include "Math/AABB.h"

namespace DYE::Math
{
	bool AABB::Contains(glm::vec3 point) const
	{
		if (point[0] < Min[0] || point[0] > Max[0]) return false;
		if (point[1] < Min[1] || point[1] > Max[1]) return false;
		if (point[2] < Min[2] || point[2] > Max[2]) return false;
		return true;
	}

	bool AABB::Contains2D(glm::vec2 point) const
	{
		if (point[0] < Min[0] || point[0] > Max[0]) return false;
		if (point[1] < Min[1] || point[1] > Max[1]) return false;
		return true;
	}

	glm::vec3 AABB::ClosestPoint(glm::vec3 point) const
	{
		for (int i = 0; i < 3; i++)
		{
			float& component = point[i];
			component = glm::max(component, Min[i]);
			component = glm::min(component, Max[i]);
		}

		return point;
	}

	glm::vec2 AABB::ClosestPoint2D(glm::vec2 point) const
	{
		for (int i = 0; i < 2; i++)
		{
			float& component = point[i];
			component = glm::max(component, Min[i]);
			component = glm::min(component, Max[i]);
		}

		return point;
	}

	float AABB::SqrDistance(glm::vec3 point) const
	{
		float sqrDistance = 0;
		for (int i = 0; i < 3; i++)
		{
			float const component = point[i];
			if (component < Min[i]) sqrDistance += (Min[i] - component) * (Min[i] - component);
			if (component > Max[i]) sqrDistance += (component - Max[i]) * (component - Max[i]);
		}
		return sqrDistance;
	}

	float AABB::SqrDistance2D(glm::vec2 point) const
	{
		float sqrDistance = 0;
		for (int i = 0; i < 2; i++)
		{
			float const component = point[i];
			if (component < Min[i]) sqrDistance += (Min[i] - component) * (Min[i] - component);
			if (component > Max[i]) sqrDistance += (component - Max[i]) * (component - Max[i]);
		}
		return sqrDistance;
	}
}