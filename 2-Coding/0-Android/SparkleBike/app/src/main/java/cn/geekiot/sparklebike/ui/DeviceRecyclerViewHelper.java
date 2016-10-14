package cn.geekiot.sparklebike.ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.view.View;
import android.view.ViewGroup;

import com.cobox.cosmart.devicebridge.Device;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * Device list recycler view helper
 * @Auther Cocoonshu
 * @Date 2016-09-29 14:04:59
 * Copyright (c) 2016 Cocoonshu
 */
public class DeviceRecyclerViewHelper {

    private RecyclerView        mRecyclerView     = null;
    private List<Device>        mDeviceList       = null;
    private DeviceListDiffer    mDeviceListDiffer = null;
    private DeviceAdapter       mAdapter          = null;
    private LinearLayoutManager mLayoutManager    = null;
    private DeviceComparator    mDeviceComparator = null;
    private TextDrawable        mEmptyBackground  = null;

    public DeviceRecyclerViewHelper(RecyclerView recyclerView) {
        mRecyclerView     = recyclerView;
        mDeviceListDiffer = new DeviceListDiffer();
        mEmptyBackground  = new TextDrawable(recyclerView.getContext());
        mLayoutManager    = new LinearLayoutManager(recyclerView.getContext(), LinearLayoutManager.VERTICAL, false);
        mAdapter          = new DeviceAdapter();
        mLayoutManager.setAutoMeasureEnabled(true);
        mRecyclerView.setLayoutManager(mLayoutManager);
    }

    public void setEmptyTextParameters(int textResid, int textSizeResid, int textColorResid) {
        mEmptyBackground.setText(textResid);
        mEmptyBackground.setTextSize(textSizeResid);
        mEmptyBackground.setTextColorResid(textColorResid);
    }

    public void updateDataSet(List<Device> dataset) {
        if (mDeviceList == null) {
            mDeviceList = new ArrayList<Device>();
        }

        mDeviceComparator.setNameOrder(false);
        Collections.sort(dataset, mDeviceComparator);

        synchronized (mDeviceList) {
            mDeviceListDiffer.diff(mDeviceList, dataset);
            int[] adds    = mDeviceListDiffer.getAdds();
            int[] removes = mDeviceListDiffer.getRemoves();
            mDeviceList = dataset;
        }

        mAdapter.setDataSet(mDeviceList);
    }

    private class DeviceListDiffer {

        public boolean diff(List<Device> reference, List<Device> current) {
            int deviceCount = dataset.size();
            for (int i = 0; i < deviceCount; i++) {
                boolean hasFound = false;
                Device  device   = dataset.get(i);
                for (Device exsitDevice : mDeviceList) {
                    if (device.getSSID().equals(exsitDevice.getSSID())
                            && device.getBSSID().equals(exsitDevice.getBSSID())) {
                        hasFound = true;
                        break;
                    }
                }
                if (!hasFound) {

                }
            }
        }

    }

    /**
     * Device comparator
     * @Auther Cocoonshu
     * @Date 2016-09-29 14:04:59
     * Copyright (c) 2016 Cocoonshu
     */
    private class DeviceComparator implements Comparator<Device> {

        private boolean mIsNameDESC = false;

        public void setNameOrder(boolean isDESC) {
            mIsNameDESC = isDESC;
        }

        @Override
        public int compare(Device lhs, Device rhs) {
            if (lhs == rhs) {
                return 0;
            } else if (lhs == null) {
                return -1;
            } else if (rhs == null) {
                return 1;
            } else {
                int titleCompare = mIsNameDESC ? lhs.getSSID().compareTo(rhs.getSSID()) : rhs.getSSID().compareTo(lhs.getSSID());
                int rssiCompare  = lhs.getLevel() - rhs.getLevel();
                return titleCompare == 0 ? rssiCompare : titleCompare;
            }
        }
    }

    /**
     * Device list recycler view adapter
     * @Auther Cocoonshu
     * @Date 2016-09-29 14:04:59
     * Copyright (c) 2016 Cocoonshu
     */
    private class DeviceAdapter extends RecyclerView.Adapter {

        private List<Device> mDataSet = null;

        public void setDataSet(List<Device> dataSet) {
            mDataSet = new ArrayList<>(dataSet);
        }

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            return null;
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {

        }

        @Override
        public int getItemCount() {
            return mDataSet == null ? 0 : mDataSet.size();
        }

    }

    /**
     * Device list recycler view holder
     * @Auther Cocoonshu
     * @Date 2016-09-29 14:04:59
     * Copyright (c) 2016 Cocoonshu
     */
    private class DeviceViewHolder extends RecyclerView.ViewHolder {

        public DeviceViewHolder(View itemView) {
            super(itemView);
        }
    }

    /**
     * Device list recycler view holder
     * @Auther Cocoonshu
     * @Date 2016-09-29 14:04:59
     * Copyright (c) 2016 Cocoonshu
     */
    private static class TextDrawable extends Drawable {

        private Context      mContext = null;
        private String       mText    = null;
        private TextPaint    mPaint   = null;
        private StaticLayout mLayout  = null;
        private Rect         mPadding = null;

        public TextDrawable(Context context) {
            mContext = context;
            mPadding = new Rect();
            mPaint   = new TextPaint(TextPaint.ANTI_ALIAS_FLAG | TextPaint.AUTO_HINTING_TEXT_FLAG);
        }

        @Override
        protected void onBoundsChange(Rect bounds) {
            if (mText != null) {
                updatePadding();
                mLayout = new StaticLayout(
                        mText, mPaint, bounds.width() - mPadding.width(),
                        Layout.Alignment.ALIGN_CENTER, 1, 0, false);
            }
        }

        public void setText(String text) {
            mText = text;
            onBoundsChange(getBounds());
        }

        public void setText(int resid) {
            mText = mContext.getString(resid);
            onBoundsChange(getBounds());
        }

        public void setTextSize(int resid) {
            setTextSize(mContext.getResources().getDimensionPixelSize(resid));
        }

        public void setTextSize(float textSize) {
            mPaint.setTextSize(textSize);
            onBoundsChange(getBounds());
        }

        public void setTextColorResid(int resid) {
            setTextColor(mContext.getResources().getColor(resid));
        }

        public void setTextColor(int color) {
            mPaint.setColor(color);
        }

        @Override
        public void setAlpha(int alpha) {
            mPaint.setAlpha(alpha);
        }

        @Override
        public void setColorFilter(ColorFilter colorFilter) {
            mPaint.setColorFilter(colorFilter);
        }

        @Override
        public int getOpacity() {
            return PixelFormat.TRANSPARENT;
        }

        @Override
        public void draw(Canvas canvas) {
            updatePadding();

            Rect bounds       = getBounds();
            int  roomWidth    = bounds.width() - mPadding.width();
            int  roomHeight   = bounds.height() - mPadding.height();
            int  layoutWidth  = mLayout.getWidth();
            int  layoutHeight = mLayout.getHeight();
            int  offsetX      = (roomWidth - layoutWidth) / 2;
            int  offsetY      = (roomHeight - layoutHeight) / 2;

            canvas.translate(mPadding.left + offsetX, mPadding.top + offsetY);
            mLayout.draw(canvas);
        }

        private void updatePadding() {
            getPadding(mPadding);
        }
    }
}
