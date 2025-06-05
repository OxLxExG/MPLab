#ifndef _ADXL356_HPP_
#define _ADXL356_HPP_

#include <tools.h>
#include <stdlib.h>


#if !defined(ADXL356)
# error "Global define ADXL356=ax,ay,az,t"
#endif

typedef struct __attribute__((packed))
{
	float T;
	float X,Y,Z;
} adx356_adc_t;

template <int8_t chx, int8_t chy, int8_t chz, uint8_t cht>
class adxl365_t
 {
	private:
		int64_t sum[4];
		uint16_t cnt; 
	public:
	void SummData(int32_t* data)
	{
		sum[0] += data[cht];
		sum[1] += data[abs(chx)];
		sum[2] += data[abs(chy)];
		sum[3] += data[abs(chz)];
		cnt++;
	}
	void ResetSumm(adx356_adc_t* txyz)
	{
		if (cnt == 0) return;
		txyz->T = (((sum[0]/cnt) >> 8)  - 21422)/78.64;
		sum[0] = 0;
		txyz->X = (sum[1]/cnt) >> 6;
		//txyz->X += 76700;
		if (chx<0)
		{
			txyz->X = -txyz->X;
		}
		sum[1] = 0;
		txyz->Y = (sum[2]/cnt) >> 6;
		//txyz->Y += 76700;
		if (chy<0)
		{
			txyz->Y = -txyz->Y;
		}
		sum[2] = 0;
		txyz->Z = (sum[3]/cnt) >> 6;
		//txyz->Z += 76700;
		if (chz<0)
		{
			txyz->Z = -txyz->Z;
		}
		sum[3] = 0;
		cnt =0;
	}	
};
#endif