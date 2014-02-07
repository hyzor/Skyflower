#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>
#include <sstream>

#include "shared/platform.h"
#include <skia/SkRect.h>
#include <skia/SkCanvas.h>
#include <skia/SkBitmap.h>
#include <skia/SkTypeface.h>
#include <skia/SkXfermode.h>

#include "LineChart.h"

// Must be included last!
#include "shared/debug.h"

static double lerp(double x, double y, double a)
{
	assert(a >= 0.0 && a <= 1.0);

	return x * (1.0 - a) + y * a;
}

LineChart::LineChart(size_t maximumDataPoints)
{
	m_bitmap = NULL;
	m_canvas = NULL;

	m_timeSpan = 30.0;
	m_resolution = 1.0 / 60.0;
	m_targetValue = 0.0;

	m_unit = "";
	m_label = "";

	m_dataPoints = new struct LineChartDataPoint[maximumDataPoints];
	m_dataPointCapacity = maximumDataPoints;
	m_dataPointStart = 0;
	m_dataPointEnd = 0;

	m_renderDataPoints = NULL;
}

LineChart::~LineChart()
{
	delete[] m_dataPoints;

	if (m_renderDataPoints)
		delete[] m_renderDataPoints;

	if (m_canvas)
		delete m_canvas;
	if (m_bitmap)
		delete m_bitmap;
}

void LineChart::SetSize(unsigned int width, unsigned int height)
{
	if (m_canvas)
		delete m_canvas;
	if (m_bitmap)
		delete m_bitmap;

	m_bitmap = new SkBitmap();
	m_bitmap->setConfig(SkBitmap::kARGB_8888_Config, width, height);
	m_bitmap->allocPixels();

	m_canvas = new SkCanvas(*m_bitmap);
}

void LineChart::SetTimeSpan(double timeSpan, double resolution)
{
	assert(timeSpan > 0.0);
	assert(resolution > 0.0);

	m_timeSpan = timeSpan;
	m_resolution = resolution;

	if (m_renderDataPoints)
		delete[] m_renderDataPoints;

	size_t size = (size_t)ceil(m_timeSpan / m_resolution);
	m_renderDataPoints = new struct LineChartDataPoint[size];
	m_renderDataPointCapacity = size;
	m_renderDataPointStart = 0;
	m_renderDataPointEnd = 0;

#if 0
	// Fill m_renderDataPoints with points to render.
	const struct LineChartDataPoint *previousRenderDataPoint = NULL;
	const struct LineChartDataPoint *previousDataPoint = NULL;
	double difference, alpha, value;

	for (size_t i = m_dataPointStart; i != m_dataPointEnd; i = (i + 1) % m_dataPointCapacity) {
		if (m_dataPoints[i].timeStamp < m_dataPoints[m_dataPointEnd].timeStamp - timeSpan) {
			continue;
		}
		if (previousRenderDataPoint && m_dataPoints[i].timeStamp < previousRenderDataPoint->timeStamp + m_resolution) {
			continue;
		}

		value = m_dataPoints[i].value;

		if (i != m_dataPointStart) {
			previousDataPoint = &m_dataPoints[(i == 0? m_dataPointCapacity - 1 : i - 1)];
		}

		if (previousDataPoint) {
			// Interpolate the value.
			difference = m_dataPoints[i].timeStamp - previousDataPoint->timeStamp;
			alpha = ((previousRenderDataPoint->timeStamp + m_resolution) - previousDataPoint->timeStamp) / (difference + DBL_EPSILON);
			value = lerp(previousDataPoint->value, m_dataPoints[i].value, alpha);
		}

		m_renderDataPoints[m_renderDataPointEnd].timeStamp = (previousRenderDataPoint? previousRenderDataPoint->timeStamp + m_resolution : m_dataPoints[i].timeStamp);
		m_renderDataPoints[m_renderDataPointEnd].value = value;

		previousRenderDataPoint = &m_renderDataPoints[m_renderDataPointEnd];

		m_renderDataPointEnd = (m_renderDataPointEnd + 1) % m_renderDataPointCapacity;

		if (m_renderDataPointEnd == m_renderDataPointStart) {
			m_renderDataPointStart = (m_renderDataPointStart + 1) % m_renderDataPointCapacity;
		}
	}
#endif
}

void LineChart::SetTargetValue(double targetValue)
{
	m_targetValue = targetValue;
}

void LineChart::SetUnit(const std::string &unit)
{
	m_unit = unit;
}

void LineChart::SetLabel(const std::string &label)
{
	m_label = label;
}

