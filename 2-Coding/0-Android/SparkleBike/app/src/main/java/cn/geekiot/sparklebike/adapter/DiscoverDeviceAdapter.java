package cn.geekiot.sparklebike.adapter;

import android.app.AlarmManager;
import android.content.Context;
import android.graphics.drawable.Drawable;
import android.support.v7.widget.CardView;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.cobox.cosmart.devicebridge.Device;

import java.util.ArrayList;
import java.util.List;

import cn.geekiot.sparklebike.R;

/**
 * DiscoverDeviceAdapter
 * @Auther Cocoonshu
 * @Date 2016-04-15 18:04
 * Copyright (c) 2016 Cocoonshu
 */
public class DiscoverDeviceAdapter extends RecyclerView.Adapter {

    private static final String TAG = "DiscoverDeviceAdapter";

    private List<Device> mDeviceList = null;

    public class DeviceViewHolder extends RecyclerView.ViewHolder {
        private Context              mContext                = null;
        private CardView             mRootCardView           = null;
        private ImageView            mImgIcon                = null;
        private TextView             mTxvTitle               = null;
        private TextView             mTxvType                = null;
        private TextView             mTxvMAC                 = null;
        private ImageButton          mBtnMore                = null;
        private View.OnTouchListener mOnItemTouchListener    = null;
        private View.OnClickListener mOnMoreBtnClickListener = null;
        private Drawable             mDefaultIcon            = null;

        public DeviceViewHolder(ViewGroup parentView, int viewType) {
            super(LayoutInflater.from(parentView.getContext()).inflate(R.layout.item_discovered_device, parentView, false));
            initializeListeners();
            mContext      = parentView.getContext();
            mRootCardView = (CardView) itemView.findViewById(R.id.CardView_ItemRootLayout);
            mTxvTitle     = (TextView) itemView.findViewById(R.id.TextView_Title);
            mTxvType      = (TextView)itemView.findViewById(R.id.TextView_Type);
            mTxvMAC       = (TextView)itemView.findViewById(R.id.TextView_MAC);
            mImgIcon      = (ImageView)itemView.findViewById(R.id.ImageView_Icon);
            mBtnMore      = (ImageButton)itemView.findViewById(R.id.ImageButton_More);
            mDefaultIcon  = mContext.getDrawable(R.mipmap.ic_rounter);
            mBtnMore.setOnClickListener(mOnMoreBtnClickListener);
            parentView.setOnTouchListener(mOnItemTouchListener);
        }

        private void initializeListeners() {
            mOnItemTouchListener = new View.OnTouchListener() {

                @Override
                public boolean onTouch(View view, MotionEvent event) {
                    mRootCardView.dispatchTouchEvent(event);
                    return false;
                }

            };

            mOnMoreBtnClickListener = new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    // TODO Ignore this item and remember this action
                }

            };
        }

        public void updateContent(Device device) {
            if (device == null) {
                mTxvTitle.setText("-");
                mTxvType.setText("-");
                mTxvCapabilities.setText("-");
                mTxvLevelAndFreq.setText("- / -");
                mTxvMAC.setText("");
                mImgIcon.setImageDrawable(mDefaultIcon);
            } else {
                mTxvTitle.setText(device.getSSID());
                mTxvType.setText("Terminal");
                mTxvCapabilities.setText(device.getCapabilities());
                mTxvLevelAndFreq.setText(device.getLevel() + " dBm / " + device.getFrequency() + " MHz");
                mTxvMAC.setText(device.getMAC() == null ? "" : device.getMACString());
                mImgIcon.setImageDrawable(mDefaultIcon);
            }
        }
    }

    public void updateFromDeviceList(List<Device> deviceList) {
        if (mDeviceList == null) {
            mDeviceList = new ArrayList<Device>(deviceList);
        } else {
            synchronized (mDeviceList) {
                mDeviceList.clear();
                mDeviceList = new ArrayList<Device>(deviceList);
            }
        }
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new DeviceViewHolder(parent, viewType);
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        ((DeviceViewHolder)holder).updateContent(mDeviceList.get(position));
    }

    @Override
    public int getItemCount() {
        if (mDeviceList == null) {
            return 0;
        } else {
            synchronized (mDeviceList) {
                return mDeviceList == null ? 0 : mDeviceList.size();
            }
        }
    }

}
