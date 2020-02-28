/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/strings/string_number_conversions.h"
#include "crypto/hmac.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/supplementable.h"

const char kBraveSessionToken[] = "brave_session_token";

#define BRAVE_TODATAURL_INTERNAL                                               \
  base::StringPiece host =                                                     \
      base::StringPiece(GetExecutionContextUrl().Host().Utf8());               \
  std::string domain = net::registry_controlled_domains::GetDomainAndRegistry( \
      host, net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);     \
  crypto::HMAC h(crypto::HMAC::SHA256);                                        \
  base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();        \
  DCHECK(cmd_line->HasSwitch(kBraveSessionToken));                             \
  uint64_t key;                                                                \
  base::StringToUint64(cmd_line->GetSwitchValueASCII(kBraveSessionToken),      \
                       &key);                                                  \
  CHECK(h.Init(reinterpret_cast<const unsigned char*>(&key), sizeof key));     \
  uint8_t domainkey[32];                                                       \
  CHECK(h.Sign(domain, domainkey, sizeof domainkey));                          \
  const uint8_t* byte = reinterpret_cast<const uint8_t*>(&domainkey);          \
  uint8_t channel = *byte % 3;                                                 \
  unsigned char* pixels = const_cast<unsigned char*>(data_buffer->Pixels());   \
  uint64_t v = *reinterpret_cast<uint64_t*>(&domainkey);                       \
  const uint64_t pixel_size = data_buffer->Width() * data_buffer->Height();    \
  const uint64_t zero = 0;                                                     \
  uint64_t pixel_index;                                                        \
  for (unsigned long i = 0; i < sizeof domainkey; i++) {                       \
    uint8_t bit = domainkey[i];                                                \
    for (int j = 8; j >= 0; j--) {                                             \
      pixel_index = 4 * (v % pixel_size) + channel;                            \
      pixels[pixel_index] = pixels[pixel_index] ^ (bit & 0x1);                 \
      bit = bit >> 1;                                                          \
      v = ((v >> 1) | (((v << 62) ^ (v << 61)) & (~(~zero << 63) << 62)));     \
    }                                                                          \
  }

#include "../../../../../../../third_party/blink/renderer/core/html/canvas/html_canvas_element.cc"

#undef BRAVE_TODATAURL_INTERNAL
