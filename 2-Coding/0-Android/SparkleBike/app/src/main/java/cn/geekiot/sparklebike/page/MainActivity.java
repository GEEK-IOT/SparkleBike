package cn.geekiot.sparklebike.page;

import android.app.ActivityOptions;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.view.ViewPager;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;

import com.cobox.utils.SDK;

import java.util.ArrayList;
import java.util.List;

import cn.geekiot.sparklebike.R;

/**
 * MainActivity
 * @Auther Cocoonshu
 * @Date 2016-04-11 19:17:15
 */
public class MainActivity extends AppCompatActivity implements FragmentControlLinker {
    public static final String TAG = "MainActivity";

    private Handler                 mAnimationHandler        = new Handler();
    private View                    mBackgroundRippleView    = null;
    private CoordinatorLayout       mCoordinatorLayout       = null;
    private Toolbar                 mToolBar                 = null;
    private CollapsingToolbarLayout mCollapsingToolbarLayout = null;
    private DrawerLayout            mDrawerLayout            = null;
    private ActionBarDrawerToggle   mActionBarDrawerToggle   = null;
    private FloatingActionButton    mFloatingActionButton    = null;

    private InnerPagerAdapter           mPagerAdapter    = null;
    private ViewPager                   mFragmentPager   = null;
    private TabLayout                   mTabLayout       = null;
    private MainActivityDevicesFragment mDevicesFragment = null;
    private MainActivityGroupsFragment  mGroupsFragment  = null;
    private MainActivityAreasFragment   mAreasFragment   = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.main_activity);
        setupActionBarAsToolBar();
        findContentViews();
        setupViewListeners();
        setupFragment();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mActionBarDrawerToggle.syncState();
        mAnimationHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                mBackgroundRippleView.setVisibility(View.INVISIBLE);
            }
        }, 0);
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

        int id = item.getItemId();
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
        mBackgroundRippleView    = findViewById(R.id.View_BackgroundRipple);
        mCoordinatorLayout       = (CoordinatorLayout) findViewById(R.id.CoordinatorLayout);
        mCollapsingToolbarLayout = (CollapsingToolbarLayout) findViewById(R.id.CollapsingToolbarLayout);
        mDrawerLayout            = (DrawerLayout) findViewById(R.id.DrawerLayout);
        mFloatingActionButton    = (FloatingActionButton) findViewById(R.id.FloatingActionButton);
        mFragmentPager           = (ViewPager) findViewById(R.id.ViewPager_FragmentPager);
        mTabLayout               = (TabLayout) findViewById(R.id.TabLayout_tabStrip);
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
                startActivity(intent,
                        ActivityOptions.makeSceneTransitionAnimation(
                                MainActivity.this, mFloatingActionButton, getString(R.string.SharedElementName_FloatingActionButton)).toBundle());
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

    private void setupFragment() {
        List<Fragment> fragmentList = new ArrayList<Fragment>();

        mDevicesFragment = new MainActivityDevicesFragment();
        Bundle deviceBundle = new Bundle();
        mDevicesFragment.setArguments(deviceBundle);
        mDevicesFragment.setControlLinker(this);
        fragmentList.add(mDevicesFragment);
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_device_item));

        mGroupsFragment = new MainActivityGroupsFragment();
        Bundle groupBundle = new Bundle();
        mGroupsFragment.setArguments(groupBundle);
        mGroupsFragment.setControlLinker(this);
        fragmentList.add(mGroupsFragment);
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_group_item));

        mAreasFragment = new MainActivityAreasFragment();
        Bundle areaBundle = new Bundle();
        mGroupsFragment.setArguments(areaBundle);
        mGroupsFragment.setControlLinker(this);
        fragmentList.add(mAreasFragment);
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_area_item));

        mPagerAdapter = new InnerPagerAdapter(getSupportFragmentManager(), fragmentList);
        mFragmentPager.setAdapter(mPagerAdapter);
        mTabLayout.setupWithViewPager(mFragmentPager);
    }

    /**
     * Inner Pager Adapter
     * @Auther Cocoonshu
     * @Date 2016-06-12 18:03:21
     */
    private class InnerPagerAdapter extends FragmentPagerAdapter {

        private List<Fragment> mFragmentList = null;

        public InnerPagerAdapter(FragmentManager fragmentManager, List<Fragment> fragmentList) {
            super(fragmentManager);
            mFragmentList = fragmentList;
        }

        @Override
        public Fragment getItem(int position) {
            return mFragmentList == null ? null : mFragmentList.get(position);
        }

        @Override
        public int getCount() {
            return mFragmentList == null ? 0 : mFragmentList.size();
        }
    }
}
