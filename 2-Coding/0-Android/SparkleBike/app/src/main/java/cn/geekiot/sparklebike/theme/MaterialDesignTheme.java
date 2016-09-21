package cn.geekiot.sparklebike.theme;

import android.content.Context;
import android.content.res.Resources;
import android.support.annotation.ColorInt;
import android.util.Pair;
import android.view.View;

import com.stylingandroid.prism.Setter;
import com.stylingandroid.prism.filter.Filter;

import java.util.IdentityHashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * Material Design theme wrapper
 * @Auther Cocoonshu
 * @Date 2016-09-14 14:01:03
 * Copyright (c) 2016 Cocoonshu
 */
public class MaterialDesignTheme extends Theme {
    public static final String PRIMARY        = "primary";
    public static final String PRIMARY_DARK   = "primary_dark";
    public static final String PRIMARY_LIGHT  = "primary_light";
    public static final String ACCENT         = "accent";
    public static final String PRIMARY_TEXT   = "primary_text";
    public static final String SECONDARY_TEXT = "secondary_text";
    public static final String ICONS          = "icons";
    public static final String DIVIDER        = "divider";
    public static final String RIPPLE_COLOR   = "ripple_color";
    public static final String BACKGROUND     = "background";

    public static final String [] KEY_ARRAYS = {
            PRIMARY,
            PRIMARY_DARK,
            PRIMARY_LIGHT,
            ACCENT,
            PRIMARY_TEXT,
            SECONDARY_TEXT,
            ICONS,
            DIVIDER,
            RIPPLE_COLOR,
            BACKGROUND
    };

    private Map<String, Map<String, Integer>>  mThemeList    = null;
    private ThemeParser<String, Integer>       mThemeParser  = null;
    private Pair<String, Map<String, Integer>> mCurrentTheme = null;

    public MaterialDesignTheme() {
        mThemeParser = new MaterialDesignThemeParser();
        mThemeList   = new IdentityHashMap<String, Map<String, Integer>>();
    }

    @Override
    public void addTheme(Context context, String assets) {
        Map<String, Integer> theme = mThemeParser.parseThemeAssetsResource(context, assets);
        if (theme != null) {
            int firstPlace = assets.lastIndexOf("/");
            int lastPlace  = assets.lastIndexOf(".");
            firstPlace  = firstPlace < 0 ? 0 : firstPlace;
            lastPlace   = lastPlace <= firstPlace ? assets.length() : lastPlace;
            String name = assets.substring(firstPlace, lastPlace);
            mThemeList.put(name, theme);

            if (mCurrentTheme == null) {
                mCurrentTheme = new Pair<String, Map<String, Integer>>(name, theme);
            }
        }
    }

    @Override
    public void addTheme(String file) {
        Map<String, Integer> theme = mThemeParser.parseThemeFileResource(file);
        if (theme != null) {
            int firstPlace = file.lastIndexOf("/");
            int lastPlace  = file.lastIndexOf(".");
            lastPlace = lastPlace <= firstPlace ? file.length() : lastPlace;
            String name = file.substring(firstPlace, lastPlace);
            mThemeList.put(name, theme);

            if (mCurrentTheme == null) {
                mCurrentTheme = new Pair<String, Map<String, Integer>>(name, theme);
            }
        }
    }

    @Override
    public int get(String colorName) {
        return mCurrentTheme.second.get(colorName);
    }

    @Override
    public String getThemeName(int index) {
        if (index < 0 || index > size()) {
            return null;
        }
        Iterator<String> iterator = mThemeList.keySet().iterator();
        for (int i = 0; i <= index && iterator.hasNext(); i++) {
            String name = iterator.next();
            if (i == index) {
                return name;
            }
        }
        return null;
    }

    @Override
    public int size() {
        return mThemeList.size();
    }

    @Override
    public Filter createFilter(String colorName) {
        return new ThemeColorFilter(colorName);
    }

    @Override
    public void applyTheme(String themeName) {
        Map theme = mThemeList.get(themeName);
        if (theme != null) {
            mCurrentTheme = new Pair<String, Map<String, Integer>>(themeName, theme);
        } else {
            throw new Resources.NotFoundException(themeName + " isn't found");
        }
    }

    public static boolean hasAttributeKey(String key) {
        if (key == null) {
            return false;
        } else {
            for (String refKey : KEY_ARRAYS) {
                if (refKey.equalsIgnoreCase(key)) {
                    return true;
                }
            }
            return false;
        }
    }

    public class ThemeColorFilter implements Filter {

        private String mColorName = null;

        public ThemeColorFilter(String name) {
            mColorName = name;
        }

        @Override
        public Integer filter(Integer colour) {
            return mCurrentTheme.second.get(mColorName);
        }
    }

    public static abstract class ThemeColorSetter implements Setter {

        private Filter mFilter = null;

        protected void setFilter(Filter filter) {
            mFilter = filter;
        }

        public abstract void setThemeColor(@ColorInt int colour);
        public abstract void setThemeTransientColor(@ColorInt int colour);

        @Override
        public void setColour(@ColorInt int colour) {
            setThemeColor(mFilter.filter(colour));
        }

        @Override
        public void setTransientColour(@ColorInt int colour) {
            setThemeTransientColor(mFilter.filter(colour));
        }
    }
}

