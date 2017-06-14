// Upgrade NOTE: replaced '_Object2World' with 'unity_ObjectToWorld'
// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "GalaxyRider/Flat Ground"
{
    Properties
    {
        _ColorA ("Color A", Color) = (1,0,0,1)
        _ColorB ("Color B", Color) = (0,1,0,1)
    }
    SubShader
    {
        Lighting Off
        ZWrite On
        // Tags {"Queue"="Transparent" "IgnoreProjector"="True" "RenderType"="Transparent"}
        // Blend SrcAlpha OneMinusSrcAlpha

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            uniform float4 _ColorA;
            uniform float4 _ColorB;

            struct v2f {
                float4 pos : SV_POSITION;
                float3 wpos : TEXCOORD1;
                float2 uv : TEXCOORD0;
            };

            v2f vert(appdata_img v) {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.wpos = mul(unity_ObjectToWorld, v.vertex).xyz;
                o.uv = v.texcoord.xy;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target {
                fixed surfDist = 1-(i.uv.x-1)*(i.uv.x-1);
                return surfDist < 0.999 ? _ColorA : _ColorB;
            }
            ENDCG
        }
    }
}
