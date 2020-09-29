#include "../include/BoundingPlane.h"

namespace Nitro
{
	namespace Physics
	{
		BoundingPlane BoundingPlane::CreateFrom3PtsCCW(Vector3 a, Vector3 b, Vector3 c)
		{
			return BoundingPlane(a, Math::Cross(b-a, c-a));
		}
		BoundingPlane::BoundingPlane()
			: m_Repr()
		{}
		BoundingPlane::BoundingPlane(Vector3 normal2Plane, float distanceFromOrigin)
			: m_Repr(normal2Plane, distanceFromOrigin)
		{}
		BoundingPlane::BoundingPlane(Vector3 point, Vector3 normal2Plane)
		{
			// @ though the normal we store in Plane struct could be un-normalized, 
			// @ but in order to calculate the distance from plane to origin, dist = |point| * |n| * cos(theta)
			// @ we need to normalize the plane normal to make sure |n|=1
			Vector3 normalizedNormal = Math::Normalize(normal2Plane);
			this->m_Repr = Vector4(normalizedNormal, -Math::Dot(normalizedNormal, point));
		}
		BoundingPlane::BoundingPlane(float a, float b, float c, float d)
			: m_Repr(a, b, c, d)
		{}
		BoundingPlane::BoundingPlane(const BoundingPlane& plane)
			: m_Repr(plane.m_Repr)
		{}
		BoundingPlane::BoundingPlane(Vector4 planeRepr)
			: m_Repr(planeRepr)
		{}

		Vector3 BoundingPlane::GetNormal() const
		{
			return Vector3(this->m_Repr);
		}
		Vector3 BoundingPlane::GetClosestPointToOrigin() const
		{
			return -this->GetNormal() * (this->m_Repr.GetW());
		}

		Scalar BoundingPlane::DistanceFromPoint(Vector3 point) const
		{
			// This is the same as Dot(Vector4(point, 1), m_Repr)
			return this->m_Repr.GetW() + Math::Dot(point, this->GetNormal());
		}

		Scalar BoundingPlane::DistanceFromPoint(Vector4 point) const
		{
			return Math::Dot(point, this->m_Repr);
		}
	}
}