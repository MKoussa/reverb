#include "userrevfx.h"
#include "buffer_ops.h"

#define BUFFER_LEN 24000
#define BUFFER_LEN_HALF 12000

static __sdram float s_reverb_ram[BUFFER_LEN];

static uint8_t timeChange, depthChange; 
static float depth, depthVal, depthDiv, wetDry, wetDryDiv, reverbL, reverbR;
static uint32_t echoCount, echoMax, echoMaxVal, reverbFill;

void REVFX_INIT(uint32_t platform, uint32_t api)
{
  buf_clr_f32(s_reverb_ram, BUFFER_LEN);

  depth = 0.0f;
  echoCount = 50;
  wetDry = 0.0f;
  echoMax = BUFFER_LEN_HALF - 1;
  //
  depthDiv = 0.0f;
  wetDryDiv = 0.0f;
  //
  depthVal = 0;
  echoMaxVal = 0;
  //
  timeChange = 0;
  depthChange = 0;
  //
  reverbL = 0.0f;
  reverbR = 0.0f;
  //
  reverbFill;
}

void REVFX_PROCESS(float *xn, uint32_t frames)
{
  depthDiv = 0.99f + depth;
  wetDryDiv = 1.0f + wetDry;
  
  for(uint32_t i = 0; i < frames; i++)
  {
    s_reverb_ram[reverbFill * 2]     = xn[i * 2];
    s_reverb_ram[reverbFill * 2 + 1] = xn[i * 2 + 1];

    if(wetDry > 0)
    {
      for(uint16_t j = 0; j < 1250;j += 10)
      {
        reverbL += s_reverb_ram[(echoCount * 2)     - j];
        reverbR += s_reverb_ram[(echoCount * 2 + 1) - j];
      }
      reverbL =  reverbL / 125.0f;
      reverbR =  reverbR / 125.0f;

      xn[i * 2]     = (xn[i * 2]     + (reverbL * wetDry)) / (wetDryDiv);
      xn[i * 2 + 1] = (xn[i * 2 + 1] + (reverbR * wetDry)) / (wetDryDiv);
    }
    else
    {
      xn[i * 2]     = xn[i * 2];
      xn[i * 2 + 1] = xn[i * 2 + 1];
    }

    reverbFill++;
    echoCount++;
    if(echoCount > echoMax) { echoCount = 620; }
    if(reverbFill > BUFFER_LEN_HALF - 1) { reverbFill = 0; }
  }
}

void REVFX_PARAM(uint8_t index, int32_t value)
{
  const float valf = q31_to_f32(value);
  switch (index) 
  {
    case k_user_revfx_param_time:
      break;
    case k_user_revfx_param_depth:
      break;
    case k_user_revfx_param_shift_depth:
      wetDry = valf;
      break;
    default:
      break;
  }
}
