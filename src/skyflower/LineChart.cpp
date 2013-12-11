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

LineChart::LineChart(size_t maximumDataPoints)
{
	m_bitmap = NULL;
	m_canvas = NULL;

	m_dataPoints = new struct LineChartDataPoint[maximumDataPoints];
	m_dataPointCapacity = maximumDataPoints;
	m_dataPointStart = 0;
	m_dataPointEnd = 0;

	m_unit = "";



	m_imageEncoder = SkImageEncoder::Create(SkImageEncoder::kPNG_Type);
}

LineChart::~LineChart()
{
	delete m_imageEncoder;


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
	// FIXME: Replace with setPixels.
	m_bitmap->allocPixels();

	m_canvas = new SkCanvas(*m_bitmap);
}

void LineChart::SetUnit(const std::string &unit)
{
	m_unit = unit;
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

void LineChart::Clear()
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
	float step = m_bitmap->width() / ((endTime - startTime) * (1.0f / resolution));
	float maxValue = FLT_MIN;
	float minValue = FLT_MAX;
	float lastTimeStamp = -10.0f; //FLT_MIN;
	float timeStamp, value;
	float position = 0.0f;
	bool first = true;

	int totalCount = 0;
	int count = 0;

	for (size_t i = m_dataPointStart; i != m_dataPointEnd; i = (i + 1) % m_dataPointCapacity) {
		totalCount++;

		timeStamp = m_dataPoints[i].timeStamp;

		// FIXME: Interpolate instead of skipping samples failing (timeStamp - lastTimeStamp) >= resolution
		if (timeStamp >= startTime && timeStamp <= endTime && (timeStamp - lastTimeStamp) >= resolution) {
			count++;

			lastTimeStamp = timeStamp;
			value = m_dataPoints[i].value;

			if (first) {
				first = false;

				path.moveTo(position, (float)m_bitmap->height() - value);
			}
			else {
				path.lineTo(position, (float)m_bitmap->height() - value);
				// cubicTo
			}

			position += step;

			maxValue = std::max(maxValue, value);
			minValue = std::min(minValue, value);
		}
	}

	//printf("drawing %d samples out of %d, maxValue=%.1f, minValue=%.1f, lastValue=%.1f\n", count, totalCount, maxValue, minValue, value);

	// Make sure the line is drawn right to the end of the chart.
	path.lineTo((float)m_bitmap->width(), (float)m_bitmap->height() - value);

	maxValue = std::max(maxValue, targetValue);
	minValue = std::min(minValue, targetValue);

	// Add some extra space at the top and bottom of the chart.
	float range = maxValue - minValue;
	maxValue += range * 0.1f;
	minValue -= range * 0.1f;

	// FIXME: Allow charts that don't start at zero.
	minValue = 0.0f;
	range = maxValue - minValue;

	// Clear the canvas with transparency.
	m_canvas->clear(SK_ColorTRANSPARENT);

	SkPaint backgroundPaint;
	backgroundPaint.setColor(SkColorSetARGB(128, 175, 175, 175));
	backgroundPaint.setStyle(SkPaint::kFill_Style);
	backgroundPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);

	// Draw a semi-transparent background.
	m_canvas->drawRect(SkRect::MakeXYWH(0.0f, 0.0f, (float)m_bitmap->width(), (float)m_bitmap->height()), backgroundPaint);

	SkPaint pathPaint;
	pathPaint.setColor(SkColorSetARGB(255, 255, 115, 0));
	pathPaint.setStyle(SkPaint::kStroke_Style);
	pathPaint.setStrokeWidth(1.0f);
	//pathPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	pathPaint.setAntiAlias(true);

	// Scale the path to fit the canvas.
	float scale = m_bitmap->height() / range;
	SkMatrix transformation;
	transformation.setScale(1.0f, scale);
	transformation.postTranslate(0.0f, m_bitmap->height() * -(scale - 1.0f));
	path.transform(transformation);

	// Draw the actual data point lines.
	m_canvas->drawPath(path, pathPaint);

	SkPaint linePaint;
	linePaint.setColor(SkColorSetARGB(255, 0, 0, 0));
	linePaint.setStyle(SkPaint::kStroke_Style);
	linePaint.setStrokeWidth(1.0f);
	//linePaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);

	// Draw the target value line.
	float targetLinePosition = ((float)m_bitmap->height() - targetValue) * scale + (m_bitmap->height() * -(scale - 1.0f));
	m_canvas->drawLine(0.0f, targetLinePosition, (float)m_bitmap->width(), targetLinePosition, linePaint);

	// Draw the labels
	SkPaint textPaint;
	textPaint.setAntiAlias(true);
	//textPaint.setSubpixelText(true);
	//textPaint.setLCDRenderText(true);
	textPaint.setTextAlign(SkPaint::kRight_Align);
	textPaint.setTextSize(10.0f);

	SkTypeface *typeface = SkTypeface::CreateFromName("serif", SkTypeface::kNormal);
	textPaint.setTypeface(typeface);
	typeface->unref();

	std::stringstream targetValueStream;
	targetValueStream.precision(1);
	targetValueStream << std::fixed << targetValue << m_unit;
	std::string targetValueString = targetValueStream.str();

	std::stringstream lastValueStream;
	lastValueStream.precision(1);
	lastValueStream << std::fixed << value << m_unit;
	std::string lastValueString = lastValueStream.str();

	//float targetValueTextWidth = textPaint.measureText(targetValueString.c_str(), targetValueString.length());
	//float lastValueTextWidth = textPaint.measureText(lastValueString.c_str(), lastValueString.length());

	m_canvas->drawText(targetValueString.c_str(), targetValueString.length(), (float)m_bitmap->width(), targetLinePosition, textPaint);
	m_canvas->drawText(lastValueString.c_str(), lastValueString.length(), (float)m_bitmap->width(), (float)m_bitmap->height() - value, textPaint);

	if (!m_imageEncoder->encodeFile("chart.png", *m_bitmap, 100)) {
		printf("Failed to encode image!\n");
	}
}
