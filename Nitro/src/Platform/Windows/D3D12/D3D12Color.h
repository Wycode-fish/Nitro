#pragma once
#include <DirectXMath.h>

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
            using namespace DirectX;

            class Color
            {
            public:
                Color() : m_value(g_XMOne) {}
                Color(FXMVECTOR vec);
                Color(const XMVECTORF32& vec);
                Color(float r, float g, float b, float a = 1.0f);
                Color(uint16_t r, uint16_t g, uint16_t b, uint16_t a = 255, uint16_t bitDepth = 8);
                explicit Color(uint32_t rgbaLittleEndian);

                float R() const { return XMVectorGetX(m_value); }
                float G() const { return XMVectorGetY(m_value); }
                float B() const { return XMVectorGetZ(m_value); }
                float A() const { return XMVectorGetW(m_value); }

                bool operator==(const Color& rhs) const { return XMVector4Equal(m_value, rhs.m_value); }
                bool operator!=(const Color& rhs) const { return !XMVector4Equal(m_value, rhs.m_value); }

                void SetR(float r) { m_value.f[0] = r; }
                void SetG(float g) { m_value.f[1] = g; }
                void SetB(float b) { m_value.f[2] = b; }
                void SetA(float a) { m_value.f[3] = a; }

                float* GetPtr(void) { return reinterpret_cast<float*>(this); }
                float& operator[](int idx) { return GetPtr()[idx]; }

                void SetRGB(float r, float g, float b) { m_value.v = XMVectorSelect(m_value, XMVectorSet(r, g, b, b), g_XMMask3); }
#ifdef COLOR_FULLY_READY
                Color ToSRGB() const;
                Color FromSRGB() const;
                Color ToREC709() const;
                Color FromREC709() const;

                // Probably want to convert to sRGB or Rec709 first
                uint32_t R10G10B10A2() const;
                uint32_t R8G8B8A8() const;

                // Pack an HDR color into 32-bits
                uint32_t R11G11B10F(bool RoundToEven = false) const;
                uint32_t R9G9B9E5() const;
#endif
                operator XMVECTOR() const { return m_value; }

            private:
                XMVECTORF32 m_value;
            };

            // @ -----------------------------------------------------------------------------------
            // @                                Implementation
            // @ -----------------------------------------------------------------------------------
            inline Color::Color(FXMVECTOR vec)
            {
                m_value.v = vec;
            }

            inline Color::Color(const XMVECTORF32& vec)
            {
                m_value = vec;
            }

            inline Color::Color(float r, float g, float b, float a)
            {
                m_value.v = XMVectorSet(r, g, b, a);
            }

            inline Color::Color(uint16_t r, uint16_t g, uint16_t b, uint16_t a, uint16_t bitDepth)
            {
                m_value.v = XMVectorScale(XMVectorSet(r, g, b, a), 1.0f / ((1 << bitDepth) - 1));
            }

            inline Color::Color(uint32_t u32)
            {
                float r = (float)((u32 >> 0) & 0xFF);
                float g = (float)((u32 >> 8) & 0xFF);
                float b = (float)((u32 >> 16) & 0xFF);
                float a = (float)((u32 >> 24) & 0xFF);
                m_value.v = XMVectorScale(XMVectorSet(r, g, b, a), 1.0f / 255.0f);
            }

		}
	}
}