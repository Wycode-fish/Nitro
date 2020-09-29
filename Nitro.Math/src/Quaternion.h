#pragma once

#include "NativeOps.h"
#include "Vector.h"

namespace Nitro
{
	namespace Math
	{
		// @ Quaternion: Q=(qvec, q0), where qvec=sin(theta/2)*uvec, and q0=cos(theta/2)
		// @	- __m128 layout:		[qvec.x, qvec.y, qvec.z, q0]
		// @
		// @	- rotation detail: operator LQ(v)=QvQ*, where qvec=sin(theta/2)*uvec, and q0=cos(theta/2)
		// @		- LQ(v) equals rotate v couner-clockwise <around> uvec <by> angle theta 
		// @
		// @	- conjugate(~): Q*=(-qvec, q0)
		// @		- LQ*(v) equals rotate v counter-clockwise <around> uvec <by> angle (-theta).
		// @			opposite of rotation effect done by Q
		// @	
		// @	- quaternion multiply and rotation apply:
		// @		- http://graphics.stanford.edu/courses/cs348a-17-winter/Papers/quaternion.pdf
		class Quaternion
		{
		public:
			INLINE Quaternion() { m_vec = XMQuaternionIdentity(); }	// Q-identity: theta=0
			INLINE Quaternion(const Vector3& axis, const Scalar& angle) { m_vec = XMQuaternionRotationAxis(axis, angle); }
			INLINE Quaternion(float pitch, float yaw, float roll) { m_vec = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }
			INLINE explicit Quaternion(const FMAT4& matrix) { m_vec = XMQuaternionRotationMatrix(matrix); }
			INLINE explicit Quaternion(const M128 vec) { m_vec = vec; }
			INLINE explicit Quaternion(EIdentityTag) { m_vec = XMQuaternionIdentity(); }
			
			INLINE operator M128() const { return m_vec; }
			INLINE Quaternion operator~(void) const { return Quaternion(XMQuaternionConjugate(m_vec)); }
			INLINE Quaternion operator-(void) const { return Quaternion(XMVectorNegate(m_vec)); }
			
			// @ XMQuaternionMultiply: 
			// @	https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmquaternionmultiply
			// @ e.g. XMQuaternionMultiply(q1, q2) => return q2 * q1
			INLINE Quaternion operator*(Quaternion rhs) const { return Quaternion(XMQuaternionMultiply(rhs, m_vec)); }
			INLINE Vector3 operator*(Vector3 rhs) const { return Vector3(XMVector3Rotate(rhs, m_vec)); }
			
			INLINE Quaternion& operator=(Quaternion rhs) { m_vec = rhs; return *this; }
			INLINE Quaternion& operator*=(Quaternion rhs) { *this = *this * rhs; return *this; }
		protected:
			M128 m_vec;
		};
	}
}