package cn.geekiot.sparklebike;

import android.app.Application;

import cn.geekiot.sparklebike.theme.ThemeColorHelper;

/**
 * Colorable application
 * @Auther Cocoonshu
 * @Date 2016-10-03 14:19:24
 * Copyright (c) 2016 Cocoonshu
 */
public class ColorApplication extends Application {

    private ThemeColorHelper mThemeColorHelper = null;

    public synchronized ThemeColorHelper getThemeColorHelper() {
        if (mThemeColorHelper == null) {
            mThemeColorHelper = ThemeColorHelper.createMaterialDesignThemeHelper(getApplicationContext());
        }
        return mThemeColorHelper;
    }
}
