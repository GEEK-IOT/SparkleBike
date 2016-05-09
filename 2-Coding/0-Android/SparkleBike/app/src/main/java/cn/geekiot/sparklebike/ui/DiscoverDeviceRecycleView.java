package cn.geekiot.sparklebike.ui;

import android.os.Handler;
import android.support.v7.widget.RecyclerView;
import android.util.Log;

import com.cobox.cosmart.devicebridge.Device;

import java.util.List;

import cn.geekiot.sparklebike.adapter.DiscoverDeviceAdapter;

/**
 * Created by Cocoonshu on 2016/5/8.
 */
public class DiscoverDeviceRecycleView {

    private static final String TAG = "RecycleView";

    private DiscoverDeviceAdapter mAdapter  = null;
    private RecyclerView          mHostView = null;
    private Handler               mHandler  = new Handler();

    public DiscoverDeviceRecycleView(RecyclerView hostView) {
        mHostView = hostView;
        mAdapter  = new DiscoverDeviceAdapter();
    }

    public void notifyDataChanged(List<Device> deviceList) {
        mAdapter.updateFromDeviceList(deviceList);

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                mAdapter.notifyDataSetChanged();
            }
        });
        Log.w(TAG, "[notifyDataChanged] device list = " + deviceList.size() + ", adapter size = " + mAdapter.getItemCount());
    }
}
