package cn.geekiot.sparklebike.ui;

import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.StaggeredGridLayoutManager;
import android.util.Log;

import com.cobox.cosmart.devicebridge.Device;

import java.util.List;

import cn.geekiot.sparklebike.adapter.DiscoverDeviceAdapter;

/**
 * Created by Cocoonshu on 2016/5/8.
 */
public class DiscoverDeviceRecycleView {

    private static final String TAG = "RecycleView";

    private RecyclerView.LayoutManager  mLayoutManager  = null;
    private DiscoverDeviceAdapter       mAdapter        = null;
    private RecyclerView.ItemAnimator   mItemAnimator   = null;
    private RecyclerView.ItemDecoration mItemDecoration = null;
    private RecyclerView                mHostView       = null;

    public DiscoverDeviceRecycleView(RecyclerView hostView) {
        mHostView      = hostView;
        mAdapter       = new DiscoverDeviceAdapter();
        mItemAnimator  = new DefaultItemAnimator();
        mItemDecoration = null;
        mLayoutManager = new LinearLayoutManager(hostView.getContext());
        mLayoutManager.setAutoMeasureEnabled(true);
        mHostView.setItemAnimator(mItemAnimator);
        mHostView.setAdapter(mAdapter);
        mHostView.setLayoutManager(mLayoutManager);
    }

    public void notifyDataChanged(List<Device> deviceList) {
        mAdapter.updateFromDeviceList(deviceList);
        mAdapter.notifyDataSetChanged();
    }

    private class FadeItemAnimator extends RecyclerView.ItemAnimator {

        @Override
        public boolean animateAppearance(@NonNull RecyclerView.ViewHolder viewHolder, @Nullable ItemHolderInfo preLayoutInfo, @NonNull ItemHolderInfo postLayoutInfo) {
            return false;
        }

        @Override
        public boolean animateDisappearance(@NonNull RecyclerView.ViewHolder viewHolder, @NonNull ItemHolderInfo preLayoutInfo, @Nullable ItemHolderInfo postLayoutInfo) {
            return false;
        }

        @Override
        public boolean animatePersistence(@NonNull RecyclerView.ViewHolder viewHolder, @NonNull ItemHolderInfo preLayoutInfo, @NonNull ItemHolderInfo postLayoutInfo) {
            return false;
        }

        @Override
        public boolean animateChange(@NonNull RecyclerView.ViewHolder oldHolder, @NonNull RecyclerView.ViewHolder newHolder, @NonNull ItemHolderInfo preLayoutInfo, @NonNull ItemHolderInfo postLayoutInfo) {
            return false;
        }

        @Override
        public void runPendingAnimations() {

        }

        @Override
        public void endAnimation(RecyclerView.ViewHolder item) {

        }

        @Override
        public void endAnimations() {

        }

        @Override
        public boolean isRunning() {
            return false;
        }
    }
}
