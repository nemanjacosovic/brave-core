// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_NAVIGATOR_H_
#define BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_NAVIGATOR_H_

#define BRAVE_NAVIGATOR_OVERRIDES \
    private: \
      Member<NavigatorBrave> brave_; \
    public: \
      NavigatorBrave* brave() { if (!brave_) { brave_ = MakeGarbageCollected<NavigatorBrave>(); } return brave_; }
#include "../../../../../../../third_party/blink/renderer/core/frame/navigator.h"
#undef BRAVE_NAVIGATOR_OVERRIDES

#endif  // BRAVE_CHROMIUM_SRC_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_NAVIGATOR_H_