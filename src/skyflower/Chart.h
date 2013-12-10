#ifndef SKYFLOWER_CHART_H
#define SKYFLOWER_CHART_H

#include "shared/platform.h"

#include "skia/SkCanvas.h"
#include "skia/SkBitmap.h"

#include "skia/SkImageEncoder.h"

class Chart
{
public:
	Chart();
	virtual ~Chart();

	void Draw();

private:
	SkCanvas *m_canvas;
	SkBitmap *m_bitmap;

	SkImageEncoder *m_imageEncoder;
};

#endif
