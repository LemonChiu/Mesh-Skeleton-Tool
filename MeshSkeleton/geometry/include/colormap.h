#ifndef _COLORMAP_H_
#define _COLORMAP_H_

#include "color.h"
#include <cmath>

class ColorMap
{
public:
    ColorMap(float fMin = 0, float fMax = 1.0f)
    {
        for (unsigned int i = 0; i < 8; i++) {
            m_colors[i][0] = 0; m_colors[i][1] = 0; m_colors[i][2] = 0.5 + (i + 1) * 0.0625;
        }
        for (unsigned int i = 8; i < 24; i++) {
            m_colors[i][0] = 0; m_colors[i][1] = 0.0625 * (i - 7); m_colors[i][2] = 1.0000;
        }
        for (unsigned int i = 24; i < 40; i++) {
            m_colors[i][0] = 0.0625 * (i - 23); m_colors[i][1] = 1; m_colors[i][2] = 1.0000 - m_colors[i][0] ;
        }
        for (unsigned int i = 40; i < 56; i++) {
            m_colors[i][0] = 1; m_colors[i][1] = 1 - 0.0625 * (i - 39); m_colors[i][2] = 0;
        }
        for (unsigned int i = 56; i < 64; i++) {
            m_colors[i][0] = 1 - 0.0625 * (i - 55); m_colors[i][1] = 0; m_colors[i][2] = 0;
        }
        m_fMin = fMin;
        m_fMax = fMax;
    }

    Colorf getColor(float f)
    {
        unsigned int index = std::floor((f - m_fMin) * 64 / (m_fMax - m_fMin));
        if (index == 64) {
            --index;
        }
        return Colorf(m_colors[index][0], m_colors[index][1], m_colors[index][2]);
    }

private:
    float m_fMin;
    float m_fMax;
    float m_colors[64][3];
};

#endif // _COLORMAP_H_
