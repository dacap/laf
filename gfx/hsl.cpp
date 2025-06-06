// LAF Gfx Library
// Copyright (c) 2020-2022  Igara Studio S.A.
// Copyright (c) 2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "gfx/hsl.h"
#include "gfx/rgb.h"

#include <algorithm>
#include <cmath>

namespace gfx {

using namespace std;

Hsl::Hsl(double hue, double saturation, double lightness)
  : m_hue(hue)
  , m_saturation(std::clamp(saturation, 0.0, 1.0))
  , m_lightness(std::clamp(lightness, 0.0, 1.0))
{
}

Hsl::Hsl(const Rgb& rgb)
{
  const int M = rgb.maxComponent();
  const int m = rgb.minComponent();
  const int c = M - m;
  const double chroma = double(c) / 255.0;
  const double l = double((M + m) / 255.0) / 2.0;
  double hue_prime = 0.0;
  double h, s;

  if (c == 0) {
    h = 0.0; // Undefined Hue because max == min
    s = 0.0;
  }
  else {
    const double r = double(rgb.red()) / 255.0;
    const double g = double(rgb.green()) / 255.0;
    const double b = double(rgb.blue()) / 255.0;

    if (M == rgb.red()) {
      hue_prime = (g - b) / chroma;

      while (hue_prime < 0.0)
        hue_prime += 6.0;
      hue_prime = std::fmod(hue_prime, 6.0);
    }
    else if (M == rgb.green()) {
      hue_prime = ((b - r) / chroma) + 2.0;
    }
    else if (M == rgb.blue()) {
      hue_prime = ((r - g) / chroma) + 4.0;
    }

    h = hue_prime * 60.0;
    s = chroma / (1 - std::fabs(2.0 * l - 1.0));
  }

  m_hue = h;
  m_saturation = s;
  m_lightness = l;
}

int Hsl::hueInt() const
{
  return int(std::floor(m_hue + 0.5));
}

int Hsl::saturationInt() const
{
  return int(std::floor(m_saturation * 100.0 + 0.5));
}

int Hsl::lightnessInt() const
{
  return int(std::floor(m_lightness * 100.0 + 0.5));
}

} // namespace gfx
