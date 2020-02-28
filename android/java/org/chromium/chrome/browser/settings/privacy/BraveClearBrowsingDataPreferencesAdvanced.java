/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.chromium.chrome.browser.settings.privacy;

import android.support.v4.util.ArraySet;

import androidx.annotation.DrawableRes;

import org.chromium.chrome.browser.browsing_data.BrowsingDataType;
import org.chromium.chrome.R;

import java.util.Arrays;
import java.util.List;
import java.util.Set;

public class BraveClearBrowsingDataPreferencesAdvanced extends ClearBrowsingDataPreferencesAdvanced {
    private static final int CLEAR_DOWNLOADS = DialogOption.NUM_ENTRIES;

    @Override
    protected List<Integer> getDialogOptions() {
        return Arrays.asList(DialogOption.CLEAR_HISTORY, DialogOption.CLEAR_COOKIES_AND_SITE_DATA,
                DialogOption.CLEAR_CACHE, DialogOption.CLEAR_PASSWORDS,
                DialogOption.CLEAR_FORM_DATA, DialogOption.CLEAR_SITE_SETTINGS, CLEAR_DOWNLOADS);
    }

    static Set<Integer> getBraveOptions() {
        Set<Integer> all = new ArraySet<>();
        all.add(CLEAR_DOWNLOADS);
        return all;
    }

    static boolean isValidDataType(int type) {
        return type == CLEAR_DOWNLOADS;
    }

    static @BrowsingDataType int getDataType(int type) {
        switch (type) {
            case CLEAR_DOWNLOADS:
                return BrowsingDataType.DOWNLOADS;
            default:
                throw new IllegalArgumentException();
        }
    }

    static String getPreferenceKey(int type) {
        switch (type) {
            case CLEAR_DOWNLOADS:
                return "clear_downloads_checkbox";
            default:
                throw new IllegalArgumentException();
        }
    }

    static @DrawableRes int getIcon(int type) {
        switch (type) {
            case CLEAR_DOWNLOADS:
                return R.drawable.ic_file_download_24dp;
            default:
                throw new IllegalArgumentException();
        }
    }
}
