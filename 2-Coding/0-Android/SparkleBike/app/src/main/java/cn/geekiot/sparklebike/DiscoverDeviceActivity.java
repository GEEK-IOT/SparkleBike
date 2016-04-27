package cn.geekiot.sparklebike;

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
import android.view.ViewGroup;
import android.view.Window;
import android.widget.FrameLayout;

import com.cobox.cosmart.devicebridge.Device;
import com.cobox.cosmart.devicebridge.DeviceBridge;
import com.cobox.cosmart.devicebridge.listeners.OnBridgeConnectionListener;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;

import java.util.List;

import cn.geekiot.sparklebike.ui.DiscoverDeviceAnimView;

public class DiscoverDeviceActivity extends AppCompatActivity {

    private ViewGroup                  mRootLayout                         = null;
    private Toolbar                    mToolBar                            = null;
    private CollapsingToolbarLayout    mCollapsingToolbarLayout            = null;
    private RecyclerView               mFoundDeviceListView                = null;
    private FloatingActionButton       mFloatingActionButton               = null;
    private DiscoverDeviceAnimView     mDiscoverDeviceAnimView             = null;
    private DeviceBridge               mDeviceBridge                       = new DeviceBridge();
    private OnDeviceScanListener       mOnDeviceScanListener               = null;
    private OnBridgeConnectionListener mOnBridgeConnectionListener         = null;

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
        mDeviceBridge.connectService(DiscoverDeviceActivity.this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mDeviceBridge.disconnectService(DiscoverDeviceActivity.this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        } else if (id == android.R.id.home) {
            finish();
            return true;
        } else {
            return super.onOptionsItemSelected(item);
        }
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
        mDiscoverDeviceAnimView  = (DiscoverDeviceAnimView) findViewById(R.id.DiscoverDeviceAnimView_ToolBarBackground);
        mCollapsingToolbarLayout.setTitle("Scanning...");
        mCollapsingToolbarLayout.setExpandedTitleColor(0xFFFFFFFF);
    }

    private void setupListeners() {
        mOnBridgeConnectionListener = new OnBridgeConnectionListener() {

            @Override
            public void onBridgeConnected() {
                mDeviceBridge.setOnDeviceScanListener(mOnDeviceScanListener);
                Snackbar.make(mRootLayout, "Connected to the CoSmart Bridge", Snackbar.LENGTH_SHORT).show();
            }

            @Override
            public void onBridgeDisconnected() {
                Snackbar.make(mRootLayout, "Disconnected from the CoSmart Bridge", Snackbar.LENGTH_SHORT).show();
            }

        };

        mOnDeviceScanListener = new OnDeviceScanListener() {

            @Override
            public void onScanStart() {
                mDiscoverDeviceAnimView.startDiscover();
            }

            @Override
            public void onDeviceScaned(List<Device> deviceList) {
                mDiscoverDeviceAnimView.toggleDeviceRespone();
            }

            @Override
            public void onScanCompleted() {
                mDiscoverDeviceAnimView.stopDiscover();
            }
        };

        mDeviceBridge.setOnBridgeConnectionListener(mOnBridgeConnectionListener);
        mDeviceBridge.setOnDeviceScanListener(mOnDeviceScanListener);
    }

}
