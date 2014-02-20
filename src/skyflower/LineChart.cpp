#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>
#include <sstream>

#include <DirectXMath.h>

#include "LineChart.h"

// Must be included last!
#include "shared/debug.h"

static size_t NextPowerOfTwo(size_t n)
{
	size_t k = 1;

	while (k < n) {
		k *= 2;
	}

	return k;
}

static double lerp(double x, double y, double a)
{
	assert(a >= 0.0 && a <= 1.0);

	return x * (1.0 - a) + y * a;
}

LineChartData::LineChartData(size_t maximumDataPoints)
{
	assert(maximumDataPoints > 0);

	m_capacity = NextPowerOfTwo(maximumDataPoints);
	m_buffer = new struct LineChartDataPoint[m_capacity];
	m_start = 0;
	m_end = 0;
}

LineChartData::~LineChartData()
{
	delete[] m_buffer;
}

bool LineChartData::IsFull() const
{
	return m_end == (m_start ^ m_capacity);
}

bool LineChartData::IsEmpty() const
{
	return m_end == m_start;
}

void LineChartData::AddDataPoint(double timeStamp, double value)
{
	// assert dataPoint.timeStamp > m_dataPoints[m_dataPointEnd - 1].timeStamp?

	m_buffer[m_end & (m_capacity - 1)].timeStamp = timeStamp;
	m_buffer[m_end & (m_capacity - 1)].value = value;

	if (IsFull()) {
		m_start = Increment(m_start);
	}

	m_end = Increment(m_end);
}

void LineChartData::ClearDataPoints()
{
	m_start = 0;
	m_end = 0;
}

void LineChartData::PushDataPoints(double startTime, double endTime, double resolution, std::vector<const struct LineChartDataPoint> &output) const
{
	assert(output.size() == 0);

	if (IsEmpty()) {
		return;
	}

	const struct LineChartDataPoint *dataPoint;
	// Floor the endTime so we can get a stable selection of data points through time (if resolution is appropriately selected).
	double nextSelection = ceil(endTime);

	size_t capacity = output.capacity();

	for (size_t i = Decrement(m_end); i != m_start; i = Decrement(i)) {
		dataPoint = &m_buffer[i & (m_capacity - 1)];

		if (dataPoint->timeStamp < startTime) {
 			break;
		}

		if (dataPoint->timeStamp <= nextSelection) {
			output.push_back(*dataPoint);
			nextSelection -= resolution;
		}
	}

	if (output.capacity() != capacity) {
		printf("new capacity is %i, old capacity was %i\n", output.capacity(), capacity);
	}
}

size_t LineChartData::Increment(size_t n) const
{
	return (n + 1) & (2 * m_capacity - 1);
}

size_t LineChartData::Decrement(size_t n) const
{
	return (n - 1) & (2 * m_capacity - 1);
}

LineChartRendererD3D::LineChartRendererD3D(GraphicsEngine *graphicsEngine)
{
	assert(graphicsEngine);

	m_graphicsEngine = graphicsEngine;

	m_bufferSize = 0;
	m_buffer = NULL;

	m_targetValue = 0.0;
	m_unit = "";
	m_label = "";
}

LineChartRendererD3D::~LineChartRendererD3D()
{
	if (m_buffer)
		delete[] m_buffer;
}

void LineChartRendererD3D::SetTargetValue(double targetValue)
{
	m_targetValue = targetValue;
}

void LineChartRendererD3D::SetUnit(const std::string &unit)
{
	m_unit = unit;
}

void LineChartRendererD3D::SetLabel(const std::string &label)
{
	m_label = label;
}

