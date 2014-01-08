#include <algorithm>
#include <cassert>
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

static float lerp(float x, float y, float a)
{
	return x * (1.0f - a) + y * a;
}

LineChart::LineChart(size_t maximumDataPoints)
{
	m_bitmap = NULL;
	m_canvas = NULL;

	m_dataPoints = new struct LineChartDataPoint[maximumDataPoints];
	m_dataPointCapacity = maximumDataPoints;
	m_dataPointStart = 0;
	m_dataPointEnd = 0;

	m_unit = "";
}

LineChart::~LineChart()
{
	delete[] m_dataPoints;

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

void LineChart::SetUnit(const std::string &unit)
{
	m_unit = unit;
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

void *LineChart::GetPixels() const
{
	assert(m_bitmap);

	return m_bitmap->getPixels();
}

void LineChart::AddPoint(float timeStamp, float value)
{
	m_dataPoints[m_dataPointEnd].timeStamp = timeStamp;
	m_dataPoints[m_dataPointEnd].value = value;

	m_dataPointEnd = (m_dataPointEnd + 1) % m_dataPointCapacity;

	if (m_dataPointEnd == m_dataPointStart) {
		m_dataPointStart = (m_dataPointStart + 1) % m_dataPointCapacity;
	}
}

void LineChart::ClearPoints()
{
	m_dataPointStart = 0;
	m_dataPointEnd = 0;
}

void LineChart::Draw(float startTime, float endTime, float resolution, float targetValue)
{
	assert(m_canvas);
	assert(m_bitmap);
	assert(endTime > startTime);
	assert(resolution > 0.0f);

	SkPath path;
	const struct LineChartDataPoint *point;
	const struct LineChartDataPoint *previousPoint;
	float timeRange = endTime - startTime;
	float nextTimeStamp = startTime;
	float value, alpha, difference, position;
	float maxValue = FLT_MIN;
	float minValue = FLT_MAX;
	double totalValue = 0.0;
	size_t previousIndex;
	int count = 0;
	bool first = true;

	for (size_t i = m_dataPointStart; i != m_dataPointEnd; i = (i + 1) % m_dataPointCapacity) {
		point = &m_dataPoints[i];

		if (point->timeStamp >= startTime && point->timeStamp <= endTime && point->timeStamp >= nextTimeStamp) {
			if (first) {
				nextTimeStamp = point->timeStamp;
				value = point->value;
			}
			else {
				previousIndex = (i - 1 < 0? m_dataPointCapacity - 1 : i - 1);
				previousPoint = &m_dataPoints[previousIndex];

				difference = point->timeStamp - previousPoint->timeStamp;

				if (difference > 0.0) {
					alpha = (nextTimeStamp - previousPoint->timeStamp) / difference;
					value = lerp(previousPoint->value, point->value, alpha);
				}
				else {
					value = point->value;
				}
			}

			position = ((nextTimeStamp - startTime) / timeRange) * m_bitmap->width();

			if (first) {
				first = false;

				path.moveTo(position, (float)m_bitmap->height() - value);
			}
			else {
				path.lineTo(position, (float)m_bitmap->height() - value);
			}

			maxValue = std::max(maxValue, value);
			minValue = std::min(minValue, value);

			count++;
			totalValue += value;
			nextTimeStamp = point->timeStamp + resolution;

			if (nextTimeStamp > endTime) {
				break;
			}
		}
	}

	//printf("drawing %d samples, maxValue=%.1f, minValue=%.1f, lastValue=%.1f\n", count, maxValue, minValue, value);

	// Add some padding to the top and bottom of the chart.
	float drawMax = std::max(maxValue, targetValue);
	float drawMin = std::min(minValue, targetValue);
	float padding = (drawMax - drawMin) * 0.1f;
	float drawRange = (drawMax + padding) + (drawMin - padding);

	// Scale the path to fit the canvas.
	float scale = m_bitmap->height() / drawRange;
	SkMatrix transformation;
	transformation.setScale(1.0f, scale);
	transformation.postTranslate(0.0f, m_bitmap->height() * -(scale - 1.0f));
	path.transform(transformation);

	// Clear the canvas with transparency.
	m_canvas->clear(SK_ColorTRANSPARENT);

	// Draw a semi-transparent background.
	SkPaint backgroundPaint;
	backgroundPaint.setColor(SkColorSetARGB(128, 0, 0, 0));
	backgroundPaint.setStyle(SkPaint::kFill_Style);
	backgroundPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	m_canvas->drawRect(SkRect::MakeXYWH(0.0f, 0.0f, (float)m_bitmap->width(), (float)m_bitmap->height()), backgroundPaint);

	float targetLinePosition = ((float)m_bitmap->height() - targetValue) * scale + (m_bitmap->height() * -(scale - 1.0f));

	// Draw the target value line.
	SkPaint linePaint;
	linePaint.setColor(SkColorSetARGB(255, 255, 133, 27));
	linePaint.setStyle(SkPaint::kStroke_Style);
	linePaint.setStrokeWidth(1.0f);
	//linePaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	m_canvas->drawLine(0.0f, targetLinePosition, (float)m_bitmap->width(), targetLinePosition, linePaint);

	// Draw the actual data point lines.
	SkPaint pathPaint;
	pathPaint.setColor(SkColorSetARGB(255, 127, 219, 255));
	pathPaint.setStyle(SkPaint::kStroke_Style);
	pathPaint.setStrokeWidth(1.0f);
	//pathPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	pathPaint.setAntiAlias(true);
	m_canvas->drawPath(path, pathPaint);

	// Draw the labels
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

	std::stringstream stringStream;
	stringStream.precision(1);
	stringStream << std::fixed << targetValue << m_unit;
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

	stringStream.str("");
	stringStream << "Cur:" << std::fixed << value << m_unit;
	std::string currentValueString = stringStream.str();

	// Draw the labels.
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
	//m_canvas->drawText(targetValueString.c_str(), targetValueString.length(), (float)m_bitmap->width(), targetLinePosition - (textPadding / 2.0f), textPaint);

	// Draw the high and low values.
	m_canvas->drawText(highValueString.c_str(), highValueString.length(), (float)m_bitmap->width(), 9.0f, textPaint);
	m_canvas->drawText(lowValueString.c_str(), lowValueString.length(), (float)m_bitmap->width(), m_bitmap->height() - 1.0f, textPaint);
}
