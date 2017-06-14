// Upgrade NOTE: replaced '_Object2World' with 'unity_ObjectToWorld'
// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "GalaxyRider/Ground"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _NoiseTex ("Noise", 2D) = "white" {}
        _Density ("Density", Float) = 1
        _NoiseDensity ("Noise Density", Float) = 1
        _HueSpin ("Hue Spin", Range(0,360)) = 0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }

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
                float4 vertex : SV_POSITION;
                float3 wpos : TEXCOORD1;
                float2 uv : TEXCOORD0;
            };

            float4 _MainTex_ST;

            sampler2D _MainTex;
            sampler2D _NoiseTex;
            float _Density;
            float _NoiseDensity;
            float _HueSpin;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.wpos = mul(unity_ObjectToWorld, v.vertex).xyz;
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                fixed4 tex = tex2D(_MainTex, i.wpos.xy * _Density);
                fixed noi = tex2D(_NoiseTex, i.wpos.xy * _NoiseDensity).r;

                fixed surfDist = 1-(i.uv.x-1)*(i.uv.x-1);

                return hslTransform(tex, _HueSpin, .5*noi + 1+(1-surfDist), -surfDist/2);
            }
            ENDCG
        }
    }
}
