package cn.geekiot.sparklebike.theme;

import android.content.Context;
import android.support.annotation.NonNull;
import android.view.View;
import android.view.Window;
import android.widget.TextView;

import com.stylingandroid.prism.ColorSetter;
import com.stylingandroid.prism.Prism;
import com.stylingandroid.prism.Setter;
import com.stylingandroid.prism.SetterAdapter;
import com.stylingandroid.prism.Trigger;
import com.stylingandroid.prism.filter.Filter;
import com.stylingandroid.prism.setter.ColourSetterFactory;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * ThemeColorHelper
 * @Auther Cocoonshu
 * @Date 2016-09-14 14:01:03
 * Copyright (c) 2016 Cocoonshu
 */
public class ThemeColorHelper {
    public static final String TAG = "ThemeColorHelper";

    private Prism mThemePrism        = null;
    private int   mCurrentThemeIndex = -1;
    private Theme mThemePackage      = null;

    protected ThemeColorHelper() {

    }

    public static ThemeColorHelper createMaterialDesignThemeHelper(Context context) {
        ThemeColorHelper    helper = new ThemeColorHelper();
        MaterialDesignTheme theme  = new MaterialDesignTheme();
        theme.addTheme(context, "colors_blue_deep_orange.xml");
        theme.addTheme(context, "colors_blue_amber.xml");
        theme.addTheme(context, "colors_blue_grey_deep_orange.xml");
        theme.addTheme(context, "colors_brown_grey.xml");
        theme.addTheme(context, "colors_deep_purple_cyan.xml");
        theme.addTheme(context, "colors_indigo_purple.xml");
        theme.addTheme(context, "colors_green_red.xml");
        theme.addTheme(context, "colors_grey_blue_grey.xml");
        theme.addTheme(context, "colors_light_blue_indigo.xml");
        theme.addTheme(context, "colors_pink_red.xml");
        theme.addTheme(context, "colors_red_yellow.xml");
        theme.addTheme(context, "colors_teal_cyan.xml");
        helper.setTheme(theme);
        return helper;
    }

    protected void setTheme(Theme theme) {
        mThemePackage = theme;
    }

    public Builder buildPalette() {
        return new Builder(mThemePackage);
    }

    public void finishPaletteBuild(Prism prism) {
        mThemePrism = prism;
    }

    public int getCurrentThemeIndex() {
        return mCurrentThemeIndex;
    }

    public int getThemeSize() {
        return mThemePackage.size();
    }

    public int getColor(String colorName) {
        return mThemePackage.get(colorName);
    }

    public String triggerNextColor() {
        synchronized (mThemePackage) {
            int    themeSize = mThemePackage.size();
            String themeName = null;
            if (themeSize > 0) {
                mCurrentThemeIndex = (mCurrentThemeIndex + 1) % themeSize;
                themeName = mThemePackage.getThemeName(mCurrentThemeIndex);
                mThemePackage.applyTheme(themeName);

                /**
                 * Whatever the color is, Filter will get color from ThemePackage.
                 * But in order to effect the color changing, here should pass different value
                 * every time.
                 */
                mThemePrism.setColor(mCurrentThemeIndex);
            }
            return themeName;
        }
    }


    public static class Builder {
        private Prism.Builder mBuilder = null;
        private Theme         mTheme   = null;

        private Builder(Theme theme) {
            mTheme   = theme;
            mBuilder = Prism.Builder.newInstance();
        }

        public Builder background(@NonNull String colorName, @NonNull View view) {
            mBuilder.background(view, mTheme.createFilter(colorName));
            return this;
        }

        public Builder background(@NonNull String colorName, @NonNull Window window) {
            mBuilder.background(window, mTheme.createFilter(colorName));
            return this;
        }

        public Builder text(@NonNull String colorName, @NonNull TextView view) {
            mBuilder.text(view, mTheme.createFilter(colorName));
            return this;
        }

        public Builder color(@NonNull String colorName, @NonNull View view) {
            mBuilder.color(view, mTheme.createFilter(colorName));
            return this;
        }

        public Builder add(@NonNull String colorName, @NonNull Setter setter) {
            if (setter != null) {
                if (setter instanceof MaterialDesignTheme.ThemeColorSetter) {
                    ((MaterialDesignTheme.ThemeColorSetter)setter).setFilter(mTheme.createFilter(colorName));
                }
                mBuilder.add(setter);
            }
            return this;
        }

        public Builder add(@NonNull String colorName, @NonNull ColorSetter setter) {
            if (setter != null) {
                if (setter instanceof MaterialDesignTheme.ThemeColorSetter) {
                    ((MaterialDesignTheme.ThemeColorSetter)setter).setFilter(mTheme.createFilter(colorName));
                }
                mBuilder.add(new SetterAdapter(setter));
            }
            return this;
        }

        public Prism build() {
            return mBuilder.build();
        }
    }

}
