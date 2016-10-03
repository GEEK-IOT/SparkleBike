package cn.geekiot.sparklebike;

import android.app.Application;
import android.support.v7.app.AppCompatActivity;

import cn.geekiot.sparklebike.theme.ThemeColorHelper;

/**
 * Colorable application
 * @Auther Cocoonshu
 * @Date 2016-10-03 14:19:24
 * Copyright (c) 2016 Cocoonshu
 */
public class ColorActivity extends AppCompatActivity {

    protected ThemeColorHelper getThemeColorHelper() {
        Application app = getApplication();
        if (app instanceof ColorApplication) {
            return ((ColorApplication)app).getThemeColorHelper();
        } else {
            return null;
        }
    }

}
