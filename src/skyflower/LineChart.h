#ifndef SKYFLOWER_CHART_H
#define SKYFLOWER_CHART_H

#include <string>
#include <vector>

#include "graphics/GraphicsEngine.h"

struct LineChartDataPoint
{
	double timeStamp;
	double value;
};

class LineChartData
{
public:
	LineChartData(size_t maximumDataPoints);
	virtual ~LineChartData();

	bool IsFull() const;
	bool IsEmpty() const;

	void AddDataPoint(double timeStamp, double value);
	void ClearDataPoints();

	// The newest data points are pushed first.
	void PushDataPoints(double startTime, double endTime, double resolution, std::vector<const struct LineChartDataPoint> &output) const;

private:
	size_t Increment(size_t n) const;
	size_t Decrement(size_t n) const;

private:
	// Mirrored circular buffer for all data points.
	struct LineChartDataPoint *m_buffer;
	size_t m_capacity;
	size_t m_start;
	size_t m_end;
};

class LineChartRendererD3D
{
public:
	LineChartRendererD3D(GraphicsEngine *graphicsEngine);
	virtual ~LineChartRendererD3D();

	void SetTargetValue(double targetValue);
	void SetUnit(const std::string &unit);
	void SetLabel(const std::string &label);

	void Draw(const std::vector<const struct LineChartDataPoint> &dataPoints, Texture2D *texture);

private:
	GraphicsEngine *m_graphicsEngine;

	size_t m_bufferSize;
	float *m_buffer;

	double m_targetValue;
	std::string m_unit;
	std::string m_label;
};

#endif
