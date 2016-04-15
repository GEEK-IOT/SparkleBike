package cn.geekiot.sparklebike;

import android.animation.Animator;
import android.app.Activity;
import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Rect;
import android.os.IBinder;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewAnimationUtils;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.FrameLayout;

import com.cobox.cosmart.devicebridge.DeviceBridgeService;

public class DiscoverDeviceActivity extends AppCompatActivity {


    private ViewGroup               mRootLayout                            = null;
    private Toolbar                 mToolBar                               = null;
    private CollapsingToolbarLayout mCollapsingToolbarLayout               = null;
    private RecyclerView            mFoundDeviceListView                   = null;
    private FloatingActionButton    mFloatingActionButton                  = null;
    private ServiceConnection       mDeviceBridgeServiceConnectionListener = null;
    private DeviceBridgeService     mDeviceBridgeService                   = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_discover_device);
        setupActionBarAsToolBar();
        findContentViews();
        setupListeners();
    }

    @Override
    protected void onResume() {
        super.onResume();
        DeviceBridgeService.bindDeviceBridgeService(
                DiscoverDeviceActivity.this,
                mDeviceBridgeServiceConnectionListener
        );
    }

    @Override
    protected void onPause() {
        super.onPause();
        DeviceBridgeService.unbindDeviceBridgeService(
                DiscoverDeviceActivity.this,
                mDeviceBridgeServiceConnectionListener
        );
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        // noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        } else if (id == android.R.id.home) {
            finish();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void setupWindowStyle() {
        Window window = getWindow();
        Transition enterTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_enter_anim);
        Transition exitTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_exit_anim);
        Transition sharedElementEnterTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_shared_element_enter_anim);
        Transition sharedElementExitTransition = TransitionInflater.from(DiscoverDeviceActivity.this).inflateTransition(R.transition.discover_device_activity_shared_element_exit_anim);

        window.requestFeature(Window.FEATURE_CONTENT_TRANSITIONS);
        window.setEnterTransition(enterTransition);
        window.setExitTransition(exitTransition);
        window.setSharedElementEnterTransition(sharedElementEnterTransition);
        window.setSharedElementExitTransition(sharedElementExitTransition);
        window.setAllowEnterTransitionOverlap(true);
        window.setAllowReturnTransitionOverlap(true);
    }

    private void setupActionBarAsToolBar() {
        mToolBar = (Toolbar) findViewById(R.id.ToolBar);
        setSupportActionBar(mToolBar);
        getSupportActionBar().setHomeButtonEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }

    private void findContentViews() {
        mRootLayout              = (FrameLayout) findViewById(R.id.FrameLayout_RootLayout);
        mFoundDeviceListView     = (RecyclerView) findViewById(R.id.RecyclerView_FoundDeviceList);
        mCollapsingToolbarLayout = (CollapsingToolbarLayout) findViewById(R.id.CollapsingToolbarLayout);
        mFloatingActionButton    = (FloatingActionButton) findViewById(R.id.FloatingActionButton);
        mCollapsingToolbarLayout.setTitle("Scanning...");
        mCollapsingToolbarLayout.setExpandedTitleColor(0xFFFFFFFF);
    }

    private void setupListeners() {
        mDeviceBridgeServiceConnectionListener = new ServiceConnection() {

            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                mDeviceBridgeService = ((DeviceBridgeService.DeviceBridgeServiceBinder) service).getService();
                Snackbar.make(mRootLayout, "Connected to " + mDeviceBridgeService.getClass().getSimpleName(), Snackbar.LENGTH_SHORT).show();
            }

            @Override
            public void onServiceDisconnected(ComponentName name) {
                Snackbar.make(mRootLayout, "Disconnected from " + mDeviceBridgeService.getClass().getSimpleName(), Snackbar.LENGTH_SHORT).show();
                mDeviceBridgeService = null;
            }

        };
    }

    private void discoverDevice() {
        if (mDeviceBridgeService != null) {
            mDeviceBridgeService.discoveryDevice();
            // TODO: 2016-04-15
        }
    }
}
