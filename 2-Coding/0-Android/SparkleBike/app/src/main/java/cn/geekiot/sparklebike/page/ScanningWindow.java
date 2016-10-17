package cn.geekiot.sparklebike.page;

import android.support.v7.widget.RecyclerView;
import android.util.Log;

import com.cobox.cosmart.devicebridge.Device;
import com.cobox.cosmart.devicebridge.DeviceBridge;
import com.cobox.cosmart.devicebridge.listeners.OnBridgeConnectionListener;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;

import java.util.List;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.adapter.DeviceManager;
import cn.geekiot.sparklebike.ui.DeviceRecyclerViewHelper;
import cn.geekiot.sparklebike.ui.DialogWindow;

/**
 * Scan smart device window
 * @Auther Cocoonshu
 * @Date 2016-10-03 15:56:08
 * Copyright (c) 2016 Cocoonshu
 */
public class ScanningWindow extends DialogWindow implements OnBridgeConnectionListener, OnDeviceScanListener {

    public static final String TAG = "ScanningWindow";

    private DeviceRecyclerViewHelper mDeviceListViewHelper = null;
    private RecyclerView             mDeviceList           = null;
    private DeviceBridge             mBridge               = DeviceManager.getDeviceBridge();

    @Override
    protected int onMenuInflate() {
        return R.menu.menu_scanning_window;
    }

    @Override
    protected int getColorMenuItemId() {
        return R.id.action_close;
    }

    @Override
    protected int getWindowInTransitionResource() {
        return R.transition.scanning_window_fade_in_content;
    }

    @Override
    protected int getWindowOutTransitionResource() {
        return R.transition.scanning_window_fade_out_content;
    }

    @Override
    protected int getWindowInTransitionLayout() {
        return R.layout.activity_scanning_content;
    }

    @Override
    protected int getWindowOutTransitionLayout() {
        return R.layout.activity_scanning_placeholder;
    }

    @Override
    protected int getWindowStyleEnterTransition() {
        return R.transition.scanning_window_show_anim;
    }

    @Override
    protected int getWindowStyleExitTransition() {
        return R.transition.scanning_window_hide_anim;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////

    @Override
    protected void onContentViewInflated() {
        mDeviceList = (RecyclerView) findViewById(R.id.RecyclerView_DeviceList);
        mDeviceListViewHelper = new DeviceRecyclerViewHelper(mDeviceList);
        mDeviceListViewHelper.setEmptyTextParameters(
                R.string.ScanningWindow_NoDeviceFound,
                R.dimen.ScanningWindow_EmptyText_TextSize,
                R.color.ScanningWindow_EmptyText_TextColor);
    }

    @Override
    protected void onStart() {
        super.onStart();
        connectDeviceBridgeService();
    }

    @Override
    protected void onStop() {
        super.onStop();
        disconnectDeviceBridgeService();
    }

    @Override
    public void onBridgeConnected() {
        mBridge.discoverDevice();
    }

    @Override
    public void onBridgeDisconnected() {

    }

    @Override
    public void onScanStart() {

    }

    @Override
    public void onDeviceScaned(List<Device> deviceList) {
        for (Device device : deviceList) {
            Log.i(TAG, "[onDeviceScaned] device = " + device);
        }
        mDeviceListViewHelper.updateDataSet(deviceList);
    }

    @Override
    public void onScanCompleted() {

    }

    private void connectDeviceBridgeService() {
        mBridge.setOnBridgeConnectionListener(this);
        mBridge.setOnDeviceScanListener(this);
        mBridge.connectService(ScanningWindow.this);
    }

    private void disconnectDeviceBridgeService() {
        mBridge.disconnectService(ScanningWindow.this);
    }

}
