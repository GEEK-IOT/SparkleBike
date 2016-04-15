package cn.geekiot.sparklebike;

import android.animation.Animator;
import android.app.ActivityOptions;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewAnimationUtils;
import android.view.Window;

import com.cobox.utils.SDK;

/**
 * MainActivity
 * @Auther Cocoonshu
 * @Date 2016-04-11 19:17:15
 */
public class MainActivity extends AppCompatActivity {
    public static final String TAG = "MainActivity";

    private Handler                 mAnimationHandler        = new Handler();
    private View                    mBackgroundRippleView    = null;
    private CoordinatorLayout       mCoordinatorLayout       = null;
    private Toolbar                 mToolBar                 = null;
    private CollapsingToolbarLayout mCollapsingToolbarLayout = null;
    private DrawerLayout            mDrawerLayout            = null;
    private ActionBarDrawerToggle   mActionBarDrawerToggle   = null;
    private FloatingActionButton    mFloatingActionButton    = null;

    private RecyclerView            mRecyclerView            = null;
    private RecyclerViewHelper      mRecyclerViewHelper      = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.main_activity);
        setupActionBarAsToolBar();
        findContentViews();
        setupViewListeners();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mActionBarDrawerToggle.syncState();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        mActionBarDrawerToggle.onConfigurationChanged(newConfig);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (mActionBarDrawerToggle.onOptionsItemSelected(item))
            return true;

        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        // noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void setupWindowStyle() {
        Window window = getWindow();
        Transition enterTransition = TransitionInflater.from(MainActivity.this).inflateTransition(R.transition.discover_device_activity_enter_anim);
        Transition exitTransition = TransitionInflater.from(MainActivity.this).inflateTransition(R.transition.discover_device_activity_exit_anim);
        Transition sharedElementEnterTransition = TransitionInflater.from(MainActivity.this).inflateTransition(R.transition.discover_device_activity_shared_element_enter_anim);
        Transition sharedElementExitTransition = TransitionInflater.from(MainActivity.this).inflateTransition(R.transition.discover_device_activity_shared_element_exit_anim);

        window.requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);
        window.setEnterTransition(enterTransition);
        window.setExitTransition(exitTransition);
        window.setSharedElementEnterTransition(sharedElementEnterTransition);
        window.setSharedElementExitTransition(sharedElementExitTransition);
        window.setAllowEnterTransitionOverlap(true);
        window.setAllowReturnTransitionOverlap(true);
    }

    private void findContentViews() {
        mRecyclerView         = (RecyclerView) findViewById(R.id.RecyclerView);
        mRecyclerViewHelper   = new RecyclerViewHelper(mRecyclerView);
        mRecyclerViewHelper.setupRecycleView();
        mRecyclerViewHelper.fillDataSet();

        mBackgroundRippleView    = findViewById(R.id.View_BackgroundRipple);
        mCoordinatorLayout       = (CoordinatorLayout) findViewById(R.id.CoordinatorLayout);
        mCollapsingToolbarLayout = (CollapsingToolbarLayout) findViewById(R.id.CollapsingToolbarLayout);
        mDrawerLayout            = (DrawerLayout) findViewById(R.id.DrawerLayout);
        mFloatingActionButton    = (FloatingActionButton) findViewById(R.id.FloatingActionButton);
        mActionBarDrawerToggle   = new ActionBarDrawerToggle(MainActivity.this, mDrawerLayout,
                R.string.MainActivity_DrawerLayoutShowDesc,
                R.string.MainActivity_DrawerLayoutHideDesc);
        mDrawerLayout.setDrawerListener(mActionBarDrawerToggle);
        mCollapsingToolbarLayout.setTitle("Cocoonshu");
    }

    private void setupViewListeners() {
        mFloatingActionButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                final Intent intent = new Intent(MainActivity.this, DiscoverDeviceActivity.class);
                if (SDK.isSupportedMaterialDesign()) {
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
                            startActivity(intent,
                                    ActivityOptions.makeSceneTransitionAnimation(
                                            MainActivity.this, mFloatingActionButton, getString(R.string.SharedElementName_FloatingActionButton)).toBundle());
                            mBackgroundRippleView.setVisibility(View.INVISIBLE);
                        }

                        @Override
                        public void onAnimationCancel(Animator animation) {
                            // Do Nothing
                        }

                        @Override
                        public void onAnimationRepeat(Animator animation) {
                            // Do Nothing
                        }

                    });
                    expandBackgroundAnimator.start();


                } else {
                    startActivity(intent);
                }
            }

        });
    }

    private void setupActionBarAsToolBar() {
        mToolBar = (Toolbar) findViewById(R.id.ToolBar);
        setSupportActionBar(mToolBar);
        getSupportActionBar().setHomeButtonEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }
}