void LineChartRendererD3D::Draw(const std::vector<const struct LineChartDataPoint> &dataPoints, double startTime, Texture2D *texture)
{
	assert(texture->IsRenderable());

	float backgroundColor[4] = {0.0f, 0.0f, 0.0f, 0.5f};
	m_graphicsEngine->ClearTexture(texture, backgroundColor);

	// FIXME: Print m_label. (We have no way to set text alignment)

	// Only draw the background and label if we don't have any points to render.
	if (dataPoints.size() == 0) {
		return;
	}

	if (!m_buffer || m_bufferSize < dataPoints.size() * 2) {
		if (m_buffer)
			delete[] m_buffer;

		m_bufferSize = (size_t)((dataPoints.size() * 2) * 1.5f);
		m_buffer = new float[m_bufferSize];
	}

	unsigned int width = texture->GetWidth();
	unsigned int height = texture->GetHeight();

	//double startTime = dataPoints.back().timeStamp;
	double endTime = dataPoints.front().timeStamp;
	double timeSpan = endTime - startTime;

	double maxValue = DBL_MIN;
	double minValue = DBL_MAX;
	double totalValue = 0.0;
	int count = 0;

	const struct LineChartDataPoint *point;
	double x, y;

	for (auto iter = dataPoints.begin(); iter != dataPoints.end(); iter++) {
		point = &(*iter);
		x = ((point->timeStamp - startTime) / timeSpan) * width;
		y = point->value;

		m_buffer[count * 2 + 0] = (float)x;
		m_buffer[count * 2 + 1] = (float)y;

		maxValue = std::max(maxValue, point->value);
		minValue = std::min(minValue, point->value);
		
		count++;
		totalValue += point->value;
	}

	assert(count > 0);

	//printf("drawing %d samples, maxValue=%.1f, minValue=%.1f, lastValue=%.1f\n", count, maxValue, minValue, value);

	// Add some padding to the top and bottom of the chart.
	double drawMax = std::max(maxValue, m_targetValue);
	//double drawMin = std::min(minValue, targetValue);
	double drawMin = 0.0;
	double padding = (drawMax - drawMin) * 0.1;
	//double drawRange = (drawMax + padding) + (drawMin - padding);
	double drawRange = drawMax + padding;

	// Scale the path to fit the canvas.
	double yScale = height / drawRange;
	double pixelPadding = height * -(yScale - 1.0);

	XMMATRIX transformation = XMMatrixMultiply(XMMatrixTranslation(0.0f, (float)pixelPadding, 0.0f), XMMatrixScaling(1.0f, (float)yScale, 1.0f));
	XMFLOAT3X3 transformationMatrix;
	XMStoreFloat3x3(&transformationMatrix, transformation);

	// Draw the target line.
	float targetLine[4] = {0.0f, (float)m_targetValue, (float)width, (float)m_targetValue};
	float lineColor[4] = {1.0f, 0.47f, 0.0f, 1.0f};
	m_graphicsEngine->DrawLines(texture, targetLine, 2, transformationMatrix, lineColor);

	// Draw the data points.
	lineColor[0] = 0.0f;
	lineColor[1] = 0.8f;
	lineColor[2] = 1.0f;
	lineColor[3] = 1.0f;
	m_graphicsEngine->DrawLines(texture, m_buffer, count, transformationMatrix, lineColor);

	// Draw the labels
	std::stringstream stringStream;
	stringStream.precision(1);

	//stringStream << std::fixed << m_targetValue << m_unit;
	//std::string targetValueString = stringStream.str();

	stringStream.str("");
	stringStream << "Min:" << std::fixed << minValue << m_unit;
	std::string minValueString = stringStream.str();

	stringStream.str("");
	stringStream << "Max:" << std::fixed << maxValue << m_unit;
	std::string maxValueString = stringStream.str();

	stringStream.str("");
	stringStream << "Avg:" << std::fixed << (totalValue / count) << m_unit;
	std::string averageValueString = stringStream.str();

	stringStream.str("");
	stringStream << "Cur:" << std::fixed << dataPoints.front().value << m_unit;
	std::string currentValueString = stringStream.str();

	int textPadding = 10;
	int textPosition[2] = {0, 0};

	m_graphicsEngine->Begin2D();

	m_graphicsEngine->PrintTextMonospaceToTexture(texture, currentValueString, textPosition);
	textPosition[1] += textPadding;
	m_graphicsEngine->PrintTextMonospaceToTexture(texture, averageValueString, textPosition);
	textPosition[1] += textPadding;
	m_graphicsEngine->PrintTextMonospaceToTexture(texture, minValueString, textPosition);
	textPosition[1] += textPadding;
	m_graphicsEngine->PrintTextMonospaceToTexture(texture, maxValueString, textPosition);

	m_graphicsEngine->End2D();
}
