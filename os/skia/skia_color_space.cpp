// LAF OS Library
// Copyright (C) 2018-2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "os/skia/skia_color_space.h"

#include "base/debug.h"

#include "include/core/SkImageInfo.h"
#include "include/core/SkString.h"
#include "modules/skcms/src/skcms_public.h"
#include "src/core/SkConvertPixels.h"

#include <algorithm>

namespace os {

namespace {

// Copied from skia/src/core/SkColorSpacePriv.h
constexpr float gSRGB_toXYZD50[]{
  0.4360747f, 0.3850649f, 0.1430804f, // Rx, Gx, Bx
  0.2225045f, 0.7168786f, 0.0606169f, // Ry, Gy, By
  0.0139322f, 0.0971045f, 0.7141733f, // Rz, Gz, Bz
};

// Code for get_color_profile_description() copied from
// skia/src/core/SkICC.cpp from Skia m102

bool nearly_equal(float x, float y)
{
  // A note on why I chose this tolerance:  transfer_fn_almost_equal() uses a
  // tolerance of 0.001f, which doesn't seem to be enough to distinguish
  // between similar transfer functions, for example: gamma2.2 and sRGB.
  //
  // If the tolerance is 0.0f, then this we can't distinguish between two
  // different encodings of what is clearly the same colorspace.  Some
  // experimentation with example files lead to this number:
  static constexpr float kTolerance = 1.0f / (1 << 11);
  return ::fabsf(x - y) <= kTolerance;
}

bool nearly_equal(const skcms_TransferFunction& u, const skcms_TransferFunction& v)
{
  return nearly_equal(u.g, v.g) && nearly_equal(u.a, v.a) && nearly_equal(u.b, v.b) &&
         nearly_equal(u.c, v.c) && nearly_equal(u.d, v.d) && nearly_equal(u.e, v.e) &&
         nearly_equal(u.f, v.f);
}

bool nearly_equal(const skcms_Matrix3x3& u, const skcms_Matrix3x3& v)
{
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      if (!nearly_equal(u.vals[r][c], v.vals[r][c])) {
        return false;
      }
    }
  }
  return true;
}

// Return nullptr if the color profile doen't have a special name.
const char* get_color_profile_description(const skcms_TransferFunction& fn,
                                          const skcms_Matrix3x3& toXYZD50)
{
  bool srgb_xfer = nearly_equal(fn, SkNamedTransferFn::kSRGB);
  bool srgb_gamut = nearly_equal(toXYZD50, SkNamedGamut::kSRGB);
  if (srgb_xfer && srgb_gamut) {
    return "sRGB";
  }
  bool line_xfer = nearly_equal(fn, SkNamedTransferFn::kLinear);
  if (line_xfer && srgb_gamut) {
    return "Linear Transfer with sRGB Gamut";
  }
  bool twoDotTwo = nearly_equal(fn, SkNamedTransferFn::k2Dot2);
  if (twoDotTwo && srgb_gamut) {
    return "2.2 Transfer with sRGB Gamut";
  }
  if (twoDotTwo && nearly_equal(toXYZD50, SkNamedGamut::kAdobeRGB)) {
    return "AdobeRGB";
  }
  bool display_p3 = nearly_equal(toXYZD50, SkNamedGamut::kDisplayP3);
  if (srgb_xfer || line_xfer) {
    if (srgb_xfer && display_p3) {
      return "sRGB Transfer with Display P3 Gamut";
    }
    if (line_xfer && display_p3) {
      return "Linear Transfer with Display P3 Gamut";
    }
    bool rec2020 = nearly_equal(toXYZD50, SkNamedGamut::kRec2020);
    if (srgb_xfer && rec2020) {
      return "sRGB Transfer with Rec-BT-2020 Gamut";
    }
    if (line_xfer && rec2020) {
      return "Linear Transfer with Rec-BT-2020 Gamut";
    }
  }
  return nullptr;
}

} // namespace

