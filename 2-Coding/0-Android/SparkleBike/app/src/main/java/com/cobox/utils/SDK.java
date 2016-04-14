package com.cobox.utils;

import android.os.Build;

/**
 * SDK
 * @Auther Cocoonshu
 * @Date 2016-04-14 15:19
 * Copyright (c) 2016 Cocoonshu
 */
public class SDK {

    public static final int version() {
        return Build.VERSION.SDK_INT;
    }

    public static final boolean isSupportedMaterialDesign() {
        return Build.VERSION.SDK_INT >= 21;
    }

    public static final boolean isSupportedMaterialDesignTransition() {
        return Build.VERSION.SDK_INT >= 23;
    }
}
