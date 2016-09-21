package cn.geekiot.sparklebike.theme;

import android.content.Context;

import com.stylingandroid.prism.filter.Filter;

/**
 * Theme
 * @Auther Cocoonshu
 * @Date 2016-09-14 14:01:03
 * Copyright (c) 2016 Cocoonshu
 */
public abstract class Theme {
    public abstract void   addTheme(Context context, String assets);
    public abstract void   addTheme(String file);
    public abstract String getThemeName(int index);
    public abstract int    get(String colorName);
    public abstract int    size();
    public abstract Filter createFilter(String colorName);
    public abstract void   applyTheme(String themeName);
}
