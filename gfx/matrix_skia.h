// LAF Gfx Library
// Copyright (c) 2020-present  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_MATRIX_SKIA_H_INCLUDED
#define GFX_MATRIX_SKIA_H_INCLUDED
#pragma once

#include "base/span.h"
#include "gfx/point.h"
#include "gfx/rect.h"

#include "include/core/SkMatrix.h"

#include <vector>

namespace gfx {

// Simple wrapper for SkMatrix
class Matrix {
public:
  constexpr Matrix() {}
  constexpr Matrix(const SkMatrix& skMatrix) : m_skMatrix(skMatrix) {}

  static Matrix MakeScale(float sx, float sy) { return Matrix(SkMatrix::Scale(sx, sy)); }

  static Matrix MakeScale(float scale) { return Matrix(SkMatrix::Scale(scale, scale)); }

  static Matrix MakeTrans(float x, float y) { return Matrix(SkMatrix::Translate(x, y)); }

  static Matrix MakeAll(float scaleX,
                        float skewX,
                        float transX,
                        float skewY,
                        float scaleY,
                        float transY,
                        float pers0,
                        float pers1,
                        float pers2)
  {
    return Matrix(
      SkMatrix::MakeAll(scaleX, skewX, transX, skewY, scaleY, transY, pers0, pers1, pers2));
  }

  Matrix& reset()
  {
    m_skMatrix.reset();
    return *this;
  }

  bool isIdentity() const { return m_skMatrix.isIdentity(); }
  bool isScaleTranslate() const { return m_skMatrix.isScaleTranslate(); }
  bool isTranslate() const { return m_skMatrix.isTranslate(); }

  float getScaleX() const { return m_skMatrix.getScaleX(); }
  float getScaleY() const { return m_skMatrix.getScaleY(); }
  float getSkewY() const { return m_skMatrix.getSkewY(); }
  float getSkewX() const { return m_skMatrix.getSkewX(); }
  float getTranslateX() const { return m_skMatrix.getTranslateX(); }
  float getTranslateY() const { return m_skMatrix.getTranslateY(); }
  float getPerspX() const { return m_skMatrix.getPerspX(); }
  float getPerspY() const { return m_skMatrix.getPerspY(); }

  Matrix& setIdentity()
  {
    m_skMatrix.setIdentity();
    return *this;
  }

  Matrix& setTranslate(float dx, float dy)
  {
    m_skMatrix.setTranslate(dx, dy);
    return *this;
  }

  void setScale(float sx, float sy, float px, float py) { m_skMatrix.setScale(sx, sy, px, py); }

  void setScale(float sx, float sy) { m_skMatrix.setScale(sx, sy); }

  void setRotate(float degrees, float px, float py) { m_skMatrix.setRotate(degrees, px, py); }

  void setRotate(float degrees) { m_skMatrix.setRotate(degrees); }

  void setScaleTranslate(float sx, float sy, float tx, float ty)
  {
    m_skMatrix.setScaleTranslate(sx, sy, tx, ty);
  }

  Matrix& preTranslate(float dx, float dy)
  {
    m_skMatrix.preTranslate(dx, dy);
    return *this;
  }

  Matrix& postTranslate(float dx, float dy)
  {
    m_skMatrix.postTranslate(dx, dy);
    return *this;
  }

  Matrix& setConcat(const Matrix& a, const Matrix& b)
  {
    m_skMatrix.setConcat(a.m_skMatrix, b.m_skMatrix);
    return *this;
  }

  Matrix& preConcat(const Matrix& other)
  {
    m_skMatrix.preConcat(other.m_skMatrix);
    return *this;
  }

  Matrix& postConcat(const Matrix& other)
  {
    m_skMatrix.postConcat(other.m_skMatrix);
    return *this;
  }

  std::optional<Matrix> invert() const
  {
    std::optional<SkMatrix> inv = m_skMatrix.invert();
    if (!inv)
      return {};
    return { *inv };
  }

  RectF mapRect(const RectF& src) const
  {
    SkRect dst;
    m_skMatrix.mapRect(
      &dst,
      SkRect::MakeXYWH(SkScalar(src.x), SkScalar(src.y), SkScalar(src.w), SkScalar(src.h)));
    return RectF(dst.x(), dst.y(), dst.width(), dst.height());
  }

  PointF mapPoint(const PointF& pt) const
  {
    SkPoint skpt = m_skMatrix.mapPoint(SkPoint::Make(pt.x, pt.y));
    return PointF(skpt.x(), skpt.y());
  }

  void mapPoints(base::span<PointF> pts) const
  {
    std::vector<SkPoint> skpts(pts.size());
    m_skMatrix.mapPoints(skpts);
    for (size_t i = 0; i < pts.size(); ++i) {
      pts[i].x = skpts[i].x();
      pts[i].y = skpts[i].y();
    }
  }

  const SkMatrix& skMatrix() const { return m_skMatrix; }
  SkMatrix& skMatrix() { return m_skMatrix; }

private:
  SkMatrix m_skMatrix;
};

} // namespace gfx

#endif
