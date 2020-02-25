/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>

#include "brave/components/ntp_sponsored_images/browser/ntp_referral_image_source.h"
#include "brave/components/ntp_sponsored_images/browser/ntp_referral_images_data.h"
#include "brave/components/ntp_sponsored_images/browser/ntp_referral_images_service.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace ntp_sponsored_images {

class TestObserver : public NTPReferralImagesService::Observer {
 public:
  TestObserver() = default;
  ~TestObserver() override = default;

  void OnReferralImagesUpdated(NTPReferralImagesData* data) override {
    called_ = true;
    data_ = data;
  }

  NTPReferralImagesData* data_ = nullptr;
  bool called_ = false;
};

TEST(NTPReferralImagesServiceTest, BasicTest) {
  NTPReferralImagesService service(nullptr, nullptr);

  // By default manager doesn't have data.
  EXPECT_EQ(service.GetReferralImagesData(), nullptr);
}

TEST(NTPReferralImagesServiceTest, InternalDataTest) {
  TestObserver observer;
  NTPReferralImagesService service(nullptr, nullptr);
  service.AddObserver(&observer);

  // Check with json file w/o schema version with empty object.
  service.images_data_.reset();
  service.OnGetReferralJsonData(base::FilePath(), "{}");
  EXPECT_EQ(nullptr, service.GetReferralImagesData());

  // Check with json file with empty object.
  const std::string test_empty_json_string = R"(
      {
          "schemaVersion": 1
      })";
  service.images_data_.reset();
  observer.called_ = false;
  observer.data_ = nullptr;
  service.OnGetReferralJsonData(base::FilePath(), test_empty_json_string);
  auto* data = service.GetReferralImagesData();
  EXPECT_EQ(nullptr, data);
  EXPECT_TRUE(observer.called_);
  EXPECT_TRUE(observer.data_->logo_alt_text.empty());

  const std::string test_json_string = R"(
    {
        "schemaVersion": 1,
        "logo": {
          "imageUrl":  "logo.png",
          "alt": "Technikke: For music lovers",
          "destinationUrl": "https://www.brave.com/",
          "companyName": "Technikke"
        },
        "wallpapers": [
          {
            "imageUrl": "background-1.jpg",
            "focalPoint": {}
          },
          {
            "imageUrl": "background-2.jpg",
            "focalPoint": {}
          },
          {
            "imageUrl": "background-3.jpg",
            "focalPoint": {}
          }
        ],
        "topSites": [
          {
            "name": "Shop at My Company",
            "destinationUrl": "https://www.company.com/shop",
            "iconUrl": "shop.png"
          }
        ]
    })";
  service.images_data_.reset();
  observer.called_ = false;
  observer.data_ = nullptr;
  service.OnGetReferralJsonData(base::FilePath(), test_json_string);
  data = service.GetReferralImagesData();
  EXPECT_TRUE(data);
  EXPECT_TRUE(data->IsValid());
  // Above json data has 3 wallpapers.
  const size_t wallpaper_count = 3;
  const size_t top_site_count = 1;
  EXPECT_EQ(wallpaper_count, data->wallpaper_image_urls().size());
  EXPECT_EQ(top_site_count, data->top_sites.size());
  EXPECT_TRUE(observer.called_);
  EXPECT_FALSE(observer.data_->logo_alt_text.empty());

  // Invalid schema version
  const std::string test_json_string_higher_schema = R"(
    {
        "schemaVersion": 2,
        "logo": {
          "imageUrl":  "logo.png",
          "alt": "Technikke: For music lovers",
          "destinationUrl": "https://www.brave.com/",
          "companyName": "Technikke"
        },
        "wallpapers": [
          {
            "imageUrl": "background-1.jpg",
            "focalPoint": {}
          },
          {
            "imageUrl": "background-2.jpg",
            "focalPoint": {}
          },
          {
            "imageUrl": "background-3.jpg",
            "focalPoint": {}
          }
        ]
    })";
  service.images_data_.reset();
  observer.called_ = false;
  observer.data_ = nullptr;
  service.OnGetReferralJsonData(base::FilePath(),
                                test_json_string_higher_schema);
  data = service.GetReferralImagesData();
  EXPECT_FALSE(data);

  service.RemoveObserver(&observer);
}

TEST(NTPReferralImagesServiceTest, ImageSourceTest) {
  NTPReferralImagesService service(nullptr, nullptr);

  const std::string test_json_string = R"(
    {
        "schemaVersion": 1,
        "logo": {
          "imageUrl":  "logo.png",
          "alt": "Technikke: For music lovers",
          "destinationUrl": "https://www.brave.com/",
          "companyName": "Technikke"
        },
        "wallpapers": [
          {
            "imageUrl": "background-1.jpg",
            "focalPoint": {}
          },
          {
            "imageUrl": "background-2.jpg",
            "focalPoint": {}
          },
          {
            "imageUrl": "background-3.jpg",
            "focalPoint": {}
          }
        ],
        "topSites": [
          {
            "name": "Shop at My Company",
            "destinationUrl": "https://www.company.com/shop",
            "iconUrl": "shop.png"
          }
        ]
    })";
  service.images_data_.reset();
  service.OnGetReferralJsonData(base::FilePath(), test_json_string);
  auto* data = service.GetReferralImagesData();
  EXPECT_TRUE(data);
  EXPECT_TRUE(data->IsValid());

  NTPReferralImageSource image_source(&service);
  EXPECT_TRUE(image_source.IsLogoPath("logo.png"));
  EXPECT_FALSE(image_source.IsLogoPath("logo1.png"));
  EXPECT_TRUE(image_source.IsIconPath("shop.png"));
  EXPECT_FALSE(image_source.IsIconPath("shop1.png"));
  // wallpaper file name pattern - wallpaper-N.jpg.
  EXPECT_TRUE(image_source.IsWallpaperPath("wallpaper-1.jpg"));
  EXPECT_FALSE(image_source.IsWallpaperPath("wallpaper-3.jpg"));
}

class TestNTPReferralImagesService : public NTPReferralImagesService {
 public:
  using NTPReferralImagesService::NTPReferralImagesService;

  void RegisterReferralComponent() override {
    register_requested_ = true;
  }

 bool register_requested_ = false;
};

TEST(NTPReferralImagesServiceTest, MapperComponentTest) {
  TestNTPReferralImagesService service(nullptr, nullptr);

  const std::string test_mapping_table = R"(
    {
        "schemaVersion": 1,
        "BRV001": {
          "publicKey": "ABCDEFGHIJKLMN",
          "componentID": "abcdefghijklmn",
          "companyName": "Alphabet software"
        },
        "BRV002": {
          "publicKey": "1234567890",
          "componentID": "0123456789",
          "companyName": "Numeric software"
        }
    })";

  service.referral_code_ = "BRV001";
  service.OnGetMappingJsonData(test_mapping_table);
  EXPECT_EQ("ABCDEFGHIJKLMN",
            *service.referral_component_info_.FindStringKey("publicKey"));
  EXPECT_TRUE(service.register_requested_);

  service.register_requested_ = false;
  service.referral_component_info_ = base::Value(base::Value::Type::DICTIONARY);
  service.referral_code_ = "BRV007";
  service.OnGetMappingJsonData(test_mapping_table);
  EXPECT_EQ(nullptr,
            service.referral_component_info_.FindStringKey("publicKey"));
  EXPECT_FALSE(service.register_requested_);
}

}  // namespace ntp_sponsored_images
