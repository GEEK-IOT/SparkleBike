package cn.geekiot.sparklebike.page;

import android.Manifest;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.RecyclerView;
import android.util.Log;

import com.cobox.cosmart.devicebridge.Device;
import com.cobox.cosmart.devicebridge.DeviceBridge;
import com.cobox.cosmart.devicebridge.listeners.OnBridgeConnectionListener;
import com.cobox.cosmart.devicebridge.listeners.OnDeviceScanListener;

import java.util.ArrayList;
import java.util.List;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.adapter.DeviceManager;
import cn.geekiot.sparklebike.ui.DeviceRecyclerViewHelper;
import cn.geekiot.sparklebike.ui.DialogWindow;
import cn.geekiot.sparklebike.ui.WiFiEnableDialog;

/**
 * Scan smart device window
 * @Auther Cocoonshu
 * @Date 2016-10-03 15:56:08
 * Copyright (c) 2016 Cocoonshu
 */
public class ScanningWindow extends DialogWindow implements OnBridgeConnectionListener, OnDeviceScanListener {

    public static final String TAG = "ScanningWindow";

    private static final int KEY_PERMISSIONS_REQUEST_FOR_WIFI_SCAN = 0x0001;

    private static final String[] PERMISSIONS = new String[] {
            Manifest.permission.ACCESS_WIFI_STATE,
            Manifest.permission.CHANGE_WIFI_STATE,
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION
    };

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
        if (!mBridge.isWiFiEnabled()) {
            AlertDialog dialog = WiFiEnableDialog.createDialog(ScanningWindow.this, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    switch (which) {
                        case DialogInterface.BUTTON_POSITIVE:
                            mBridge.setWiFiEnabled(true);
                            if (checkWiFiPermission(ScanningWindow.this, PERMISSIONS)) {
                                mBridge.discoverDevice();
                            }
                            break;
                        case DialogInterface.BUTTON_NEGATIVE:
                            break;
                    }
                }
            });
            dialog.show();
        } else {
            if (checkWiFiPermission(ScanningWindow.this, PERMISSIONS)) {
                mBridge.discoverDevice();
            }
        }
    }

    private boolean checkWiFiPermission(Activity activity, String[] permissions) {
        ArrayList<String> requestList = new ArrayList<String>();
        for (String permission : permissions) {
            if (ContextCompat.checkSelfPermission(activity, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                if (ActivityCompat.shouldShowRequestPermissionRationale(activity, permission)) {
                    // TODO Show a dialog to notify user what permission we need request,
                    //      but now we don't have to do this because system will show a permission
                    //      request dialog to user for us later.
                }
                requestList.add(permission);
            }
        }

        if (!requestList.isEmpty()) {
            String[] requestPermissions = new String[requestList.size()];
            ActivityCompat.requestPermissions(
                    activity,
                    requestPermissions,
                    KEY_PERMISSIONS_REQUEST_FOR_WIFI_SCAN);
            return false;
        } else {
            return true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case KEY_PERMISSIONS_REQUEST_FOR_WIFI_SCAN: {
                if (grantResults.length > 0) {
                    boolean isAllGranted = true;
                    for (int i = 0; i < grantResults.length; i++) {
                        boolean isGranted = grantResults[i] == PackageManager.PERMISSION_GRANTED;
                        isAllGranted &= isGranted;
                        if (isGranted) {
                            Log.i(TAG, "[onRequestPermissionsResult] " + permissions[i] + " granted");
                        } else {
                            Log.e(TAG, "[onRequestPermissionsResult] " + permissions[i] + " rejected");
                        }
                    }
                    if (isAllGranted) {
                        mBridge.discoverDevice();
                        return;
                    }
                }

                // Permissions has been rejected by user, finish
                // TODO
                mBridge.discoverDevice();
            }
        }
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
