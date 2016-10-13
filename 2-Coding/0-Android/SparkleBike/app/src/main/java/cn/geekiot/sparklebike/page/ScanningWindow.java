package cn.geekiot.sparklebike.page;

import android.graphics.Rect;
import android.transition.Explode;
import android.transition.Transition;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.ui.DialogWindow;

/**
 * Scan smart device window
 * @Auther Cocoonshu
 * @Date 2016-10-03 15:56:08
 * Copyright (c) 2016 Cocoonshu
 */
public class ScanningWindow extends DialogWindow {

    public static final String TAG = "ScanningWindow";

    @Override
    protected int onMenuInflate() {
        return R.menu.menu_scanning_window;
    }

    @Override
    protected int getColorMenuItemId() {
        return R.id.action_close;
    }

    @Override
    protected int getWindowInTransitionResource() {
        return R.transition.scanning_window_fade_in_content;
    }

    @Override
    protected int getWindowOutTransitionResource() {
        return R.transition.scanning_window_fade_out_content;
    }

    @Override
    protected int getWindowInTransitionLayout() {
        return R.layout.activity_scanning_content;
    }

    @Override
    protected int getWindowOutTransitionLayout() {
        return R.layout.activity_scanning_placeholder;
    }

    @Override
    protected int getWindowStyleEnterTransition() {
        return R.transition.scanning_window_show_anim;
    }

    @Override
    protected int getWindowStyleExitTransition() {
        return R.transition.scanning_window_hide_anim;
    }

}