SkiaColorSpace::SkiaColorSpace(const gfx::ColorSpaceRef& gfxcs) : m_gfxcs(gfxcs), m_skcs(nullptr)
{
  switch (m_gfxcs->type()) {
    case gfx::ColorSpace::None:
      if (m_gfxcs->name().empty())
        m_gfxcs->setName("None");
      break;

    case gfx::ColorSpace::sRGB:
    case gfx::ColorSpace::RGB:
      if (gfxcs->hasGamma()) {
        if (gfxcs->gamma() == 1.0)
          m_skcs = SkColorSpace::MakeSRGBLinear();
        else {
          skcms_TransferFunction fn;
          fn.a = 1.0f;
          fn.b = fn.c = fn.d = fn.e = fn.f = 0.0f;
          fn.g = gfxcs->gamma();
          m_skcs = SkColorSpace::MakeRGB(fn, SkNamedGamut::kSRGB);
        }
      }
      else {
        skcms_TransferFunction skFn;
        skcms_Matrix3x3 toXYZD50;

        if (m_gfxcs->hasPrimaries()) {
          const gfx::ColorSpacePrimaries* primaries = m_gfxcs->primaries();
          if (!skcms_PrimariesToXYZD50(primaries->rx,
                                       primaries->ry,
                                       primaries->gx,
                                       primaries->gy,
                                       primaries->bx,
                                       primaries->by,
                                       primaries->wx,
                                       primaries->wy,
                                       &toXYZD50)) {
            toXYZD50 = skcms_sRGB_profile()->toXYZD50;
          }
        }

        if (m_gfxcs->hasTransferFn()) {
          const gfx::ColorSpaceTransferFn* fn = m_gfxcs->transferFn();
          skFn.g = fn->g;
          skFn.a = fn->a;
          skFn.b = fn->b;
          skFn.c = fn->c;
          skFn.d = fn->d;
          skFn.e = fn->e;
          skFn.f = fn->f;
        }

        if (m_gfxcs->hasTransferFn()) {
          if (!m_gfxcs->hasPrimaries()) {
            toXYZD50 = skcms_sRGB_profile()->toXYZD50;
          }
          m_skcs = SkColorSpace::MakeRGB(skFn, toXYZD50);
        }
        else if (m_gfxcs->hasPrimaries()) {
          m_skcs = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, toXYZD50);
        }
        else {
          m_skcs = SkColorSpace::MakeSRGB();
        }
      }
      break;

    case gfx::ColorSpace::ICC: {
      skcms_ICCProfile icc;
      if (skcms_Parse(m_gfxcs->iccData(), m_gfxcs->iccSize(), &icc)) {
        m_skcs = SkColorSpace::Make(icc);
      }
      break;
    }
  }

  // TODO read color profile name from ICC data

  if (m_skcs && m_gfxcs->name().empty()) {
    skcms_TransferFunction fn;
    skcms_Matrix3x3 toXYZD50;
    if (m_skcs->isNumericalTransferFn(&fn) && m_skcs->toXYZD50(&toXYZD50)) {
      // Create a description for the color profile
      const char* desc = get_color_profile_description(fn, toXYZD50);
      if (desc)
        m_gfxcs->setName(desc);
    }
  }

  if (m_gfxcs->name().empty())
    m_gfxcs->setName("Custom Profile");
}

SkiaColorSpaceConversion::SkiaColorSpaceConversion(const os::ColorSpaceRef& srcColorSpace,
                                                   const os::ColorSpaceRef& dstColorSpace)
  : m_srcCS(srcColorSpace)
  , m_dstCS(dstColorSpace)
{
  ASSERT(srcColorSpace);
  ASSERT(dstColorSpace);
}

bool SkiaColorSpaceConversion::convertRgba(uint32_t* dst, const uint32_t* src, int n)
{
  auto dstInfo = SkImageInfo::Make(
    n,
    1,
    kRGBA_8888_SkColorType,
    kUnpremul_SkAlphaType,
    static_cast<const SkiaColorSpace*>(m_dstCS.get())->skColorSpace());
  auto srcInfo = SkImageInfo::Make(
    n,
    1,
    kRGBA_8888_SkColorType,
    kUnpremul_SkAlphaType,
    static_cast<const SkiaColorSpace*>(m_srcCS.get())->skColorSpace());
  return SkConvertPixels(dstInfo, dst, 4 * n, srcInfo, src, 4 * n);
}

bool SkiaColorSpaceConversion::convertGray(uint8_t* dst, const uint8_t* src, int n)
{
  auto dstInfo = SkImageInfo::Make(
    n,
    1,
    kGray_8_SkColorType,
    kOpaque_SkAlphaType,
    static_cast<const SkiaColorSpace*>(m_dstCS.get())->skColorSpace());
  auto srcInfo = SkImageInfo::Make(
    n,
    1,
    kGray_8_SkColorType,
    kOpaque_SkAlphaType,
    static_cast<const SkiaColorSpace*>(m_srcCS.get())->skColorSpace());
  return SkConvertPixels(dstInfo, dst, n, srcInfo, src, n);
}

} // namespace os
