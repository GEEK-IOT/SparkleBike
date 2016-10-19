package cn.geekiot.sparklebike.ui;

import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.content.Intent;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBar;
import android.support.v7.graphics.drawable.DrawerArrowDrawable;
import android.support.v7.widget.Toolbar;
import android.transition.Scene;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.transition.TransitionManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;

import cn.geekiot.sparklebike.ColorActivity;
import cn.geekiot.sparklebike.R;

/**
 * Setting popup window
 * @Auther Cocoonshu
 * @Date 2016-09-29 14:04:59
 * Copyright (c) 2016 Cocoonshu
 */
public abstract class DialogWindow extends ColorActivity {
    public static final String TAG                         = "DialogWindow";
    public static final String KEY_NEED_LAUNCH_ANIM        = "needLaunchAnimation";
    public static final String KEY_PAUSE_BACKGROUND_COLOR  = "pauseBackgroundColor";
    public static final String KEY_RESUME_BACKGROUND_COLOR = "resumeBackgroundColor";
    public static final String KEY_PAUSE_STATUSBAR_COLOR   = "pauseStatusBarColor";
    public static final String KEY_RESUME_STATUSBAR_COLOR  = "resumeStatusBarColor";

    private Handler       mHandler               = new Handler();
    private ColorDrawable mWindowBackground      = null;
    private View          mContentView           = null;
    private ViewGroup     mSceneRoot             = null;
    private Toolbar       mToolbar               = null;
    public  int           mPauseBackgroundColor  = 0x00000000;
    public  int           mResumeBackgroundColor = 0x00000000;
    public  int           mPauseStatusBarColor   = 0x00000000;
    public  int           mResumeStatusBarColor  = 0x00000000;

