// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_NAVIGATOR_BRAVE_H_
#define BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_NAVIGATOR_BRAVE_H_

#include "third_party/blink/renderer/core/execution_context/context_lifecycle_observer.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class ScriptPromise;
class ScriptState;
class NavigatorBrave : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  NavigatorBrave() = default;
  ScriptPromise isBrave(ScriptState*);
};

}  // namespace blink

#endif  // BRAVE_THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_NAVIGATOR_BRAVE_H_
