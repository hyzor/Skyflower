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
	double totalValue = 0.0;
	int count = 0;
	bool first = true;

	for (size_t i = m_dataPointStart; i != m_dataPointEnd; i = (i + 1) % m_dataPointCapacity) {
		timeStamp = m_dataPoints[i].timeStamp;

		// FIXME: Interpolate instead of skipping samples failing (timeStamp - lastTimeStamp) >= resolution
		if (timeStamp >= startTime && timeStamp <= endTime && (timeStamp - lastTimeStamp) >= resolution) {
			value = m_dataPoints[i].value;
			totalValue += value;

			if (first) {
				first = false;

				path.moveTo(count * step, (float)m_bitmap->height() - value);
			}
			else {
				path.lineTo(count * step, (float)m_bitmap->height() - value);
				// cubicTo
			}

			maxValue = std::max(maxValue, value);
			minValue = std::min(minValue, value);

			lastTimeStamp = timeStamp;
			count++;
		}
	}

	//printf("drawing %d samples, maxValue=%.1f, minValue=%.1f, lastValue=%.1f\n", count, maxValue, minValue, value);

	// Add some padding to the top and bottom of the chart.
	float drawMax = std::max(maxValue, targetValue);
	float drawMin = std::min(minValue, targetValue);
	float padding = (drawMax - drawMin) * 0.1f;
	// FIXME: Allow charts not starting at zero.
	float drawRange = (drawMax + padding); // - (drawMin - padding)

	// Clear the canvas with transparency.
	m_canvas->clear(SK_ColorTRANSPARENT);

	SkPaint backgroundPaint;
	backgroundPaint.setColor(SkColorSetARGB(32, 0, 0, 0));
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
	float scale = m_bitmap->height() / drawRange;
	SkMatrix transformation;
	transformation.setScale(1.0f, scale);
	transformation.postTranslate(0.0f, m_bitmap->height() * -(scale - 1.0f));
	path.transform(transformation);

	// Draw the actual data point lines.
	m_canvas->drawPath(path, pathPaint);

	SkPaint linePaint;
	linePaint.setColor(SkColorSetARGB(255, 0, 255, 255));
	linePaint.setStyle(SkPaint::kStroke_Style);
	linePaint.setStrokeWidth(1.0f);
	//linePaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);

	// Draw the target value line.
	float targetLinePosition = ((float)m_bitmap->height() - targetValue) * scale + (m_bitmap->height() * -(scale - 1.0f));
	m_canvas->drawLine(0.0f, targetLinePosition, (float)m_bitmap->width(), targetLinePosition, linePaint);

	// Draw the labels
	SkPaint textPaint;
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
	m_canvas->drawText(minValueString.c_str(), minValueString.length(), 0.0f, textPosition, textPaint);
	textPosition += textPadding;
	m_canvas->drawText(maxValueString.c_str(), maxValueString.length(), 0.0f, textPosition, textPaint);
	textPosition += textPadding;
	m_canvas->drawText(averageValueString.c_str(), averageValueString.length(), 0.0f, textPosition, textPaint);
	textPosition += textPadding;
	m_canvas->drawText(currentValueString.c_str(), currentValueString.length(), 0.0f, textPosition, textPaint);

	// Draw the target label at the end of the chart just above the target line.
	textPaint.setTextAlign(SkPaint::kRight_Align);
	m_canvas->drawText(targetValueString.c_str(), targetValueString.length(), (float)m_bitmap->width(), targetLinePosition - (textPadding / 2.0f), textPaint);

	if (!m_imageEncoder->encodeFile("chart.png", *m_bitmap, 100)) {
		printf("Failed to encode image!\n");
	}
}
