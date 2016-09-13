package cn.geekiot.sparklebike.page;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Bundle;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.view.LayoutInflater;
import android.view.View;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import cn.geekiot.sparklebike.R;

public class HomeActivity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener,
                                                               FragmentControlLinker {

    private Toolbar                 mToolbar        = null;
    private CollapsingToolbarLayout mToolbarLayout  = null;
    private DrawerLayout            mDrawerLayout   = null;
    private ActionBarDrawerToggle   mDrawerToggle   = null;
    private NavigationView          mNavigationView = null;
    private FloatingActionButton    mFabAction      = null;

    private TabLayout                   mTabLayout       = null;
    private ViewPager                   mFragmentPager   = null;
    private InnerPagerAdapter           mPagerAdapter    = null;
    private MainActivityDevicesFragment mDevicesFragment = null;
    private MainActivityGroupsFragment  mGroupsFragment  = null;
    private MainActivityAreasFragment   mAreasFragment   = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);
        setupActionBar();
        findViews();
        setupListeners();
        setupLayout();
        setupFragment();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        mDrawerToggle.onConfigurationChanged(newConfig);
    }

    private void setupActionBar() {
        mToolbar = (Toolbar) findViewById(R.id.ToolBar);
        setSupportActionBar(mToolbar);
    }

    private void setupListeners() {
        mFabAction.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
    }

    private void findViews() {
        mFabAction      = (FloatingActionButton) findViewById(R.id.FloatingActionBar);
        mNavigationView = (NavigationView) findViewById(R.id.NavigationView);
        mFragmentPager  = (ViewPager) findViewById(R.id.ViewPager_FragmentPager);
        mTabLayout      = (TabLayout) findViewById(R.id.TabLayout_tabStrip);
        mToolbarLayout  = (CollapsingToolbarLayout) findViewById(R.id.CollapsingToolbarLayout);
        mDrawerLayout   = (DrawerLayout) findViewById(R.id.drawer_layout);
        mDrawerToggle   = new ActionBarDrawerToggle(
                this, mDrawerLayout, mToolbar,
                R.string.navigation_drawer_open, R.string.navigation_drawer_close);
    }

    private void setupLayout() {
        mDrawerLayout.setDrawerListener(mDrawerToggle);
        mDrawerToggle.syncState();
        mNavigationView.setNavigationItemSelectedListener(this);
    }

    private void setupFragment() {
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_device_item));
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_group_item));
        mTabLayout.addTab(mTabLayout.newTab().setCustomView(R.layout.tabitem_main_activity_content_layout_area_item));

        mPagerAdapter = new InnerPagerAdapter(getSupportFragmentManager(), HomeActivity.this);
        mFragmentPager.setAdapter(mPagerAdapter);
        mTabLayout.setupWithViewPager(mFragmentPager);
        mPagerAdapter.setTabLayout(mTabLayout);
    }

    @Override
    public void onBackPressed() {
        if (mDrawerLayout.isDrawerOpen(GravityCompat.START)) {
            mDrawerLayout.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.home, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.nav_camera) {

        } else if (id == R.id.nav_gallery) {

        } else if (id == R.id.nav_slideshow) {

        } else if (id == R.id.nav_manage) {

        } else if (id == R.id.nav_share) {

        } else if (id == R.id.nav_send) {

        }

        mDrawerLayout.closeDrawer(GravityCompat.START);
        return true;
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
                case POSITION_GROUP_PAGE:  return "group";
                case POSITION_AREA_PAGE:   return "area";
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
