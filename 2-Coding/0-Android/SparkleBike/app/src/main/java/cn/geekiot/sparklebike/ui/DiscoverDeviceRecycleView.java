package cn.geekiot.sparklebike.ui;

import android.graphics.Canvas;
import android.graphics.Rect;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.StaggeredGridLayoutManager;
import android.util.Log;
import android.view.View;

import com.cobox.cosmart.devicebridge.Device;

import java.util.List;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.adapter.DiscoverDeviceAdapter;

/**
 * Created by Cocoonshu on 2016/5/8.
 */
public class DiscoverDeviceRecycleView {

    private static final String TAG = "RecycleView";

    private final int fItemMarginTop;
    private final int fItemMarginBottom;
    private final int getfItemSpacing;

    private RecyclerView.LayoutManager  mLayoutManager       = null;
    private DiscoverDeviceAdapter       mAdapter             = null;
    private RecyclerView.ItemAnimator   mItemAnimator        = null;
    private RecyclerView.ItemDecoration mItemDecoration      = null;
    private RecyclerView                mHostView            = null;
    private OnItemClickListener         mOnItemClickListener = null;

    public interface OnItemClickListener {
        void onItemClick(Device device, int position, long id);
    }

    public DiscoverDeviceRecycleView(RecyclerView hostView) {
        mHostView          = hostView;
        mAdapter           = new DiscoverDeviceAdapter(hostView);
        mItemAnimator      = new DefaultItemAnimator();
        mItemDecoration    = new MarginItemItemDecoration();
        mLayoutManager     = new LinearLayoutManager(hostView.getContext());
        fItemMarginTop     = hostView.getResources().getDimensionPixelOffset(R.dimen.DiscoverDeviceActivity_ItemMarginTop);
        fItemMarginBottom  = hostView.getResources().getDimensionPixelOffset(R.dimen.DiscoverDeviceActivity_ItemMarginBottom);
        getfItemSpacing    = hostView.getResources().getDimensionPixelOffset(R.dimen.DiscoverDeviceActivity_ItemSpacing);
        mLayoutManager.setAutoMeasureEnabled(true);
        mHostView.setItemAnimator(mItemAnimator);
        mHostView.addItemDecoration(mItemDecoration);
        mHostView.setAdapter(mAdapter);
        mHostView.setLayoutManager(mLayoutManager);
        initializeListeners();
    }

    private void initializeListeners() {
        mAdapter.setOnItemClickListener(new DiscoverDeviceAdapter.OnItemClickListener() {

            @Override
            public void onItemClick(View view, int position, long id) {
                if (mOnItemClickListener != null) {
                    final Device device = mAdapter.getItemContent(position);
                    mOnItemClickListener.onItemClick(device, position, id);
                }
            }

        });
    }

    public void setOnItemClickListener(OnItemClickListener listener) {
        mOnItemClickListener = listener;
    }

    public void notifyDataChanged(List<Device> deviceList) {
        mAdapter.updateFromDeviceList(deviceList);
        mAdapter.notifyDataSetChanged();
    }

    private class MarginItemItemDecoration extends RecyclerView.ItemDecoration {

        @Override
        public void onDrawOver(Canvas c, RecyclerView parent) {
            super.onDrawOver(c, parent);
        }

        @Override
        public void getItemOffsets(Rect outRect, int itemPosition, RecyclerView parent) {
            final int itemCount = mAdapter == null ? 0 : mAdapter.getItemCount();
            if (itemPosition == 0) {
                outRect.set(0, fItemMarginTop, 0, getfItemSpacing >> 1);
            } else if (itemPosition == itemCount - 1) {
                outRect.set(0, getfItemSpacing >> 1, 0, fItemMarginBottom);
            } else {
                outRect.set(0, getfItemSpacing >> 1, 0, getfItemSpacing >> 1);
            }
        }

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
