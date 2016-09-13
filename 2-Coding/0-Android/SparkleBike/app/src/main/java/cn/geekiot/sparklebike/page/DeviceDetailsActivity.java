package cn.geekiot.sparklebike.page;

import android.content.Intent;
import android.content.res.ColorStateList;
import android.os.Bundle;
import android.support.design.widget.CollapsingToolbarLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.ui.DiscoverDeviceRecycleView;

public class DeviceDetailsActivity extends AppCompatActivity {

    private static final String TAG = "DeviceDetailsActivity";
    public  static final String KEY_DEVICE_ICON_COLOR = "KEY_DEVICE_ICON_COLOR";
    public  static final String KEY_DEVICE_SSID       = "KEY_DEVICE_SSID";
    public  static final String KEY_DEVICE_BSSID      = "KEY_DEVICE_BSSID";

    private ViewGroup                  mRootLayout                 = null;
    private Toolbar                    mToolBar                    = null;
    private CollapsingToolbarLayout    mCollapsingToolbarLayout    = null;
    private DiscoverDeviceRecycleView  mFoundDeviceListView        = null;
    private FloatingActionButton       mFloatingActionButton       = null;
    private DeviceDecoration           mDeviceDecoration           = null;

    private class DeviceDecoration {
        public int    iconTintColor = 0x00000000;
        public String deviceSSID    = null;
        public String deviceBSSID   = null;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_device_details);
        setupActionBarAsToolBar();
        findContentViews();
        setupListeners();
    }

    @Override
    protected void onStart() {
        super.onStart();
        initializeFromIntent();
    }

    private void setupWindowStyle() {
        Window window = getWindow();
        window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.setStatusBarColor(getResources().getColor(R.color.primary_dark));
    }

    private void setupActionBarAsToolBar() {
        mToolBar = (Toolbar) findViewById(R.id.ToolBar);
        setSupportActionBar(mToolBar);
        getSupportActionBar().setHomeButtonEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        getWindow().setStatusBarColor(getResources().getColor(R.color.primary));
    }

    private void findContentViews() {
        mRootLayout              = (FrameLayout) findViewById(R.id.FrameLayout_RootLayout);
        mFoundDeviceListView     = new DiscoverDeviceRecycleView((RecyclerView) findViewById(R.id.RecyclerView_FoundDeviceList));
        mCollapsingToolbarLayout = (CollapsingToolbarLayout) findViewById(R.id.CollapsingToolbarLayout);
        mFloatingActionButton    = (FloatingActionButton) findViewById(R.id.FloatingActionButton);
        mCollapsingToolbarLayout.setTitle("Scanning...");
        mCollapsingToolbarLayout.setExpandedTitleColor(0xFFFFFFFF);
    }

    private void setupListeners() {

    }

    private void initializeFromIntent() {
        Intent intent = getIntent();
        if (intent != null) {
            mDeviceDecoration               = new DeviceDecoration();
            mDeviceDecoration.iconTintColor = intent.getIntExtra(KEY_DEVICE_ICON_COLOR, mDeviceDecoration.iconTintColor);
            mDeviceDecoration.deviceSSID    = intent.getStringExtra(KEY_DEVICE_SSID);
            mDeviceDecoration.deviceBSSID   = intent.getStringExtra(KEY_DEVICE_BSSID);

            Log.e(TAG, String.format("[initializeFromIntent] iconTintColor = #%X, SSID = %s, BSSID = %s",
                    mDeviceDecoration.iconTintColor, mDeviceDecoration.deviceSSID, mDeviceDecoration.deviceBSSID));
            mFloatingActionButton.setBackgroundTintList(ColorStateList.valueOf(mDeviceDecoration.iconTintColor));
            mCollapsingToolbarLayout.setTitle(mDeviceDecoration.deviceSSID);
        }
    }
}
