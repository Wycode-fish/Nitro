#pragma once

#include "NitroConfig_Physics.h"
#include "BoundingPlane.h"
#include "BoundingSphere.h"
#include "BoundingBox.h"

namespace Nitro
{
	namespace Physics
	{
		class Frustum
		{
		public:
			enum class CornerID
			{
				kNearLowerLeft, kNearUpperLeft, kNearLowerRight, kNearUpperRight,
				kFarLowerLeft, kFarUpperLeft, kFarLowerRight, kFarUpperRight
			};
			enum class PlaneID
			{
				kNearPlane, kFarPlane,
				kLeftPlane, kRightPlane,
				kTopPlane,	kBottomPlane
			};

			Frustum();
			Frustum(const Matrix4& projMat);

			INLINE Vector3			GetFrustumCorner(Frustum::CornerID cid) const	{ return this->m_FrustumCorners[(u32)cid]; }
			INLINE BoundingPlane	GetFrustumPlane(Frustum::PlaneID pid)	const	{ return this->m_FrustumPlanes[(u32)pid]; }

			bool IntersectSphere(BoundingSphere sphere) const;
			bool IntersectBoundingBox(const BoundingBox& box) const;

			// Projection Matrix: http://www.songho.ca/opengl/gl_projectionmatrix.html
			void InitializeFrustum_Orthographics(f32 left, f32 right, f32 top, f32 bottom, f32 front, f32 back);
			void InitializeFrustum_Perspective(f32 horizontalTangent, f32 verticalTangent, f32 nearClip, f32 farClip);

			friend Frustum operator*(const OrthogonalTransform& tf, const Frustum& frustum);
			friend Frustum operator*(const Matrix4& mat_tf, const Frustum& frustum);

		private:
			Vector3			m_FrustumCorners[8];
			BoundingPlane	m_FrustumPlanes[6];
		};

		INLINE Frustum operator*(const OrthogonalTransform& tf, const Frustum& frustum)
		{
			Frustum res;
			for (u32 i = 0; i < 8; ++i)
			{
				res.m_FrustumCorners[i] = tf * frustum.m_FrustumCorners[i];
			}
			for (u32 i = 0; i < 6; ++i)
			{
				res.m_FrustumPlanes[i] = tf * frustum.m_FrustumPlanes[i];
			}
			return res;
		}

		INLINE Frustum operator*(const Matrix4& mat_tf, const Frustum& frustum)
		{
			Frustum res;
			for (u32 i = 0; i < 8; ++i)
			{
				res.m_FrustumCorners[i] = Vector3(mat_tf * frustum.m_FrustumCorners[i]);
			}
			Matrix4 invertedAndTranspose = Math::Transpose(Math::Invert(mat_tf));
			for (u32 i = 0; i < 6; ++i)
			{
				// Though BoundingPlane has already overloaded the * operator to automatically transform input 
				// matrix M into ((M)^(-1))^(T) when trying to apply matrix to plane, here in order to reduce 
				// the repeated matrix operation, we do the invert+transpose once and apply it directly to 
				// vec4 instead of use the * operator of bounding plane.
				res.m_FrustumPlanes[i] = BoundingPlane(invertedAndTranspose * Vector4(frustum.m_FrustumPlanes[i]));
			}
			return res;
		}
	}
}