// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

#include "brave/components/ntp_sponsored_images/browser/ntp_referral_images_service.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/optional.h"
#include "base/task/post_task.h"
#include "brave/common/pref_names.h"
#include "brave/components/ntp_sponsored_images/browser/ntp_referral_images_data.h"
#include "brave/components/ntp_sponsored_images/browser/ntp_referral_component_installer.h"
#include "brave/components/ntp_sponsored_images/browser/ntp_referral_mapper_component_installer.h"
#include "brave/components/ntp_sponsored_images/common/pref_names.h"
#include "brave/components/ntp_sponsored_images/browser/switches.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace ntp_sponsored_images {

namespace {

constexpr char kJsonDataFilename[] = "data.json";
constexpr char kPublicKeyPath[] = "publicKey";
constexpr char kComponentID[] = "componentID";
constexpr char kComponentName[] = "companyName";

std::string ReadJsonFile(const base::FilePath& json_path) {
  std::string contents;
  bool success = base::ReadFileToString(json_path, &contents);
  if (!success || contents.empty()) {
    DVLOG(2) << __func__
             << " : cannot read data.json file " << json_path;
  }
  return contents;
}

bool IsValidComponentInfo(const base::Value& component_info) {
  if (!component_info.FindStringKey(kPublicKeyPath))
    return false;
  if (!component_info.FindStringKey(kComponentID))
    return false;
  if (!component_info.FindStringKey(kComponentName))
    return false;

  return true;
}

}  // namespace

void NTPReferralImagesService::RegisterLocalStatePrefs(
    PrefRegistrySimple* registry) {
  registry->RegisterDictionaryPref(prefs::kReferralImagesServiceComponent);
}

NTPReferralImagesService::NTPReferralImagesService(
    component_updater::ComponentUpdateService* cus,
    PrefService* local_pref)
    : cus_(cus),
      referral_component_info_(base::Value::Type::DICTIONARY),
      local_pref_(local_pref),
      weak_factory_(this) {
  if (!cus_)
    return;

  base::FilePath forced_local_path(base::CommandLine::ForCurrentProcess()->
      GetSwitchValueNative(switches::kNTPReferralDataPathForTesting));
  if (!forced_local_path.empty()) {
    DVLOG(2)
        << "NTP Referral Image package will be loaded from local path at: "
        << forced_local_path.LossyDisplayName();
    OnReferralComponentReady(forced_local_path);
    return;
  }

  if (ShouldRegisterMapperComponent()) {
    InitializeMappingTable();
    pref_change_registrar_.Init(local_pref_);
    pref_change_registrar_.Add(kReferralPromoCode,
        base::BindRepeating(&NTPReferralImagesService::OnPreferenceChanged,
        base::Unretained(this)));
  } else {
    CacheReferralComponentInfo();
    RegisterReferralComponent();
  }
}

NTPReferralImagesService::~NTPReferralImagesService() {}

void NTPReferralImagesService::AddObserver(Observer* observer) {
  observer_list_.AddObserver(observer);
}

void NTPReferralImagesService::RemoveObserver(Observer* observer) {
  observer_list_.RemoveObserver(observer);
}

bool NTPReferralImagesService::HasObserver(Observer* observer) {
  return observer_list_.HasObserver(observer);
}

NTPReferralImagesData* NTPReferralImagesService::GetReferralImagesData() const {
  // TODO(simonhong): This should not return nullptr before we can confirm that
  // this install is on-going super referrer.
  // When we can confirm?
  if (images_data_ && images_data_->IsValid())
    return images_data_.get();

  return nullptr;
}

void NTPReferralImagesService::OnPreferenceChanged() {
  referral_code_ = local_pref_->GetString(kReferralPromoCode);
  if (referral_code_.empty())
    return;

  // At first launch, any event can be happened first.
  // If mapping table is prepared already, register referral component here.
  // If not, referral component will be registered when mapping table is ready.
  if (IsValidComponentInfo(referral_component_info_))
    RegisterReferralComponent();
}

void NTPReferralImagesService::OnReferralComponentReady(
    const base::FilePath& installed_dir) {
  // image list is no longer valid after the component has been updated
  images_data_.reset();

  base::PostTaskAndReplyWithResult(
      FROM_HERE, {base::ThreadPool(), base::MayBlock()},
      base::BindOnce(&ReadJsonFile,
                     installed_dir.AppendASCII(kJsonDataFilename)),
      base::BindOnce(&NTPReferralImagesService::OnGetReferralJsonData,
                     weak_factory_.GetWeakPtr(),
                     installed_dir));
}

void NTPReferralImagesService::OnGetReferralJsonData(
    const base::FilePath& installed_dir,
    const std::string& json) {
  images_data_.reset(new NTPReferralImagesData(json, installed_dir));
  NotifyObservers();

  // TODO(simonhong): if |images_data_| is invalid, that means this referrer's
  // campaign is end. Handle it here.
}

void NTPReferralImagesService::NotifyObservers() {
  for (auto& observer : observer_list_)
    observer.OnReferralImagesUpdated(images_data_.get());
}

bool NTPReferralImagesService::ShouldRegisterMapperComponent() {
  if (local_pref_) {
    const base::Value* value =
        local_pref_->Get(prefs::kReferralImagesServiceComponent);
    return !IsValidComponentInfo(*value);
  }

  return false;
}

void NTPReferralImagesService::InitializeMappingTable() {
  RegisterNTPReferralMapperComponent(
      cus_,
      base::BindRepeating(&NTPReferralImagesService::OnMapperComponentReady,
                          weak_factory_.GetWeakPtr()));
}

void NTPReferralImagesService::OnMapperComponentReady(
    const base::FilePath& installed_dir) {
  base::PostTaskAndReplyWithResult(
      FROM_HERE, {base::ThreadPool(), base::MayBlock()},
      base::BindOnce(&ReadJsonFile,
                     installed_dir.AppendASCII(kJsonDataFilename)),
      base::BindOnce(&NTPReferralImagesService::OnGetMappingJsonData,
                     weak_factory_.GetWeakPtr()));
}

void NTPReferralImagesService::OnGetMappingJsonData(const std::string& json) {
  base::Optional<base::Value> mapping_table_value =
      base::JSONReader::Read(json);
  if (!mapping_table_value)
    return;

  // If code is empty, it means mapper component is downloaded before fetching
  // referrer code. In this case, referrer component will be registered when
  // it is fetched. Or, nothing happened if this install is not by super
  // referrer.
  if (referral_code_.empty())
    return;

  if (auto* value = mapping_table_value->FindDictKey(referral_code_)) {
    if (!IsValidComponentInfo(*value))
      return;
    referral_component_info_ = value->Clone();

    // Can be nullptr in test.
    if (local_pref_)
      local_pref_->Set(prefs::kReferralImagesServiceComponent, value->Clone());

    RegisterReferralComponent();
  }
}

void NTPReferralImagesService::RegisterReferralComponent() {
  RegisterNTPReferralComponent(
      cus_,
      *referral_component_info_.FindStringKey(kPublicKeyPath),
      *referral_component_info_.FindStringKey(kComponentID),
      *referral_component_info_.FindStringKey(kComponentName),
      base::BindRepeating(&NTPReferralImagesService::OnReferralComponentReady,
                          weak_factory_.GetWeakPtr()));
}

void NTPReferralImagesService::CacheReferralComponentInfo() {
  if (local_pref_) {
    const base::Value* value =
        local_pref_->Get(prefs::kReferralImagesServiceComponent);
    referral_component_info_ = value->Clone();
  }
}

}  // namespace ntp_sponsored_images
