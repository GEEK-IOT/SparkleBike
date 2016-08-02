package cn.geekiot.sparklebike.page;

import android.app.ActivityOptions;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.transition.Transition;
import android.transition.TransitionInflater;
import android.util.Pair;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;

import com.cobox.cosmart.devicebridge.Device;
import com.cobox.cosmart.devicebridge.DeviceBridge;
import com.cobox.cosmart.devicebridge.listeners.OnBridgeConnectionListener;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;
import com.cobox.utils.SDK;

import java.util.List;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.ui.DiscoverDeviceAnimView;
import cn.geekiot.sparklebike.ui.WiFiEnableDialog;
import cn.geekiot.sparklebike.ui.DiscoverDeviceRecycleView;
import cn.geekiot.sparklebike.ui.DiscoverDeviceRecycleView.OnItemClickListener;

public class DiscoverDeviceActivity extends AppCompatActivity {

    private ViewGroup                  mRootLayout                 = null;
    private Toolbar                    mToolBar                    = null;
    private CollapsingToolbarLayout    mCollapsingToolbarLayout    = null;
    private DiscoverDeviceRecycleView  mFoundDeviceListView        = null;
    private FloatingActionButton       mFloatingActionButton       = null;
    private DiscoverDeviceAnimView     mDiscoverDeviceAnimView     = null;
    private DeviceBridge               mDeviceBridge               = new DeviceBridge();
    private OnDeviceScanListener       mOnDeviceScanListener       = null;
    private OnBridgeConnectionListener mOnBridgeConnectionListener = null;
    private OnItemClickListener        mOnItemClickListener        = null;

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

        window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.setStatusBarColor(getResources().getColor(R.color.primary_dark));
    }

    private void setupActionBarAsToolBar() {
        mToolBar = (Toolbar) findViewById(R.id.ToolBar);
        setSupportActionBar(mToolBar);
        getSupportActionBar().setHomeButtonEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }

    private void findContentViews() {
        mRootLayout              = (FrameLayout) findViewById(R.id.FrameLayout_RootLayout);
        mFoundDeviceListView     = new DiscoverDeviceRecycleView((RecyclerView) findViewById(R.id.RecyclerView_FoundDeviceList));
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
                startDiscover();
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
                mFoundDeviceListView.notifyDataChanged(deviceList);
                mDiscoverDeviceAnimView.toggleDeviceResponse();
            }

            @Override
            public void onScanCompleted() {
                mDiscoverDeviceAnimView.stopDiscover();
            }
        };

        mOnItemClickListener = new OnItemClickListener() {

            @Override
            public void onItemClick(View itemView, Device device, int position, long id) {
                Intent intent        = new Intent(DiscoverDeviceActivity.this, DeviceDetailsActivity.class);
                View   itemIcon      = itemView.findViewById(R.id.ImageView_Icon);
                int    itemIconColor = (Integer) itemIcon.getTag();
                intent.putExtra(DeviceDetailsActivity.KEY_DEVICE_ICON_COLOR, itemIconColor);
                intent.putExtra(DeviceDetailsActivity.KEY_DEVICE_SSID, device.getSSID());
                intent.putExtra(DeviceDetailsActivity.KEY_DEVICE_BSSID, device.getBSSID());

                if (SDK.isSupportedMaterialDesign()) {
                    Pair<View, String> sharedViewIcon = new Pair<View, String>(
                            itemIcon, getString(R.string.SharedElementName_FloatingActionButton));
                    Pair<View, String> sharedViewTitle = new Pair<View, String>(
                            itemView, getString(R.string.SharedElementName_DeviceItem));

                    startActivity(intent,
                        ActivityOptions.makeSceneTransitionAnimation(
                            DiscoverDeviceActivity.this, sharedViewIcon).toBundle());
                } else {
                    startActivity(intent);
                }
            }

        };

        mFoundDeviceListView.setOnItemClickListener(mOnItemClickListener);
        mDeviceBridge.setOnBridgeConnectionListener(mOnBridgeConnectionListener);
        mDeviceBridge.setOnDeviceScanListener(mOnDeviceScanListener);
    }

    private void startDiscover() {
        if (mDeviceBridge != null) {
            if (!mDeviceBridge.isWiFiEnabled()) {
                AlertDialog dialog = WiFiEnableDialog.createDialog(DiscoverDeviceActivity.this, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                    switch (which) {
                        case DialogInterface.BUTTON_POSITIVE:
                            mDeviceBridge.setWiFiEnabled(true);
                            mDeviceBridge.discoverDevice();
                            break;
                        case DialogInterface.BUTTON_NEGATIVE:
                            break;
                    }
                    }
                });
                dialog.show();
            } else {
                mDeviceBridge.discoverDevice();
            }
        }
    }

}
