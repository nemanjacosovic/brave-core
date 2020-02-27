/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_SEARCH_ENGINES_DEFAULT_SEARCH_ENGINE_PROVIDER_SERVICE_H_
#define BRAVE_BROWSER_SEARCH_ENGINES_DEFAULT_SEARCH_ENGINE_PROVIDER_SERVICE_H_

#include "base/scoped_observer.h"
#include "brave/browser/search_engines/search_engine_provider_service.h"
#include "components/search_engines/template_url_data.h"
#include "components/search_engines/template_url_service.h"
#include "components/search_engines/template_url_service_observer.h"

class DefaultSearchEngineProviderService : public SearchEngineProviderService,
                                           public TemplateURLServiceObserver {
 public:
  explicit DefaultSearchEngineProviderService(Profile* otr_profile);
  ~DefaultSearchEngineProviderService() override;

  DefaultSearchEngineProviderService(
      const DefaultSearchEngineProviderService&) = delete;
  DefaultSearchEngineProviderService& operator=(
      const DefaultSearchEngineProviderService&) = delete;

 private:
  // TemplateURLServiceObserver overrides:
  void OnTemplateURLServiceChanged() override;

  ScopedObserver<TemplateURLService, TemplateURLServiceObserver> observer_{
      this};

  // Keeping this to check for changes in |OnTemplateURLServiceChanged|.
  GURL default_search_url_;
};

#endif  // BRAVE_BROWSER_SEARCH_ENGINES_DEFAULT_SEARCH_ENGINE_PROVIDER_SERVICE_H_