unsigned int LineChart::GetWidth() const
{
	assert(m_bitmap);

	return (unsigned int)m_bitmap->width();
}

unsigned int LineChart::GetHeight() const
{
	assert(m_bitmap);

	return (unsigned int)m_bitmap->height();
}

const void *LineChart::GetPixels() const
{
	assert(m_bitmap);

	return m_bitmap->getPixels();
}

void LineChart::AddDataPoint(double timeStamp, double value)
{
	const struct LineChartDataPoint *previousDataPoint = NULL;

	if (m_dataPointStart != m_dataPointEnd) {
		previousDataPoint = &m_dataPoints[(m_dataPointEnd == 0? m_dataPointCapacity - 1 : m_dataPointEnd - 1)];
	}

	m_dataPoints[m_dataPointEnd].timeStamp = timeStamp;
	m_dataPoints[m_dataPointEnd].value = value;

	m_dataPointEnd = (m_dataPointEnd + 1) % m_dataPointCapacity;

	if (m_dataPointEnd == m_dataPointStart) {
		m_dataPointStart = (m_dataPointStart + 1) % m_dataPointCapacity;
	}

	// Add the data point to the render set if needed.
	size_t previousRenderDataPointIndex = (m_renderDataPointEnd == 0? m_renderDataPointCapacity - 1 : m_renderDataPointEnd - 1);
	double pointTimeStamp, pointValue;
	//double difference, alpha;

	// FIXME: We might need to insert several render data points.

	pointTimeStamp = (m_renderDataPointEnd == m_renderDataPointStart? timeStamp : m_renderDataPoints[previousRenderDataPointIndex].timeStamp + m_resolution);

	if (timeStamp >= pointTimeStamp) {
		pointValue = value;

#if 0
		if (previousDataPoint) {
			// Interpolate the value.
			difference = timeStamp - previousDataPoint->timeStamp;
			alpha = (pointTimeStamp - previousDataPoint->timeStamp) / (difference + DBL_EPSILON);
			pointValue = lerp(previousDataPoint->value, value, alpha);
		}
#endif

		m_renderDataPoints[m_renderDataPointEnd].timeStamp = pointTimeStamp;
		m_renderDataPoints[m_renderDataPointEnd].value = pointValue;

		m_renderDataPointEnd = (m_renderDataPointEnd + 1) % m_renderDataPointCapacity;

		if (m_renderDataPointEnd == m_renderDataPointStart) {
			m_renderDataPointStart = (m_renderDataPointStart + 1) % m_renderDataPointCapacity;
		}
	}
}

void LineChart::ClearDataPoints()
{
	m_dataPointStart = 0;
	m_dataPointEnd = 0;

	m_renderDataPointStart = 0;
	m_renderDataPointEnd = 0;
}

