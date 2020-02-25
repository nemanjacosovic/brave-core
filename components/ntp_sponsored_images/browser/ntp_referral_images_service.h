// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef BRAVE_COMPONENTS_NTP_SPONSORED_IMAGES_BROWSER_NTP_REFERRAL_IMAGES_SERVICE_H_
#define BRAVE_COMPONENTS_NTP_SPONSORED_IMAGES_BROWSER_NTP_REFERRAL_IMAGES_SERVICE_H_

#include <memory>
#include <string>

#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/values.h"
#include "components/prefs/pref_change_registrar.h"

namespace component_updater {
class ComponentUpdateService;
}  // namespace component_updater

class PrefRegistrySimple;
class PrefService;

namespace ntp_sponsored_images {

struct NTPReferralImagesData;

class NTPReferralImagesService {
 public:
  static void RegisterLocalStatePrefs(PrefRegistrySimple* registry);

  class Observer {
   public:
    // Called whenever ntp referral images component is updated.
    virtual void OnReferralImagesUpdated(NTPReferralImagesData* data) = 0;
   protected:
    virtual ~Observer() {}
  };

  NTPReferralImagesService(component_updater::ComponentUpdateService* cus,
                           PrefService* local_pref);
  virtual ~NTPReferralImagesService();

  NTPReferralImagesService(const NTPReferralImagesService&) = delete;
  NTPReferralImagesService& operator=(
      const NTPReferralImagesService&) = delete;

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);
  bool HasObserver(Observer* observer);

  NTPReferralImagesData* GetReferralImagesData() const;

 private:
  FRIEND_TEST_ALL_PREFIXES(NTPReferralImagesServiceTest, ImageSourceTest);
  FRIEND_TEST_ALL_PREFIXES(NTPReferralImagesServiceTest, InternalDataTest);
  FRIEND_TEST_ALL_PREFIXES(NTPReferralImagesServiceTest, MapperComponentTest);

  void OnPreferenceChanged();

  void CacheReferralComponentInfo();

  // Called when referrer component that has super referrer's assets is ready.
  void OnReferralComponentReady(const base::FilePath& installed_dir);
  void OnGetReferralJsonData(const base::FilePath& installed_dir,
                             const std::string& json);
  void NotifyObservers();

  // Initialize data with mapping data between super referrer and its referral
  // images component.
  void InitializeMappingTable();
  bool ShouldRegisterMapperComponent();
  // Called when mapper component is ready.
  void OnMapperComponentReady(const base::FilePath& installed_dir);
  void OnGetMappingJsonData(const std::string& json);
  // virtual for test.
  virtual void RegisterReferralComponent();

  std::string referral_code_;
  component_updater::ComponentUpdateService* cus_;
  // TODO(simonhong): Delete this before pushing PR.
  base::Value referral_component_info_;
  PrefService* local_pref_;
  PrefChangeRegistrar pref_change_registrar_;
  base::ObserverList<Observer>::Unchecked observer_list_;
  std::unique_ptr<NTPReferralImagesData> images_data_;
  base::WeakPtrFactory<NTPReferralImagesService> weak_factory_;
};

}  // namespace ntp_sponsored_images


#endif  // BRAVE_COMPONENTS_NTP_SPONSORED_IMAGES_BROWSER_NTP_REFERRAL_IMAGES_SERVICE_H_
