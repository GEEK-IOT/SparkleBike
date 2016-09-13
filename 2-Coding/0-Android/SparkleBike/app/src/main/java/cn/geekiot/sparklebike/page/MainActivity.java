package cn.geekiot.sparklebike.page;

import android.app.ActivityOptions;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.VectorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.TabLayout;
import android.support.graphics.drawable.AnimatedVectorDrawableCompat;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.TextView;

import com.cobox.utils.SDK;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.ui.WiFiEnableDialog;

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
        } else if (id == R.id.action_home_router) {
            Drawable icon = item.getIcon();
            if (icon instanceof AnimatedVectorDrawableCompat) {
                AnimatedVectorDrawableCompat vectorIcon = (AnimatedVectorDrawableCompat) icon;
                vectorIcon.start();
                // TODO Show a router chooser window
//                WiFiEnableDialog.createDialog(MainActivity.this, new DialogInterface.OnClickListener() {
//                    @Override
//                    public void onClick(DialogInterface dialog, int which) {
//                        dialog.dismiss();
//                    }
//                }).show();
            }
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
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_device_item));
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_group_item));
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_area_item));

        mPagerAdapter = new InnerPagerAdapter(getSupportFragmentManager(), MainActivity.this);
        mFragmentPager.setAdapter(mPagerAdapter);
        mTabLayout.setupWithViewPager(mFragmentPager);
        mPagerAdapter.setTabLayout(mTabLayout);
    }

    /**
     * Inner Pager Adapter
     * @Auther Cocoonshu
     * @Date 2016-06-12 18:03:21
     */
    private class InnerPagerAdapter extends FragmentPagerAdapter {

        public static final int   POSITION_DEVICE_PAGE = 0;
        public static final int   POSITION_GROUP_PAGE  = 1;
        public static final int   POSITION_AREA_PAGE   = 2;
        public static final int   PAGE_SIZE            = 3;
        public        final int   TAB_LAYOUT_TEMPLATE  = R.layout.main_activity_content_layout_tabitem;
        public        final int[] TAB_ITEM_UNIT_RESIDS = {
                R.plurals.MainActivity_TabLayout_Unit_Device,
                R.plurals.MainActivity_TabLayout_Unit_Group,
                R.plurals.MainActivity_TabLayout_Unit_Area
        };

        private TabLayout             mTabLayout     = null;
        private FragmentControlLinker mControlLinker = null;

        public InnerPagerAdapter(FragmentManager fragmentManager, FragmentControlLinker controlLinker) {
            super(fragmentManager);
            mControlLinker = controlLinker;
        }

        @Override
        public CharSequence getPageTitle(int position) {
            switch (position) {
                case POSITION_DEVICE_PAGE: return "device";
                case POSITION_GROUP_PAGE: return "group";
                case POSITION_AREA_PAGE: return "area";
                default:return "xx";
            }
        }

        @Override
        public Fragment getItem(int position) {
            Fragment targetFragment = null;
            switch (position) {
                case POSITION_DEVICE_PAGE:
                    Bundle deviceBundle = new Bundle();
                    mDevicesFragment = new MainActivityDevicesFragment();
                    mDevicesFragment.setArguments(deviceBundle);
                    mDevicesFragment.setControlLinker(mControlLinker);
                    targetFragment = mDevicesFragment;
                    break;

                case POSITION_GROUP_PAGE:
                    Bundle groupBundle = new Bundle();
                    mGroupsFragment = new MainActivityGroupsFragment();
                    mGroupsFragment.setArguments(groupBundle);
                    mGroupsFragment.setControlLinker(mControlLinker);
                    targetFragment = mGroupsFragment;
                    break;

                case POSITION_AREA_PAGE:
                    Bundle areaBundle = new Bundle();
                    mAreasFragment = new MainActivityAreasFragment();
                    mAreasFragment.setArguments(areaBundle);
                    mAreasFragment.setControlLinker(mControlLinker);
                    targetFragment = mAreasFragment;
                    break;
            }
            return targetFragment;
        }

        @Override
        public int getCount() {
            return PAGE_SIZE;
        }

        public void setTabLayout(TabLayout tabLayout) {
            mTabLayout = tabLayout;

            int            tabCount = mTabLayout.getTabCount();
            Context        context  = mTabLayout.getContext();
            LayoutInflater inflater = LayoutInflater.from(context);
            for (int i = 0; i < tabCount; i++) {
                TabLayout.Tab tab = mTabLayout.getTabAt(i);
                if (tab != null) {
                    View     customView = inflater.inflate(TAB_LAYOUT_TEMPLATE, null, false);
                    TextView unitView   = (TextView) customView.findViewById(R.id.TextView_Unit);
                    unitView.setText(context.getResources().getQuantityText(TAB_ITEM_UNIT_RESIDS[i], 0));
                    tab.setCustomView(customView);
                }
            }
        }
    }
}
