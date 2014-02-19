#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>
#include <sstream>

// skia needs Windows.h to be included.
#include "shared/platform.h"
#include <skia/SkRect.h>
#include <skia/SkCanvas.h>
#include <skia/SkBitmap.h>
#include <skia/SkTypeface.h>
#include <skia/SkXfermode.h>

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


LineChartRendererSkia::LineChartRendererSkia()
{
	m_bitmap = NULL;
	m_canvas = NULL;

	m_targetValue = 0.0;
	m_unit = "";
	m_label = "";
}

LineChartRendererSkia::~LineChartRendererSkia()
{
	if (m_canvas)
		delete m_canvas;
	if (m_bitmap)
		delete m_bitmap;
}

void LineChartRendererSkia::SetTargetValue(double targetValue)
{
	m_targetValue = targetValue;
}

void LineChartRendererSkia::SetUnit(const std::string &unit)
{
	m_unit = unit;
}

void LineChartRendererSkia::SetLabel(const std::string &label)
{
	m_label = label;
}

void LineChartRendererSkia::Draw(const std::vector<const struct LineChartDataPoint> &dataPoints, Texture2D *texture)
{
	unsigned int width = texture->GetWidth();
	unsigned int height = texture->GetHeight();

	if (!m_canvas || (m_bitmap->width() != width || m_bitmap->height() != height)) {
		if (m_canvas)
			delete m_canvas;
		if (m_bitmap)
			delete m_bitmap;

		m_bitmap = new SkBitmap();
		m_bitmap->setConfig(SkBitmap::kARGB_8888_Config, width, height);
		m_bitmap->allocPixels();

		m_canvas = new SkCanvas(*m_bitmap);

		printf("allocated skia bitmap\n");
	}

	assert(m_canvas);
	assert(m_bitmap);

	// Clear the canvas with transparency.
	m_canvas->clear(SK_ColorTRANSPARENT);

	// Draw a semi-transparent background.
	SkPaint backgroundPaint;
	backgroundPaint.setColor(SkColorSetARGB(128, 0, 0, 0));
	backgroundPaint.setStyle(SkPaint::kFill_Style);
	backgroundPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	m_canvas->drawRect(SkRect::MakeXYWH(0.0f, 0.0f, (SkScalar)width, (SkScalar)height), backgroundPaint);

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
	m_canvas->drawText(m_label.c_str(), m_label.length(), (SkScalar)width, 9.0f, textPaint);

	// Only draw the background and label if we don't have any points to render.
	if (dataPoints.size() == 0) {
		texture->UploadData(m_bitmap->getPixels());
		return;
	}

	double startTime = dataPoints.back().timeStamp;
	double endTime = dataPoints.front().timeStamp;
	double timeSpan = endTime - startTime;

	const struct LineChartDataPoint *point;
	SkPath path;

	double maxValue = DBL_MIN;
	double minValue = DBL_MAX;
	double totalValue = 0.0;
	int count = 0;

	double position;
	bool first = true;

	for (auto iter = dataPoints.begin(); iter != dataPoints.end(); iter++) {
		point = &(*iter);
		position = ((point->timeStamp - startTime) / timeSpan) * width;

		//assert(point->value > 0.0);

		if (first) {
			first = false;
			path.moveTo((SkScalar)position, (SkScalar)(height - point->value));
		}
		else {
			path.lineTo((SkScalar)position, (SkScalar)(height - point->value));
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
	double scale = height / drawRange;
	double pixelPadding = height * -(scale - 1.0);
	SkMatrix transformation;
	transformation.setScale(1.0f, (SkScalar)scale);
	transformation.postTranslate(0.0f, (SkScalar)pixelPadding);
	path.transform(transformation);

	double targetLinePosition = (height - m_targetValue) * scale + pixelPadding;

	// Draw the target value line.
	SkPaint linePaint;
	linePaint.setColor(SkColorSetARGB(255, 255, 133, 27));
	linePaint.setStyle(SkPaint::kStroke_Style);
	linePaint.setStrokeWidth(1.0f);
	//linePaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
	m_canvas->drawLine(0.0f, (SkScalar)targetLinePosition, (SkScalar)width, (SkScalar)targetLinePosition, linePaint);

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

	stringStream.str("");
	stringStream << "Cur:" << std::fixed << dataPoints.back().value << m_unit;
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
	//m_canvas->drawText(targetValueString.c_str(), targetValueString.length(), (float)width, (float)(targetLinePosition - (textPadding / 2.0)), textPaint);

	/*
	// Draw the high and low values.
	m_canvas->drawText(highValueString.c_str(), highValueString.length(), (SkScalar)width, 9.0f, textPaint);
	m_canvas->drawText(lowValueString.c_str(), lowValueString.length(), (SkScalar)width, height - 1.0f, textPaint);
	*/

	texture->UploadData(m_bitmap->getPixels());
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

void LineChartRendererD3D::Draw(const std::vector<const struct LineChartDataPoint> &dataPoints, Texture2D *texture)
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

	double startTime = dataPoints.back().timeStamp;
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

	stringStream << std::fixed << m_targetValue << m_unit;
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