    abstract protected int  onMenuInflate();
    abstract protected void onContentViewInflated();
    abstract protected int  getColorMenuItemId();
    abstract protected int  getWindowInTransitionResource();
    abstract protected int  getWindowOutTransitionResource();
    abstract protected int  getWindowInTransitionLayout();
    abstract protected int  getWindowOutTransitionLayout();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_dialog_window);
        setupActionBar();
        findViews();
    }

    private void initializeIntent() {
        Intent intent         = getIntent();
        boolean needLaunchAnim = false;
        if (intent != null) {
            needLaunchAnim         = intent.getBooleanExtra(KEY_NEED_LAUNCH_ANIM, false);
            mPauseBackgroundColor  = intent.getIntExtra(KEY_PAUSE_BACKGROUND_COLOR, mPauseBackgroundColor);
            mResumeBackgroundColor = intent.getIntExtra(KEY_RESUME_BACKGROUND_COLOR, mResumeBackgroundColor);
            mPauseStatusBarColor   = intent.getIntExtra(KEY_PAUSE_STATUSBAR_COLOR, mPauseStatusBarColor);
            mResumeStatusBarColor  = intent.getIntExtra(KEY_RESUME_STATUSBAR_COLOR, mResumeStatusBarColor);

            Log.i(TAG, String.format("[initializeIntent] Background (#%08X -> #%08X)", mPauseBackgroundColor, mResumeBackgroundColor));
            Log.i(TAG, String.format("[initializeIntent] StatusBar  (#%08X -> #%08X)", mPauseStatusBarColor, mResumeStatusBarColor));
        }

        if (needLaunchAnim) {
            startToolBarRecolor();
        } else {
            mContentView.setBackgroundColor(mResumeBackgroundColor);
            mToolbar.setBackgroundColor(mResumeBackgroundColor);
            getWindow().setStatusBarColor(mResumeBackgroundColor);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        initializeIntent();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(onMenuInflate(), menu);
        MenuItem closeItem     = menu.findItem(getColorMenuItemId());
        Drawable closeItemIcon = closeItem.getIcon();
        if (closeItemIcon instanceof Animatable) {
            Animatable icon = (Animatable) closeItem.getIcon();
            if (icon.isRunning()) {
                icon.stop();
            }
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == getColorMenuItemId()) {
            Animatable icon = (Animatable) item.getIcon();
            if (icon.isRunning()) {
                return true;
            } else {
                // Expend setting layout
                icon.start();
                finishSettingWindowDelay();
                return true;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    private void finishSettingWindowDelay() {
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
                int startColor       = mPauseBackgroundColor;
                int endColor         = mResumeBackgroundColor;
                int startWindowColor = mPauseStatusBarColor;
                int endWindowColor   = mResumeStatusBarColor;
                ObjectAnimator.ofArgb(toolBarBackground, "color", startColor, endColor)
                        .setDuration(500)
                        .start();
                ObjectAnimator.ofArgb(layoutBackground, "color", startColor, endColor)
                        .setDuration(500)
                        .start();
                ValueAnimator animator = ValueAnimator.ofArgb(startWindowColor, endWindowColor);
                animator.setDuration(500);
                animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                    @Override
                    public void onAnimationUpdate(ValueAnimator animation) {
                        getWindow().setStatusBarColor((Integer) animation.getAnimatedValue());
                    }
                });
                animator.start();
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
                int startColor       = mResumeBackgroundColor;
                int endColor         = mPauseBackgroundColor;
                int startWindowColor = mResumeStatusBarColor;
                int endWindowColor   = mPauseStatusBarColor;
                ObjectAnimator.ofArgb(toolBarBackground, "color", startColor, endColor)
                        .setDuration(400)
                        .start();
                ObjectAnimator.ofArgb(layoutBackground, "color", startColor, endColor)
                        .setDuration(400)
                        .start();
                ValueAnimator animator = ValueAnimator.ofArgb(startWindowColor, endWindowColor);
                animator.setDuration(400);
                animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                    @Override
                    public void onAnimationUpdate(ValueAnimator animation) {
                        getWindow().setStatusBarColor((Integer) animation.getAnimatedValue());
                    }
                });
                animator.start();

                mContentView.setBackground(layoutBackground);
                scheduleSceneHideTranstion();
            }
        });
    }

    private void scheduleSceneShowTranstion() {
        Transition transition = TransitionInflater.from(DialogWindow.this).inflateTransition(getWindowInTransitionResource());
        Scene scene = Scene.getSceneForLayout(mSceneRoot, getWindowInTransitionLayout(), DialogWindow.this);
        TransitionManager.go(scene, transition);
        onContentViewInflated();
    }

    private void scheduleSceneHideTranstion() {
        Transition transition = TransitionInflater.from(DialogWindow.this).inflateTransition(getWindowOutTransitionResource());
        Scene scene = Scene.getSceneForLayout(mSceneRoot, getWindowOutTransitionLayout(), DialogWindow.this);
        TransitionManager.go(scene, transition);
    }

    private void setupWindowStyle() {
        mResumeBackgroundColor = getResources().getColor(R.color.DialogWindowActivity_Background);
        mPauseBackgroundColor  = getResources().getColor(R.color.DialogWindowActivity_Background);

        int enterTransitionResid = getWindowStyleEnterTransition();
        int exitTransitionResid  = getWindowStyleExitTransition();

        Window window = getWindow();
        Transition enterTransition = enterTransitionResid == 0 ?
                null : TransitionInflater.from(DialogWindow.this).inflateTransition(enterTransitionResid);
        Transition exitTransition  = exitTransitionResid == 0 ?
                null : TransitionInflater.from(DialogWindow.this).inflateTransition(exitTransitionResid);

        window.requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);
        window.setEnterTransition(enterTransition);
        window.setExitTransition(exitTransition);
        window.setAllowEnterTransitionOverlap(true);
        window.setAllowReturnTransitionOverlap(true);
    }

    protected int getWindowStyleEnterTransition() {
        return R.transition.dialog_window_show_anim;
    }

    protected int getWindowStyleExitTransition() {
        return R.transition.dialog_window_hide_anim;
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

