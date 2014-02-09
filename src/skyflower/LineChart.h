#ifndef SKYFLOWER_CHART_H
#define SKYFLOWER_CHART_H

#include <string>

#include "shared/platform.h"
#include <skia/SkCanvas.h>
#include <skia/SkBitmap.h>

struct LineChartDataPoint
{
	double timeStamp;
	double value;
};

class LineChart
{
public:
	LineChart(size_t maximumDataPoints);
	virtual ~LineChart();

	void SetSize(unsigned int width, unsigned int height);
	void SetTimeSpan(double timeSpan, double resolution);
	void SetTargetValue(double targetValue);
	void SetUnit(const std::string &unit);
	void SetLabel(const std::string &label);

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	const void *GetPixels() const;

	void AddDataPoint(double timeStamp, double value);
	void ClearDataPoints();

	void Draw(double time);

private:
	SkCanvas *m_canvas;
	SkBitmap *m_bitmap;

	double m_timeSpan;
	double m_resolution;
	double m_targetValue;

	std::string m_unit;
	std::string m_label;

	// Circular buffer for all data points.
	struct LineChartDataPoint *m_dataPoints;
	size_t m_dataPointCapacity;
	size_t m_dataPointStart;
	size_t m_dataPointEnd;

	// Circular buffer for data points to be rendered.
	struct LineChartDataPoint *m_renderDataPoints;
	size_t m_renderDataPointCapacity;
	size_t m_renderDataPointStart;
	size_t m_renderDataPointEnd;
};

#endif
