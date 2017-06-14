// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "Custom/Camera"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _LavaTex ("Lava", 2D) = "white" {}
    }
    SubShader
    {
        Cull Off ZWrite Off ZTest Always

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"


// ---- HSL space transform stuff ---------------------------------------

    static float3x3 matrixH =
    {
        0.8164966f,  0,           0.5352037f,
        -0.4082483f,  0.70710677f, 1.0548190f,
        -0.4082483f, -0.70710677f, 0.1420281f
    };

    static float3x3 matrixH2 =
    {
        0.84630f, -0.37844f, -0.37844f,
        -0.37265f,  0.33446f, -1.07975f,
        0.57735f,  0.57735f,  0.57735f
    };

    // Hue :         0..360, default 0
    // Saturation :  0..2,   default 1
    // Luminosity : -1..1,   default 0
    float4 hslTransform(float4 c, float Hue, float Saturation, float Luminosity) {
        float3x3 rotateZ = {
            cos(radians(Hue)), sin(radians(Hue)), 0,
            -sin(radians(Hue)), cos(radians(Hue)), 0,
            0, 0, 1
        };
        matrixH = mul(matrixH, rotateZ);
        matrixH = mul(matrixH, matrixH2);

        float i = 1 - Saturation;
        float3x3 matrixS = {
            i*0.3086f+Saturation, i*0.3086f, i*0.3086f,
            i*0.6094f, i*0.6094f+Saturation, i*0.6094f,
            i*0.0820f, i*0.0820f, i*0.0820f+Saturation
        };
        matrixH = mul(matrixH, matrixS);

        float3 c1 = mul(c, matrixH);
        c1 += Luminosity;
        return float4(c1, c.a);
    }

// ----------------------------------------------------------------------
            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            sampler2D _MainTex;
            sampler2D _LavaTex;

            fixed4 frag (v2f i) : SV_Target
            {
                fixed4 col = tex2D(_MainTex, i.uv);
                fixed4 col2 = tex2D(_LavaTex, i.uv);
                return hslTransform(col, col2.r*180, 1, 0.5*col2.r);
            }
            ENDCG
        }
    }
}
