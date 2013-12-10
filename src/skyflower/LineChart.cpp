#include <algorithm>
#include <cassert>
#include <cstdio>

#include "shared/platform.h"
#include <skia/SkRect.h>
#include <skia/SkCanvas.h>
#include <skia/SkBitmap.h>
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
	/*
	paint.measureText(...);
	paint.getTextBounds(...);
	paint.textToGlyphs(...);
	paint.getFontMetrics(...);
	*/

	assert(m_canvas);
	assert(m_bitmap);
	assert(endTime > startTime);
	assert(resolution > 0.0f);

	// Clear with transparency.
	m_canvas->drawColor(SK_ColorTRANSPARENT);

	SkPaint backgroundPaint;
	backgroundPaint.setColor(SkColorSetARGB(128, 175, 175, 175));
	backgroundPaint.setStyle(SkPaint::kFill_Style);
	backgroundPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);

	// Draw a semi-transparent background.
	m_canvas->drawRect(SkRect::MakeXYWH(0.0f, 0.0f, (float)m_bitmap->width(), (float)m_bitmap->height()), backgroundPaint);

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

		if (timeStamp >= startTime && timeStamp <= endTime && (timeStamp - lastTimeStamp) >= resolution) {
			lastTimeStamp = timeStamp;
			value = (float)m_bitmap->height() - m_dataPoints[i].value;

			if (first) {
				first = false;

				path.moveTo(position, value);
			}
			else {
				path.lineTo(position, value);
				// cubicTo
			}

			position += step;

			maxValue = std::max(maxValue, value);
			minValue = std::min(minValue, value);

			count++;
		}
	}

	// Make sure the line is drawn right to the end of the chart.
	path.lineTo((float)m_bitmap->width(), value);

	maxValue = std::max(maxValue, targetValue);
	minValue = std::min(minValue, targetValue);

	// FIXME: Allow charts that don't start at zero.
	minValue = 0.0f;

	// FIXME: Add some extra space to the top and bottom of the chart.
	// Top only for now since the minValue is fixed at zero.

	float scale = m_bitmap->height() / (maxValue - minValue);

	// Scale the path to fit the canvas.
	SkMatrix transformation;
	transformation.setScale(1.0f, scale);
	transformation.postTranslate(0.0f, m_bitmap->height() * -(scale - 1.0f));
	path.transform(transformation);

	SkPaint pathPaint;
	pathPaint.setColor(SkColorSetARGB(255, 255, 115, 0));
	pathPaint.setStyle(SkPaint::kStroke_Style);
	pathPaint.setStrokeWidth(1.0f);
	pathPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	pathPaint.setFlags(SkPaint::kAntiAlias_Flag);

	// Draw the actual data point lines.
	m_canvas->drawPath(path, pathPaint);

	SkPaint linePaint;
	linePaint.setColor(SkColorSetARGB(255, 0, 0, 0));
	linePaint.setStyle(SkPaint::kStroke_Style);
	linePaint.setStrokeWidth(1.0f);
	linePaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);

	// Draw the target value line.
	float lineValue = ((float)m_bitmap->height() - targetValue) * scale + (m_bitmap->height() * -(scale - 1.0f));
	m_canvas->drawLine(0.0f, lineValue, (float)m_bitmap->width(), lineValue, linePaint);

	//printf("Drew %i out of %i samples with %.2f scale\n", count, totalCount, scale);

	if (!m_imageEncoder->encodeFile("chart.png", *m_bitmap, 100)) {
		printf("Failed to encode image!\n");
	}
}
