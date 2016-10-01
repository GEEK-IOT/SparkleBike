package cn.geekiot.sparklebike.page;

import android.content.Intent;
import android.animation.ObjectAnimator;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.graphics.drawable.DrawerArrowDrawable;
import android.support.v7.widget.Toolbar;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;

import cn.geekiot.sparklebike.R;

/**
 * Setting popup window
 * @Auther Cocoonshu
 * @Date 2016-09-29 14:04:59
 * Copyright (c) 2016 Cocoonshu
 */
public class SettingWindow extends AppCompatActivity {

    public static final String TAG                  = "SettingWindow";
    public static final String KEY_NEED_LAUNCH_ANIM = "needLaunchAnimation";

    private Handler       mHandler          = new Handler();
    private ColorDrawable mWindowBackground = null;
    private View          mContentView      = null;
    private Toolbar       mToolbar          = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_setting);
        setupActionBar();
        findViews();
    }

    private void initializeIntent() {
        Intent  intent         = getIntent();
        boolean needLaunchAnim = false;
        if (intent != null) {
            needLaunchAnim = intent.getBooleanExtra(KEY_NEED_LAUNCH_ANIM, false);
        }

        if (needLaunchAnim) {
            startToolBarRecolor();
        } else {
            mContentView.setBackgroundColor(getResources().getColor(R.color.SettingWindowActivity_Background));
            mToolbar.setBackgroundColor(getResources().getColor(R.color.SettingWindowActivity_Background));
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        initializeIntent();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_setting_window, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            Animatable icon = (Animatable) item.getIcon();
            if (icon.isRunning()) {
                return true;
            } else {
                // Expend setting layout
                icon.start();
                return true;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    private void setupActionBar() {
        mToolbar = (Toolbar) findViewById(R.id.ToolBar);
        setSupportActionBar(mToolbar);

        ActionBar actionBar = getSupportActionBar();
        actionBar.setDisplayShowHomeEnabled(true);
        actionBar.setHomeButtonEnabled(true);
        actionBar.setDisplayHomeAsUpEnabled(true);

        DrawerArrowDrawable arrow = new DrawerArrowDrawable(mToolbar.getContext());
        actionBar.setHomeAsUpIndicator(arrow);
    }

    private void startToolBarRecolor() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ColorDrawable toolBarBackground = (ColorDrawable)mToolbar.getBackground();
                ColorDrawable layoutBackground  = new ColorDrawable();
                int startColor = getResources().getColor(R.color.primary);
                int endColor   = getResources().getColor(R.color.SettingWindowActivity_Background);
                ObjectAnimator.ofArgb(toolBarBackground, "color", startColor, endColor)
                        .setDuration(1000)
                        .start();
                ObjectAnimator.ofArgb(layoutBackground, "color", startColor, endColor)
                        .setDuration(1000)
                        .start();
                mContentView.setBackground(layoutBackground);
            }
        });
    }

    private void finishToolBarRecolor() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ColorDrawable toolBarBackground = (ColorDrawable)mToolbar.getBackground();
                ColorDrawable layoutBackground  = new ColorDrawable();
                int startColor = getResources().getColor(R.color.SettingWindowActivity_Background);
                int endColor   = getResources().getColor(R.color.primary);
                ObjectAnimator.ofArgb(toolBarBackground, "color", startColor, endColor)
                        .setDuration(400)
                        .start();
                ObjectAnimator.ofArgb(layoutBackground, "color", startColor, endColor)
                        .setDuration(400)
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

    @Override
    public void finishAfterTransition() {
        finishToolBarRecolor();
        super.finishAfterTransition();
    }
}
