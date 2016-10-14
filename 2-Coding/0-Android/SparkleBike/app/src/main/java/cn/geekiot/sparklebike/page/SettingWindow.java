package cn.geekiot.sparklebike.page;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.ui.DialogWindow;

/**
 * Setting popup window
 * @Auther Cocoonshu
 * @Date 2016-09-29 14:04:59
 * Copyright (c) 2016 Cocoonshu
 */
public class SettingWindow extends DialogWindow {

    public static final String TAG = "SettingWindow";

    @Override
    protected int onMenuInflate() {
        return R.menu.menu_setting_window;
    }

    @Override
    protected int getColorMenuItemId() {
        return R.id.action_close;
    }

    @Override
    protected int getWindowInTransitionResource() {
        return R.transition.setting_window_fade_in_content;
    }

    @Override
    protected int getWindowOutTransitionResource() {
        return R.transition.setting_window_fade_out_content;
    }

    @Override
    protected int getWindowInTransitionLayout() {
        return R.layout.activity_setting_content;
    }

    @Override
    protected int getWindowOutTransitionLayout() {
        return R.layout.activity_scanning_placeholder;
    }

    @Override
    protected void onContentViewInflated() {

    }
}
