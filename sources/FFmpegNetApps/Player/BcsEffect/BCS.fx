float brightness : register(C0);
float contrast   : register(C1);
float saturation : register(C2);

sampler2D implicitInputSampler : register(S0);

static float4 Grey50 = float4(0.5f, 0.5f, 0.5f, 0.5f);
static float4 Coeff  = float4(0.2125f, 0.7154f, 0.0721f, 0.0f);


float4 main(float2 uv : TEXCOORD) : COLOR
{
   // Get the source color
   float4 color = tex2D(implicitInputSampler, uv);

   color = color + brightness;
   color.a = 1.0f;

   color = lerp(Grey50, color, contrast);
   color = saturate(color);
   color.a = 1.0f;

   float  grey  = mul(Coeff, color);
   float4 grey4 = float4(grey, grey, grey, 1.0f);
   color = lerp(grey4, color, saturation);
   color = saturate(color);
   color.a = 1.0f;

   // Return new color
   return color;
}