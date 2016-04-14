package cn.geekiot.sparklebike;

import android.animation.Animator;
import android.graphics.Rect;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.view.View;
import android.view.ViewAnimationUtils;
import android.view.ViewGroup;
import android.view.Window;

public class DiscoverDeviceActivity extends AppCompatActivity {

    private View                 mBackgroundRippleView    = null;
    private FloatingActionButton mFloatingActionButton    = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_discover_device);
        findContentViews();
    }

    private void setupWindowStyle() {
        Window window = getWindow();
        Transition enterTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_enter_anim);
        Transition exitTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_exit_anim);
        Transition sharedElementEnterTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_shared_element_enter_anim);
        Transition sharedElementExitTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_shared_element_exit_anim);

        sharedElementEnterTransition.setEpicenterCallback(new Transition.EpicenterCallback() {
            @Override
            public Rect onGetEpicenter(Transition transition) {
                return null;
            }
        });

        window.requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);
//        window.setEnterTransition(enterTransition);
//        window.setExitTransition(exitTransition);
        window.setSharedElementEnterTransition(sharedElementEnterTransition);
        window.setSharedElementExitTransition(sharedElementExitTransition);
        window.setAllowEnterTransitionOverlap(true);
        window.setAllowReturnTransitionOverlap(true);

        sharedElementEnterTransition.addListener(new Transition.TransitionListener() {

            @Override
            public void onTransitionStart(Transition transition) {

            }

            @Override
            public void onTransitionEnd(Transition transition) {
                expandBackground();
            }

            @Override
            public void onTransitionCancel(Transition transition) {

            }

            @Override
            public void onTransitionPause(Transition transition) {

            }

            @Override
            public void onTransitionResume(Transition transition) {

            }

        });
    }

    private void expandBackground() {
        if (mFloatingActionButton != null && mBackgroundRippleView != null) {
            Rect floatActionButtonRect = new Rect();
            mFloatingActionButton.getGlobalVisibleRect(floatActionButtonRect);
            final Animator expandBackgroundAnimator = ViewAnimationUtils.createCircularReveal(
                    mBackgroundRippleView, floatActionButtonRect.centerX(), floatActionButtonRect.centerY(),
                    0, (int)Math.hypot(mBackgroundRippleView.getWidth(), mBackgroundRippleView.getHeight())
            );
            expandBackgroundAnimator.addListener(new Animator.AnimatorListener() {

                @Override
                public void onAnimationStart(Animator animation) {
                    mBackgroundRippleView.setVisibility(View.VISIBLE);
                }

                @Override
                public void onAnimationEnd(Animator animation) {

                }

                @Override
                public void onAnimationCancel(Animator animation) {

                }

                @Override
                public void onAnimationRepeat(Animator animation) {

                }

            });
            expandBackgroundAnimator.start();
        }
    }

    private void findContentViews() {
        mBackgroundRippleView = findViewById(R.id.View_BackgroundRipple);
        mFloatingActionButton = (FloatingActionButton)findViewById(R.id.FloatingActionButton);
    }
}
