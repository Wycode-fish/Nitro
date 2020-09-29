#include "../include/Frustum.h"

namespace Nitro
{
    namespace Physics
    {
        Frustum::Frustum()
        {
        }

        Frustum::Frustum(const Matrix4& projMat)
        {
            const float* matPtr = (const float*)&projMat;

            // reciprocals of diagnal elements
            const float reciprocalOfElementXX = 1.0f / matPtr[0 * 4 + 0];
            const float reciprocalOfElementYY = 1.0f / matPtr[1 * 4 + 1];
            const float reciprocalOfElementZZ = 1.0f / matPtr[2 * 4 + 2];

            if (matPtr[0 * 4 + 3] == 0.0f
                && matPtr[1 * 4 + 3] == 0.0f
                && matPtr[2 * 4 + 3] == 0.0f
                && matPtr[3 * 4 + 3] == 1.0f)
            {
                // @ -------------------------- Orthogonal Projection --------------------------
                float left = (-1.0f - matPtr[3 * 4 + 0]) * reciprocalOfElementXX;
                float right = (1.0f - matPtr[3 * 4 + 0]) * reciprocalOfElementXX;
                float top = (1.0f - matPtr[3 * 4 + 1]) * reciprocalOfElementYY;
                float bottom = (-1.0f - matPtr[3 * 4 + 1]) * reciprocalOfElementYY;
                float front = (0.0f - matPtr[3 * 4 + 2]) * reciprocalOfElementZZ;
                float back = (1.0f - matPtr[3 * 4 + 2]) * reciprocalOfElementZZ;

                if (front < back)
                {
                    InitializeFrustum_Orthographics(left, right, top, bottom, front, back);
                }
                else
                {
                    InitializeFrustum_Orthographics(left, right, top, bottom, back, front);
                }
            }
            else
            {
                // @ -------------------------- Perspective Projection --------------------------
                float near, far;
                // @ zz : -(f+n)/(f-n) => hence, if zz > 0.0f => f < n, else f > n
                if (reciprocalOfElementZZ > 0.0f)
                {
                    // By solving equations:
                    //      1. -(f+n)/(f-n) = matPtr[2 * 4 + 2];
                    //      2. -2*f*n/(f-n) = matPtr[3 * 4 + 2];
                    // we can get *n* and *f*, where n = f * (1+zz)/(zz-1), so if:
                    //      - zz > 0.f, the order of the near/far plane will be:
                    //          ---- 0 ---- far ---- near ---->
                    //                      OR
                    //          ---- near ---- far ---- 0 ---->
                    //          where the *near* becomes the *far*.
                    near = matPtr[3 * 4 + 2] / (1 + matPtr[2 * 4 + 2]);
                    far = matPtr[3 * 4 + 2] / (matPtr[2 * 4 + 2] - 1); // Here different than ModelViewer example!
                }
                else
                {
                    far = matPtr[3 * 4 + 2] / (1 + matPtr[2 * 4 + 2]);
                    near = matPtr[3 * 4 + 2] / (matPtr[2 * 4 + 2] - 1);
                }

                InitializeFrustum_Perspective(reciprocalOfElementXX, reciprocalOfElementYY, near, far);
            }
        }

        bool Frustum::IntersectSphere(BoundingSphere sphere) const
        {
            float radius = sphere.GetRadius();
            for (u32 i = 0; i < 6; ++i)
            {
                if (this->m_FrustumPlanes[i].DistanceFromPoint(sphere.GetCenter()) < -radius)
                {
                    return false;
                }
            }
            return true;
        }

        bool Frustum::IntersectBoundingBox(const BoundingBox& box) const
        {
            Vector3 minBound = box.GetMinBound(), maxBound = box.GetMaxBound();
            for (u32 i = 0; i < 6; ++i)
            {
                BoundingPlane currPlane = this->m_FrustumPlanes[i];
                Vector3 closeCorner = Math::Select(minBound, maxBound, currPlane.GetNormal() < Vector3(kZero));
                if (currPlane.DistanceFromPoint(closeCorner) < 0.0f)
                {
                    return false;
                }
            }
            return true;
        }

