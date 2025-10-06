// LAF Text Library
// Copyright (c) 2024-2025  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef LAF_TEXT_SKIA_TEXT_BLOB_INCLUDED
#define LAF_TEXT_SKIA_TEXT_BLOB_INCLUDED
#pragma once

#include "text/text_blob.h"

#include "include/core/SkTextBlob.h"

namespace text {

class SkiaTextBlob : public TextBlob {
public:
  SkiaTextBlob() = delete;
  SkiaTextBlob(const sk_sp<SkTextBlob>& skTextBlob, const gfx::RectF& bounds = gfx::RectF());

  sk_sp<SkTextBlob> skTextBlob() const { return m_skTextBlob; }

  void setVisitOffset(const gfx::PointF& visitOffset);
  void visitRuns(const RunVisitor& visitor) override;

  static TextBlobRef Make(const FontRef& font, const std::string& text);

  static TextBlobRef MakeWithShaper(const FontMgrRef& fontMgr,
                                    const FontRef& font,
                                    const std::string& text,
                                    TextBlob::RunHandler* handler,
                                    const ShaperFeatures features);

private:
  sk_sp<SkTextBlob> m_skTextBlob;
  gfx::PointF m_visitOffset;
};

} // namespace text

#endif
