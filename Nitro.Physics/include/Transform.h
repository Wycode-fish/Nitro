#pragma once

#include "NitroConfig_Physics.h"

namespace Nitro
{
	namespace Physics
	{
		NT_ALIGN struct OrthogonalTransform
		{
#pragma region Member Vars
			Quaternion	Rotation;
			Vector3		Translation;
#pragma endregion

			static INLINE OrthogonalTransform MakeXRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kXUnitVector), angle)); }
			static INLINE OrthogonalTransform MakeYRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kYUnitVector), angle)); }
			static INLINE OrthogonalTransform MakeZRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kZUnitVector), angle)); }
			static INLINE OrthogonalTransform MakeTranslation(Vector3 translate) { return OrthogonalTransform(translate); }

			INLINE OrthogonalTransform() : Rotation(kIdentity), Translation(kZero) {}
			INLINE OrthogonalTransform(EIdentityTag) : Rotation(kIdentity), Translation(kZero) {}
			INLINE OrthogonalTransform(Quaternion rotate) : Rotation(rotate), Translation(kZero) {}
			INLINE OrthogonalTransform(Vector3 translation) : Rotation(kIdentity), Translation(translation) {}
			INLINE OrthogonalTransform(Quaternion rotate, Vector3 translate) : Rotation(rotate), Translation(translate) {}
			INLINE OrthogonalTransform(const Matrix3& rotate_mat) : Rotation(rotate_mat), Translation(kZero) {}
			INLINE OrthogonalTransform(const Matrix3& rotate_mat, Vector3 translate) : Rotation(rotate_mat), Translation(translate) {}
			INLINE explicit OrthogonalTransform(const FMAT4& tf_mat) { *this = OrthogonalTransform(Matrix3(tf_mat), Vector3(tf_mat.r[3])); }

			INLINE Vector3 operator*(Vector3 vec) const { return this->Rotation * vec + this->Translation; }
			// @ No matter input vec is a point or a directional vector, rotation needs to be applied. But translation 
			// @ should get applied depends on whether it's a point or direction (w == 1?)
			INLINE Vector4 operator*(Vector4 vec) const 
			{ 
				M128 rawAfterRotation = Math::SetWToZero(this->Rotation * Vector3((M128)vec));
				return Vector4(rawAfterRotation) + Vector4(Math::SetWToOne(this->Translation)) * vec.GetW();
			}
			INLINE OrthogonalTransform operator*(const OrthogonalTransform& tf) const
			{
				return OrthogonalTransform(this->Rotation * tf.Rotation, this->Rotation * tf.Translation + this->Translation);
			}
			INLINE OrthogonalTransform operator~() const
			{
				Quaternion invertedRotation = ~this->Rotation;
				return OrthogonalTransform(invertedRotation, invertedRotation * (-this->Translation));
			}
		};

		INLINE OrthogonalTransform Invert(const OrthogonalTransform& xform) { return ~xform; }
		// This specialized matrix invert assumes that the 3x3 matrix is orthogonal (and normalized).
		// INLINE AffineTransform OrthoInvert(const AffineTransform& xform)
		// {
		// 	Matrix3 basis = Transpose(xform.GetBasis());
		// 	return AffineTransform(basis, basis * -xform.GetTranslation());
		// }
	}
}