void LineChart::Draw(double time)
{
	assert(m_canvas);
	assert(m_bitmap);
	assert(m_renderDataPoints);

	// Clear the canvas with transparency.
	m_canvas->clear(SK_ColorTRANSPARENT);

	// Draw a semi-transparent background.
	SkPaint backgroundPaint;
	backgroundPaint.setColor(SkColorSetARGB(128, 0, 0, 0));
	backgroundPaint.setStyle(SkPaint::kFill_Style);
	backgroundPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	m_canvas->drawRect(SkRect::MakeXYWH(0.0f, 0.0f, (SkScalar)m_bitmap->width(), (SkScalar)m_bitmap->height()), backgroundPaint);

	SkPaint textPaint;
	textPaint.setColor(SkColorSetARGB(255, 255, 255, 255));
	textPaint.setAntiAlias(true);
	textPaint.setSubpixelText(true);
	textPaint.setLCDRenderText(true);
	textPaint.setTextSize(6.0f);
	textPaint.setTextAlign(SkPaint::kLeft_Align);

	SkTypeface *typeface = SkTypeface::CreateFromName("courier", SkTypeface::kNormal);
	textPaint.setTypeface(typeface);
	typeface->unref();

	// Draw the label.
	textPaint.setTextAlign(SkPaint::kRight_Align);
	m_canvas->drawText(m_label.c_str(), m_label.length(), (SkScalar)m_bitmap->width(), 9.0f, textPaint);

	// Only draw the background and label if we don't have any points to render.
	if (m_renderDataPointStart == m_renderDataPointEnd) {
		return;
	}

	double startTime = time - m_timeSpan;

	const struct LineChartDataPoint *point;
	SkPath path;

	double maxValue = DBL_MIN;
	double minValue = DBL_MAX;
	double totalValue = 0.0;
	int count = 0;

	double position;

	for (size_t i = m_renderDataPointStart; i != m_renderDataPointEnd; i = (i + 1) % m_renderDataPointCapacity) {
		point = &m_renderDataPoints[i];
		position = ((point->timeStamp - startTime) / m_timeSpan) * m_bitmap->width();

		assert(point->value > 0.0);

		if (i == m_renderDataPointStart) {
			path.moveTo((SkScalar)position, (SkScalar)(m_bitmap->height() - point->value));
		}
		else {
			path.lineTo((SkScalar)position, (SkScalar)(m_bitmap->height() - point->value));
		}

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
	double scale = m_bitmap->height() / drawRange;
	double pixelPadding = m_bitmap->height() * -(scale - 1.0);
	SkMatrix transformation;
	transformation.setScale(1.0f, (SkScalar)scale);
	transformation.postTranslate(0.0f, (SkScalar)pixelPadding);
	path.transform(transformation);

	double targetLinePosition = (m_bitmap->height() - m_targetValue) * scale + pixelPadding;

	// Draw the target value line.
	SkPaint linePaint;
	linePaint.setColor(SkColorSetARGB(255, 255, 133, 27));
	linePaint.setStyle(SkPaint::kStroke_Style);
	linePaint.setStrokeWidth(1.0f);
	//linePaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	m_canvas->drawLine(0.0f, (SkScalar)targetLinePosition, (SkScalar)m_bitmap->width(), (SkScalar)targetLinePosition, linePaint);

	// Draw the actual data point lines.
	SkPaint pathPaint;
	pathPaint.setColor(SkColorSetARGB(255, 127, 219, 255));
	pathPaint.setStyle(SkPaint::kStroke_Style);
	pathPaint.setStrokeWidth(1.0f);
	//pathPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	pathPaint.setAntiAlias(true);
	m_canvas->drawPath(path, pathPaint);

	// Draw the labels
	std::stringstream stringStream;
	stringStream.precision(1);
	stringStream << std::fixed << m_targetValue << m_unit;
	std::string targetValueString = stringStream.str();

	stringStream.str("");
	stringStream << std::fixed << drawMin - padding << m_unit;
	std::string lowValueString = stringStream.str();

	stringStream.str("");
	stringStream << std::fixed << drawMax + padding << m_unit;
	std::string highValueString = stringStream.str();

	stringStream.str("");
	stringStream << "Min:" << std::fixed << minValue << m_unit;
	std::string minValueString = stringStream.str();

	stringStream.str("");
	stringStream << "Max:" << std::fixed << maxValue << m_unit;
	std::string maxValueString = stringStream.str();

	stringStream.str("");
	stringStream << "Avg:" << std::fixed << (totalValue / count) << m_unit;
	std::string averageValueString = stringStream.str();

	size_t lastRenderDataPointIndex = (m_renderDataPointEnd == 0? m_renderDataPointCapacity - 1 : m_renderDataPointEnd - 1);

	stringStream.str("");
	stringStream << "Cur:" << std::fixed << m_renderDataPoints[lastRenderDataPointIndex].value << m_unit;
	std::string currentValueString = stringStream.str();

	// Draw the labels.
	textPaint.setTextAlign(SkPaint::kLeft_Align);

	float textPadding = 10.0f;
	float textPosition = textPadding;
	m_canvas->drawText(currentValueString.c_str(), currentValueString.length(), 0.0f, textPosition, textPaint);
	textPosition += textPadding;
	m_canvas->drawText(averageValueString.c_str(), averageValueString.length(), 0.0f, textPosition, textPaint);
	textPosition += textPadding;
	m_canvas->drawText(minValueString.c_str(), minValueString.length(), 0.0f, textPosition, textPaint);
	textPosition += textPadding;
	m_canvas->drawText(maxValueString.c_str(), maxValueString.length(), 0.0f, textPosition, textPaint);

	textPaint.setTextAlign(SkPaint::kRight_Align);

	// Draw the target label at the end of the chart just above the target line.
	//m_canvas->drawText(targetValueString.c_str(), targetValueString.length(), (float)m_bitmap->width(), (float)(targetLinePosition - (textPadding / 2.0)), textPaint);

	/*
	// Draw the high and low values.
	m_canvas->drawText(highValueString.c_str(), highValueString.length(), (SkScalar)m_bitmap->width(), 9.0f, textPaint);
	m_canvas->drawText(lowValueString.c_str(), lowValueString.length(), (SkScalar)m_bitmap->width(), m_bitmap->height() - 1.0f, textPaint);
	*/
}
