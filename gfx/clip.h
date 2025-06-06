// LAF Gfx Library
// Copyright (C) 2020-2024  Igara Studio S.A.
// Copyright (C) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_CLIP_H_INCLUDED
#define GFX_CLIP_H_INCLUDED
#pragma once

#include "gfx/point.h"
#include "gfx/rect.h"
#include "gfx/size.h"

namespace gfx {

// Imagine you want to copy the following rectangle
//
//       size.w
//    +---------+
//    |         | size.h
//    +---------+
//
// from a "source image" to a "destination image", from "src"
// location to the "dst" location:
//
//          "source image"        "destination image"
//         +--------------+      +---------------------+
// src.xy  |              |      |         dst.xy      |
//    +---------+         |      |            +---------+
//    |         |         | ---> |            |         |
//    +---------+         |      |            +---------+
//         |              |      |                     |
//         +--------------+      +---------------------+
//
// There are two restrictions:
//
// 1) You cannot read from the "source image" outside its bounds, and
// 2) you cannot write into the "destination image" outside its bounds.
//
// This Clip class helps to determine the only valid region to
// read/write between these two images. The initial Clip values can
// be outside these bounds, but the Clip::clip() function determines
// the valid region.
//
// So in our example, if the "source image" has a dimension of
// "avail_src_w/h" and the "destination image" a dimension of
// "avail_dst_w/h", after calling Clip::clip() we get these modified
// Clip fields:
//
//          "source image"        "destination image"
//         +--------------+      +---------------------+
//      src.xy            |      |              dst.xy |
//    .....+---+.         |      |            .....+---+.
//    .....|   |.         | ---> |            .....|   |.
//    .....+---+.         |      |            .....+---+.
//         |              |      |                     |
//         +--------------+      +---------------------+
//
// The dotted areas (...) are invalid image regions that cannot be
// read/written. The "dst", "src", and "size" fields of the Clip
// instance are adjusted to be inside both images.
//
// Clip::clip() returns false in case there is no valid area to
// copy.
template<typename T>
class ClipT {
public:
  PointT<T> dst;
  PointT<T> src;
  SizeT<T> size;

  ClipT() : dst(0, 0), src(0, 0), size(0, 0) {}

  ClipT(T w, T h) : dst(0, 0), src(0, 0), size(w, h) {}

  explicit ClipT(const SizeT<T>& sz) : dst(0, 0), src(0, 0), size(sz) {}

  ClipT(T dst_x, T dst_y, T src_x, T src_y, T w, T h)
    : dst(dst_x, dst_y)
    , src(src_x, src_y)
    , size(w, h)
  {
  }

  ClipT(T dst_x, T dst_y, const RectT<T>& srcBounds)
    : dst(dst_x, dst_y)
    , src(srcBounds.x, srcBounds.y)
    , size(srcBounds.w, srcBounds.h)
  {
  }

  ClipT(const PointT<T>& dst, const PointT<T>& src, const SizeT<T>& size)
    : dst(dst)
    , src(src)
    , size(size)
  {
  }

  ClipT(const PointT<T>& dst, const RectT<T>& srcBounds)
    : dst(dst)
    , src(srcBounds.x, srcBounds.y)
    , size(srcBounds.w, srcBounds.h)
  {
  }

  ClipT(const RectT<T>& bounds)
    : dst(bounds.x, bounds.y)
    , src(bounds.x, bounds.y)
    , size(bounds.w, bounds.h)
  {
  }

  template<typename U>
  ClipT(const ClipT<U>& other) : dst(other.dst)
                               , src(other.src)
                               , size(other.size)
  {
  }

  RectT<T> dstBounds() const { return RectT<T>(dst, size); }
  RectT<T> srcBounds() const { return RectT<T>(src, size); }

  bool operator==(const ClipT<T>& other) const
  {
    return (dst == other.dst && src == other.src && size == other.size);
  }

  bool clip(
    // Available area
    T avail_dst_w,
    T avail_dst_h,
    T avail_src_w,
    T avail_src_h)
  {
    // Clip srcBounds

    if (src.x < T(0)) {
      size.w += src.x;
      dst.x -= src.x;
      src.x = T(0);
    }

    if (src.y < T(0)) {
      size.h += src.y;
      dst.y -= src.y;
      src.y = T(0);
    }

    if (src.x + size.w > avail_src_w)
      size.w -= src.x + size.w - avail_src_w;

    if (src.y + size.h > avail_src_h)
      size.h -= src.y + size.h - avail_src_h;

    // Clip dstBounds

    if (dst.x < T(0)) {
      size.w += dst.x;
      src.x -= dst.x;
      dst.x = T(0);
    }

    if (dst.y < T(0)) {
      size.h += dst.y;
      src.y -= dst.y;
      dst.y = T(0);
    }

    if (dst.x + size.w > avail_dst_w)
      size.w -= dst.x + size.w - avail_dst_w;

    if (dst.y + size.h > avail_dst_h)
      size.h -= dst.y + size.h - avail_dst_h;

    return (size.w > T(0) && size.h > T(0));
  }
};

using Clip = ClipT<int>;
using ClipF = ClipT<float>;

} // namespace gfx

#endif
