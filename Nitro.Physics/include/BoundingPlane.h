#pragma once

#include "NitroConfig_Physics.h"
#include "Transform.h"

namespace Nitro
{
	using namespace Math;

	namespace Physics
	{
		// @ Plane is represented in Hesse Normal Form
		// @	https://en.wikipedia.org/wiki/Hesse_normal_form
		// @	- m_Repr = (normal.x, normal.y, normal.z, distanceFromOrigin);
		// @		where plane normal points to origin inwards, and distanceFromOrigin is positive
		class BoundingPlane
		{
		public:
			static BoundingPlane CreateFrom3PtsCCW(Vector3 a, Vector3 b, Vector3 c);
		public:
			BoundingPlane();
			BoundingPlane(Vector3 normal2Plane, float distanceFromOrigin);
			BoundingPlane(Vector3 point, Vector3 normal2Plane);
			BoundingPlane(float a, float b, float c, float d);
			BoundingPlane(const BoundingPlane& plane);
			explicit BoundingPlane(Vector4 planeRepr);

			INLINE operator Vector4() const { return m_Repr; }
			
			Vector3 GetNormal() const;
			Vector3 GetClosestPointToOrigin() const;
			
			// @ *Distance* here has direction:
			// @	- if point's *below* plane, to get to the plane, the distance pt-2-plane is +
			// @	- if point's *above* plane, to get to the plane, the distance pt-2-plane is -
			Scalar DistanceFromPoint(Vector3 point) const;
			Scalar DistanceFromPoint(Vector4 point) const;

			friend BoundingPlane operator*(const OrthogonalTransform& tf, BoundingPlane plane);
			friend BoundingPlane operator*(const Matrix4& mat_tf, BoundingPlane plane);
		private:
			Vector4 m_Repr;
		};

		INLINE BoundingPlane operator*(const OrthogonalTransform& tf, BoundingPlane plane)
		{
			Vector3 transformedNormal = tf.Rotation * plane.GetNormal();
			float distanceFromOrigin = plane.m_Repr.GetW() - Dot(transformedNormal, tf.Translation);
			return BoundingPlane(transformedNormal, distanceFromOrigin);
		}

		// @ transform mat is of form:
		// @   [s r r 0
		// @	r s r 0
		// @	r r s 0
		// @	t t t 1]
		// @ --------------------
		// @	So when (MatTF * [PlaneNormal(vec3) | Dist2Origin(scalar)]), the result vec4's W component
		// @	would be res[3] = [t, t, t, 1] * [PlaneNormal, Dist2Origin]
		// @					= Dot(Vector3(t, t, t), PlaneNormal) + 1 * Dist2Origin
		// @					= translation_offset + Dist2Origin = new_Dist2Origin
		INLINE BoundingPlane operator*(const Matrix4& mat_tf, BoundingPlane plane)
		{
			return BoundingPlane(Math::Transpose(Math::Invert(mat_tf)) * plane.m_Repr);
		}
	}
}