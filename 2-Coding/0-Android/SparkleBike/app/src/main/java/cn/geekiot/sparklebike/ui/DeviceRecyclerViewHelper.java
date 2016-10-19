package cn.geekiot.sparklebike.ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.media.Image;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.cobox.cosmart.devicebridge.Device;
import com.cobox.utils.Differ;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

import cn.geekiot.sparklebike.R;

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
        mDeviceComparator = new DeviceComparator();
        mEmptyBackground  = new TextDrawable(recyclerView.getContext());
        mLayoutManager    = new LinearLayoutManager(recyclerView.getContext(), LinearLayoutManager.VERTICAL, false);
        mAdapter          = new DeviceAdapter(recyclerView.getContext());
        mLayoutManager.setAutoMeasureEnabled(true);
        mRecyclerView.setLayoutManager(mLayoutManager);
        mRecyclerView.setAdapter(mAdapter);
    }

    public void setEmptyTextParameters(int textResid, int textSizeResid, int textColorResid) {
        mEmptyBackground.setText(textResid);
        mEmptyBackground.setTextSizeResid(textSizeResid);
        mEmptyBackground.setTextColorResid(textColorResid);
    }

    public void updateDataSet(List<Device> dataset) {
        if (mDeviceList == null) {
            mDeviceList = new ArrayList<Device>();
        }

        int[] adds    = null;
        int[] removes = null;
        mDeviceComparator.setNameOrder(true);
        mDeviceListDiffer.setComparator(mDeviceComparator);
        synchronized (mDeviceList) {
            if (mDeviceListDiffer.diff(mDeviceList, dataset)) {
                adds    = mDeviceListDiffer.getAdds();
                removes = mDeviceListDiffer.getRemoves();
            }
            mDeviceList = dataset;
        }

        mAdapter.setDataSet(mDeviceList);
        if (removes != null) {
            for (int remove : removes) {
                mAdapter.notifyItemRemoved(remove);
            }
        }
        if (adds != null) {
            for (int add : adds) {
                mAdapter.notifyItemInserted(add);
            }
        }
    }

    /**
     * Device list differ
     * @Auther Cocoonshu
     * @Date 2016-09-29 14:04:59
     * Copyright (c) 2016 Cocoonshu
     */
    private class DeviceListDiffer extends Differ<Device> {

        @Override
        protected boolean isSame(Device current, Device reference) {
            return current.getSSID().equals(reference.getSSID())
                    && current.getBSSID().equals(reference.getBSSID());
        }

    }

    /**
     * Device comparator
     * @Auther Cocoonshu
     * @Date 2016-09-29 14:04:59
     * Copyright (c) 2016 Cocoonshu
     */
    private class DeviceComparator implements Comparator<Device> {

        private boolean mIsNameASC = false;

        public void setNameOrder(boolean isASC) {
            mIsNameASC = isASC;
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
                int titleCompare = mIsNameASC ? lhs.getSSID().compareTo(rhs.getSSID()) : rhs.getSSID().compareTo(lhs.getSSID());
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

        private Context      mContext          = null;
        private List<Device> mDataSet          = null;
        private Drawable     mDblDefaultDevice = null;
        private Drawable     mDblStateInfo     = null;
        private Drawable     mDblStateQuestion = null;
        private Drawable     mDblStateExchange = null;

        public DeviceAdapter(Context context) {
            mContext          = context;
            mDblDefaultDevice = context.getDrawable(R.drawable.ic_default_device);
            mDblStateInfo     = context.getDrawable(R.drawable.ic_device_info);
            mDblStateQuestion = context.getDrawable(R.drawable.ic_device_question);
            mDblStateExchange = context.getDrawable(R.drawable.ic_device_exchange);
        }

        public void setDataSet(List<Device> dataSet) {
            mDataSet = new ArrayList<>(dataSet);
        }

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            View itemView = LayoutInflater.from(mContext).inflate(R.layout.layout_scanning_device_item, null);
            DeviceViewHolder holder = new DeviceViewHolder(itemView);
            return holder;
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
            String title    = null;
            String subTitle = null;
            synchronized (mDataSet) {
                Device device = mDataSet.get(position);
                title    = device.getSSID();
                subTitle = device.getMACString();
            }

            DeviceViewHolder viewHolder = (DeviceViewHolder) holder;
            if (title.length() > 12) {
                viewHolder.setTitle(title.substring(0, title.length() - 12));
            } else {
                viewHolder.setTitle(title);
            }
            viewHolder.setSubtitle(subTitle);
            viewHolder.setIcon(mDblDefaultDevice);
            viewHolder.setSubicon(mDblStateInfo);
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

        private ImageView   mImgIcon     = null;
        private TextView    mTxvTitle    = null;
        private TextView    mTxvSubTitle = null;
        private ImageButton mBtnOption   = null;

        public DeviceViewHolder(View itemView) {
            super(itemView);
            mImgIcon     = (ImageView) itemView.findViewById(R.id.ImageView_Icon);
            mTxvTitle    = (TextView) itemView.findViewById(R.id.TextView_Title);
            mTxvSubTitle = (TextView) itemView.findViewById(R.id.TextView_Subtitle);
            mBtnOption   = (ImageButton) itemView.findViewById(R.id.ImageButton_Option);
        }

        public void setIcon(Drawable icon) {
            mImgIcon.setImageDrawable(icon);
        }

        public void setTitle(String title) {
            mTxvTitle.setText(title);
        }

        public void setSubtitle(String subtitle) {
            mTxvSubTitle.setText(subtitle);
        }

        public void setSubicon(Drawable subicon) {
            mBtnOption.setImageDrawable(subicon);
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

        public void setTextSizeResid(int resid) {
            mPaint.setTextSize(mContext.getResources().getDimensionPixelSize(resid));
            onBoundsChange(getBounds());
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
