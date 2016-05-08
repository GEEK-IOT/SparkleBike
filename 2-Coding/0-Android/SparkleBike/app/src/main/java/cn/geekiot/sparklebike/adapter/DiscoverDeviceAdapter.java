package cn.geekiot.sparklebike.adapter;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.Icon;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.cobox.cosmart.devicebridge.Device;

import java.util.ArrayList;
import java.util.List;
import java.util.zip.Inflater;

import cn.geekiot.sparklebike.R;

/**
 * DiscoverDeviceAdapter
 * @Auther Cocoonshu
 * @Date 2016-04-15 18:04
 * Copyright (c) 2016 Cocoonshu
 */
public class DiscoverDeviceAdapter extends RecyclerView.Adapter {

    private List<Device> mDeviceList = null;

    public class DeviceViewHolder extends RecyclerView.ViewHolder {
        private Context              mContext         = null;
        private TextView             mTxvTitle        = null;
        private TextView             mTxvType         = null;
        private TextView             mTxvMAC          = null;
        private TextView             mBtnIgnore       = null;
        private ImageView            mImgIcon         = null;
        private View.OnClickListener mOnClickListener = null;
        private Drawable             mDefaultIcon     = null;

        public DeviceViewHolder(View parentView, int viewType) {
            super(LayoutInflater.from(parentView.getContext()).inflate(R.layout.item_discovered_device, null));
            initializeListeners();
            mContext     = parentView.getContext();
            mTxvTitle    = (TextView) itemView.findViewById(R.id.TextView_Title);
            mTxvType     = (TextView)itemView.findViewById(R.id.TextView_Type);
            mTxvMAC      = (TextView)itemView.findViewById(R.id.TextView_MAC);
            mImgIcon     = (ImageView)itemView.findViewById(R.id.ImageView_Icon);
            mBtnIgnore   = (TextView)itemView.findViewById(R.id.TextView_Ignore);
            mDefaultIcon = mContext.getDrawable(R.mipmap.ic_developer_board_black);
            mBtnIgnore.setOnClickListener(mOnClickListener);

        }

        private void initializeListeners() {
            mOnClickListener = new View.OnClickListener() {

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
                mTxvMAC.setText("");
                mImgIcon.setImageDrawable(mDefaultIcon);
            } else {
                mTxvTitle.setText(device.getSSID());
                mTxvType.setText("Terminal");
                mTxvMAC.setText(device.getMAC() == null ? "" : device.getMACString());
                mImgIcon.setImageDrawable(mDefaultIcon);
            }
        }
    }

    public void updateFromDeviceList(List<Device> deviceList) {
        synchronized (mDeviceList) {
            mDeviceList.clear();
            mDeviceList = new ArrayList<Device>(deviceList);
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
        synchronized (mDeviceList) {
            return mDeviceList == null ? 0 : mDeviceList.size();
        }
    }

}
