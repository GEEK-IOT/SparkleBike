package cn.geekiot.sparklebike.page;

import android.animation.ObjectAnimator;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.PersistableBundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import cn.geekiot.sparklebike.R;

/**
 * Setting popup window
 * @Auther Cocoonshu
 * @Date 2016-09-29 14:04:59
 * Copyright (c) 2016 Cocoonshu
 */
public class SettingWindow extends AppCompatActivity {

    private Handler       mHandler          = new Handler();
    private ColorDrawable mWindowBackground = null;
    private View          mContentView      = null;
    private Toolbar       mToolbar          = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_setting);
        findViews();
    }

    @Override
    protected void onResume() {
        super.onResume();
        startToolBarRecolor();
    }

    private void startToolBarRecolor() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ColorDrawable toolBarBackground = (ColorDrawable)mToolbar.getBackground();
                ColorDrawable layoutBackground  = new ColorDrawable();
                int startColor = getResources().getColor(R.color.primary);
                int endColor   = getResources().getColor(R.color.accent);
                ObjectAnimator.ofArgb(toolBarBackground, "color", startColor, endColor)
                        .setDuration(500)
                        .start();
                ObjectAnimator.ofArgb(layoutBackground, "color", startColor, endColor)
                        .setDuration(500)
                        .start();
                mContentView.setBackground(layoutBackground);
            }
        });
    }

    private void setupWindowStyle() {
        Window window = getWindow();
        Transition enterTransition = TransitionInflater.from(SettingWindow.this).inflateTransition(R.transition.setting_window_show_anim);
        Transition exitTransition = TransitionInflater.from(SettingWindow.this).inflateTransition(R.transition.setting_window_hide_anim);

        window.requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);
        window.setEnterTransition(enterTransition);
        window.setExitTransition(exitTransition);
        window.setAllowEnterTransitionOverlap(true);
        window.setAllowReturnTransitionOverlap(true);
    }

    private void findViews() {
        mToolbar     = (Toolbar) findViewById(R.id.ToolBar);
        mContentView = findViewById(R.id.Layout_SettingPage);
    }
}
