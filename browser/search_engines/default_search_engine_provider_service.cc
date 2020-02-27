/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/search_engines/default_search_engine_provider_service.h"

#include "base/metrics/histogram_macros.h"
#include "brave/browser/profiles/profile_util.h"
#include "chrome/browser/profiles/profile.h"
#include "components/search_engines/template_url_service.h"

namespace {

// Note: append-only enumeration! Never remove any existing values, as this enum
// is used to bucket a UMA histogram, and removing values breaks that.
enum class SearchEngineP3A {
  kOther,
  kGoogle,
  kDuckDuckGo,
  kStartpage,
  kBing,
  kQwant,
  kMaxValue = kQwant,
};

void RecordSearchEngineP3A(const GURL& search_engine_url,
                           SearchEngineType type) {
  SearchEngineP3A answer = SearchEngineP3A::kOther;

  if (type == SEARCH_ENGINE_GOOGLE) {
    answer = SearchEngineP3A::kGoogle;
  } else if (type == SEARCH_ENGINE_DUCKDUCKGO) {
    answer = SearchEngineP3A::kDuckDuckGo;
  } else if (type == SEARCH_ENGINE_BING) {
    answer = SearchEngineP3A::kBing;
  } else if (type == SEARCH_ENGINE_QWANT) {
    answer = SearchEngineP3A::kQwant;
  } else if (type == SEARCH_ENGINE_OTHER){
    if (search_engine_url.host() == "startpage.com")  {
      answer = SearchEngineP3A::kStartpage;
    }
  }

  LOG(ERROR) << "LOGGED " << (int)answer;
  UMA_HISTOGRAM_ENUMERATION("Brave.Search.DefaultEngine", answer);
}

}  // namespace

DefaultSearchEngineProviderService::
DefaultSearchEngineProviderService(Profile* otr_profile)
    : SearchEngineProviderService(otr_profile) {
  observer_.Add(original_template_url_service_);
  LOG(ERROR) << "=============================";
  const TemplateURL* template_url =
      original_template_url_service_->GetDefaultSearchProvider();

  if (template_url) {
    const GURL url = template_url->
        GenerateSearchURL(original_template_url_service_->search_terms_data());
    if (!url.is_empty()) {
      default_search_url_ = url;
    }
  }
}

DefaultSearchEngineProviderService::~DefaultSearchEngineProviderService() {
}

void DefaultSearchEngineProviderService::OnTemplateURLServiceChanged() {
  const TemplateURL* template_url =
      original_template_url_service_->GetDefaultSearchProvider();
  if (template_url) {
    const SearchTermsData& search_terms =
        original_template_url_service_->search_terms_data();
    const GURL& url = template_url->GenerateSearchURL(search_terms);
    if (url != default_search_url_) {
      RecordSearchEngineP3A(url, template_url->GetEngineType(search_terms));
    }
  }
}

