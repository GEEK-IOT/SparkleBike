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
import android.transition.Scene;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.transition.TransitionManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;

import cn.geekiot.sparklebike.R;

/**
 * Setting popup window
 * @Auther Cocoonshu
 * @Date 2016-09-29 14:04:59
 * Copyright (c) 2016 Cocoonshu
 */
public class SettingWindow extends AppCompatActivity {

    public static final String TAG                         = "SettingWindow";
    public static final String KEY_NEED_LAUNCH_ANIM        = "needLaunchAnimation";
    public static final String KEY_PAUSE_BACKGROUND_COLOR  = "pauseBackgroundColor";
    public static final String KEY_RESUME_BACKGROUND_COLOR = "resumeBackgroundColor";

    private Handler       mHandler               = new Handler();
    private ColorDrawable mWindowBackground      = null;
    private View          mContentView           = null;
    private ViewGroup     mSceneRoot             = null;
    private Toolbar       mToolbar               = null;
    public  int           mPauseBackgroundColor  = 0x00000000;
    public  int           mResumeBackgroundColor = 0x00000000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_setting);
        setupActionBar();
        findViews();
    }

    private void initializeIntent() {
        Intent  intent                = getIntent();
        boolean needLaunchAnim        = false;
        if (intent != null) {
            needLaunchAnim         = intent.getBooleanExtra(KEY_NEED_LAUNCH_ANIM, false);
            mPauseBackgroundColor  = intent.getIntExtra(KEY_PAUSE_BACKGROUND_COLOR, mPauseBackgroundColor);
            mResumeBackgroundColor = intent.getIntExtra(KEY_RESUME_BACKGROUND_COLOR, mResumeBackgroundColor);
        }

        if (needLaunchAnim) {
            startToolBarRecolor();
        } else {
            mContentView.setBackgroundColor(mResumeBackgroundColor);
            mToolbar.setBackgroundColor(mResumeBackgroundColor);
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
        MenuItem settingItem = menu.findItem(R.id.action_settings);
        Animatable icon = (Animatable) settingItem.getIcon();
        if (icon.isRunning()) {
            icon.stop();
        }
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
                startSettingWindowDelay();
                return true;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    private void startSettingWindowDelay() {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                finishAfterTransition();
            }
        }, 250);
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
                int startColor = mPauseBackgroundColor;
                int endColor   = mResumeBackgroundColor;
                ObjectAnimator.ofArgb(toolBarBackground, "color", startColor, endColor)
                        .setDuration(500)
                        .start();
                ObjectAnimator.ofArgb(layoutBackground, "color", startColor, endColor)
                        .setDuration(500)
                        .start();
                mContentView.setBackground(layoutBackground);
                scheduleSceneShowTranstion();
            }
        });
    }

    private void finishToolBarRecolor() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ColorDrawable toolBarBackground = (ColorDrawable)mToolbar.getBackground();
                ColorDrawable layoutBackground  = new ColorDrawable();
                int startColor = mResumeBackgroundColor;
                int endColor   = mPauseBackgroundColor;
                ObjectAnimator.ofArgb(toolBarBackground, "color", startColor, endColor)
                        .setDuration(400)
                        .start();
                ObjectAnimator.ofArgb(layoutBackground, "color", startColor, endColor)
                        .setDuration(400)
                        .start();
                mContentView.setBackground(layoutBackground);
                scheduleSceneHideTranstion();
            }
        });
    }

    private void scheduleSceneShowTranstion() {
        TransitionManager manager = TransitionInflater.from(SettingWindow.this).inflateTransitionManager(R.transition.setting_window_fade_in_content, mSceneRoot);
        Scene scene = Scene.getSceneForLayout(mSceneRoot, R.layout.activity_setting_content, SettingWindow.this);
        manager.transitionTo(scene);
    }

    private void scheduleSceneHideTranstion() {
        TransitionManager manager = TransitionInflater.from(SettingWindow.this).inflateTransitionManager(R.transition.setting_window_fade_out_content, mSceneRoot);
        Scene scene = Scene.getSceneForLayout(mSceneRoot, R.layout.activity_setting_placeholder, SettingWindow.this);
        manager.transitionTo(scene);
    }

    private void setupWindowStyle() {
        mResumeBackgroundColor = getResources().getColor(R.color.SettingWindowActivity_Background);
        mPauseBackgroundColor  = getResources().getColor(R.color.SettingWindowActivity_Background);

        Window window = getWindow();
        Transition enterTransition = TransitionInflater.from(SettingWindow.this).inflateTransition(R.transition.setting_window_show_anim);
        Transition exitTransition  = TransitionInflater.from(SettingWindow.this).inflateTransition(R.transition.setting_window_hide_anim);

        window.requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);
        window.setEnterTransition(enterTransition);
        window.setExitTransition(exitTransition);
        window.setAllowEnterTransitionOverlap(true);
        window.setAllowReturnTransitionOverlap(true);
    }

    private void findViews() {
        mToolbar     = (Toolbar) findViewById(R.id.ToolBar);
        mContentView = findViewById(R.id.Layout_SettingPage);
        mSceneRoot   = (ViewGroup) findViewById(R.id.Layout_ContentContainer);
    }

    @Override
    public void finishAfterTransition() {
        finishToolBarRecolor();
        super.finishAfterTransition();
    }
}
