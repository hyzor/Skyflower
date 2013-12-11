#ifndef SKYFLOWER_CHART_H
#define SKYFLOWER_CHART_H

#include <string>

#include "shared/platform.h"
#include <skia/SkCanvas.h>
#include <skia/SkBitmap.h>

#include <skia/SkImageEncoder.h>

struct LineChartDataPoint
{
	float timeStamp;
	float value;
};

class LineChart
{
public:
	LineChart(size_t maximumDataPoints);
	virtual ~LineChart();

	void SetSize(unsigned int width, unsigned int height);
	void SetUnit(const std::string &unit);

	void AddPoint(float timeStamp, float value);
	void Clear();

	void Draw(float startTime, float endTime, float resolution, float targetValue);

private:
	SkCanvas *m_canvas;
	SkBitmap *m_bitmap;

	struct LineChartDataPoint *m_dataPoints;
	size_t m_dataPointCapacity;
	size_t m_dataPointStart;
	size_t m_dataPointEnd;

	std::string m_unit;



	SkImageEncoder *m_imageEncoder;
};

#endif
