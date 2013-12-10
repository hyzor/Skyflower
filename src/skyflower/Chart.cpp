#include <cassert>
#include <cstdio>

#include "Chart.h"

Chart::Chart()
{
	m_bitmap = new SkBitmap();
	m_bitmap->setConfig(SkBitmap::kARGB_8888_Config, 200, 100);
	// FIXME: Replace with setPixels.
	m_bitmap->allocPixels();

	m_canvas = new SkCanvas(*m_bitmap);
	m_canvas->drawColor(SK_ColorGREEN);



	m_imageEncoder = SkImageEncoder::Create(SkImageEncoder::kPNG_Type);
}

Chart::~Chart()
{
	delete m_imageEncoder;

	delete m_canvas;
	delete m_bitmap;
}

void Chart::Draw()
{
	if (!m_imageEncoder->encodeFile("chart.png", *m_bitmap, 100)) {
		printf("Failed to encode image!\n");
	}
}
