/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_PURCHASE_INTENT_CLASSIFIER_H_
#define BAT_ADS_INTERNAL_PURCHASE_INTENT_CLASSIFIER_H_

#include <string>
#include <vector>
#include <deque>
#include <map>

#include "bat/ads/internal/search_providers.h"
#include "bat/ads/internal/purchase_intent/purchase_intent_signal_info.h"
#include "bat/ads/purchase_intent_signal_history.h"

namespace ads {

class PurchaseIntentClassifier {
 public:
  explicit PurchaseIntentClassifier(
      const u_int16_t signal_level,
      const u_int16_t classification_threshold,
      const uint64_t signal_decay_time_window);

  ~PurchaseIntentClassifier();

  PurchaseIntentSignalInfo ExtractIntentSignal(const std::string& url);

  std::vector<std::string> GetWinningCategories(
      std::map<std::string, std::deque<PurchaseIntentSignalHistory>> history,
      const uint8_t max_segments);

 private:
  u_int8_t GetIntentScoreForSegment(
      const std::deque<PurchaseIntentSignalHistory> segment_history);

  const u_int16_t signal_level_;
  const u_int16_t classification_threshold_;
  const uint64_t signal_decay_time_window_;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_PURCHASE_INTENT_CLASSIFIER_H_