        void Frustum::InitializeFrustum_Orthographics(f32 left, f32 right, f32 top, f32 bottom, f32 front, f32 back)
        {
            // Note that: left < 0, bottom < 0, back > front > 0
            this->m_FrustumCorners[(u32)CornerID::kNearLowerLeft] = Vector3 (left, bottom, -front);
            this->m_FrustumCorners[(u32)CornerID::kNearLowerRight] = Vector3(right, bottom, -front);
            this->m_FrustumCorners[(u32)CornerID::kNearUpperLeft] = Vector3 (left, top, -front);
            this->m_FrustumCorners[(u32)CornerID::kNearUpperRight] = Vector3(right, top, -front);
            this->m_FrustumCorners[(u32)CornerID::kFarLowerLeft] = Vector3(left, bottom, -back);
            this->m_FrustumCorners[(u32)CornerID::kFarLowerRight] = Vector3(right, bottom, -back);
            this->m_FrustumCorners[(u32)CornerID::kFarUpperLeft] = Vector3(left, top, -back);
            this->m_FrustumCorners[(u32)CornerID::kFarUpperRight] = Vector3(right, top, -back);

            this->m_FrustumPlanes[(u32)PlaneID::kNearPlane] = BoundingPlane(0.0f, 0.0f, -1.0f, -front);
            this->m_FrustumPlanes[(u32)PlaneID::kFarPlane] = BoundingPlane(0.0f, 0.0f, 1.0f, back);
            this->m_FrustumPlanes[(u32)PlaneID::kLeftPlane] = BoundingPlane(1.0f, 0.0f, 0.0f, -left);
            this->m_FrustumPlanes[(u32)PlaneID::kRightPlane] = BoundingPlane(-1.0f, 0.0f, 0.0f, right);
            this->m_FrustumPlanes[(u32)PlaneID::kTopPlane] = BoundingPlane(0.0f, -1.0f, 0.0f, top); // Here different than ModelViewer example!
            this->m_FrustumPlanes[(u32)PlaneID::kBottomPlane] = BoundingPlane(0.0f, 1.0f, 0.0f, -bottom);
        }

        void Frustum::InitializeFrustum_Perspective(f32 horizontalTangent, f32 verticalTangent, f32 nearClip, f32 farClip)
        {
            const f32 nearX = nearClip * horizontalTangent
                , nearY = nearClip * verticalTangent
                , farX = farClip * horizontalTangent
                , farY = farClip * verticalTangent;

            this->m_FrustumCorners[(u32)CornerID::kNearLowerLeft] = Vector3(-nearX, -nearY, -nearClip);
            this->m_FrustumCorners[(u32)CornerID::kNearLowerRight] = Vector3(nearX, -nearY, -nearClip);
            this->m_FrustumCorners[(u32)CornerID::kNearUpperLeft] = Vector3(-nearX, nearY, -nearClip);
            this->m_FrustumCorners[(u32)CornerID::kNearUpperRight] = Vector3(nearX, nearY, -nearClip);

            this->m_FrustumCorners[(u32)CornerID::kFarLowerLeft] = Vector3(-nearX, -nearY, -farClip);
            this->m_FrustumCorners[(u32)CornerID::kFarLowerRight] = Vector3(nearX, -nearY, -farClip);
            this->m_FrustumCorners[(u32)CornerID::kFarUpperLeft] = Vector3(-nearX, nearY, -farClip);
            this->m_FrustumCorners[(u32)CornerID::kFarUpperRight] = Vector3(nearX, nearY, -farClip);

            f32 normalHorizontalX = Math::RecipSqrt(1 + horizontalTangent * horizontalTangent);  // = cos(theta)
            f32 normalHorizontalZ = -1.0f * normalHorizontalX * horizontalTangent; // = -1 * cos * tan
            f32 normalVerticalY = Math::RecipSqrt(1 + verticalTangent * verticalTangent);
            f32 normalVerticalZ = -1.0f * normalVerticalY * verticalTangent;

            // @ Frustum has a converge point at origin, which means all 4 edges will converge at origion
            this->m_FrustumPlanes[(u32)PlaneID::kNearPlane] = BoundingPlane(0.0f, 0.0f, -1.0f, -nearClip);
            this->m_FrustumPlanes[(u32)PlaneID::kFarPlane] = BoundingPlane(0.0f, 0.0f, 1.0f, farClip);
            this->m_FrustumPlanes[(u32)PlaneID::kLeftPlane] = BoundingPlane(normalHorizontalX, 0.0f, normalHorizontalZ, 0.0f);
            this->m_FrustumPlanes[(u32)PlaneID::kRightPlane] = BoundingPlane(-normalHorizontalX, 0.0f, normalHorizontalZ, 0.0f);
            this->m_FrustumPlanes[(u32)PlaneID::kTopPlane] = BoundingPlane(0.0f, -normalVerticalY, normalVerticalZ, 0.0f);
            this->m_FrustumPlanes[(u32)PlaneID::kBottomPlane] = BoundingPlane(0.0f, normalVerticalY, normalVerticalZ, 0.0f);
        }
    }
}
