package cn.geekiot.sparklebike.view;

import android.support.v7.widget.RecyclerView;

import com.cobox.cosmart.devicebridge.Device;

import java.util.List;

import cn.geekiot.sparklebike.adapter.DiscoverDeviceAdapter;

/**
 * Created by Cocoonshu on 2016/5/8.
 */
public class DiscoverDeviceRecycleView {
    private DiscoverDeviceAdapter mAdapter = null;
    private RecyclerView          mHostView = null;

    public DiscoverDeviceRecycleView(RecyclerView hostView) {
        mHostView = hostView;
        mAdapter  = new DiscoverDeviceAdapter();
    }

    public void notifyDataChanged(List<Device> deviceList) {
        mAdapter.updateFromDeviceList(deviceList);
        mAdapter.notifyDataSetChanged();
    }
}
