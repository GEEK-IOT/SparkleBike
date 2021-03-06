package cn.geekiot.sparklebike.adapter;

import android.app.AlarmManager;
import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.support.v7.widget.CardView;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.cobox.cosmart.devicebridge.Config;
import com.cobox.cosmart.devicebridge.Device;

import java.util.ArrayList;
import java.util.List;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.page.DeviceDetailsActivity;

/**
 * DiscoverDeviceAdapter
 * @Auther Cocoonshu
 * @Date 2016-04-15 18:04
 * Copyright (c) 2016 Cocoonshu
 */
public class DiscoverDeviceAdapter extends RecyclerView.Adapter {

    private static final String TAG = "DiscoverDeviceAdapter";

    private static String[]       sDeviceTypeNameList         = {/* Load during construction */};
    private static int[]          sDeviceIconHintColorList    = {/* Load during construction */};
    private static String         sDefaultDeviceType          = null;
    private static ColorStateList sDefaultDeviceIconTintColor = null;

    private List<Device>        mDeviceList          = null;
    private RecyclerView        mHostView            = null;
    private OnItemClickListener mOnItemClickListener = null;

    public interface OnItemClickListener {
        void onItemClick(View view, int position, long id);
    }

    public class DeviceViewHolder extends RecyclerView.ViewHolder {
        private Context              mContext                = null;
        private CardView             mRootCardView           = null;
        private ImageView            mImgIcon                = null;
        private TextView             mTxvTitle               = null;
        private TextView             mTxvType                = null;
        private TextView             mTxvMAC                 = null;
        private TextView             mTxvCapabilities        = null;
        private TextView             mTxvLevelAndFreq        = null;
        private ImageButton          mBtnMore                = null;
        private Drawable             mDefaultIcon            = null;

        public DeviceViewHolder(ViewGroup parentView, int viewType) {
            super(LayoutInflater.from(parentView.getContext()).inflate(R.layout.item_discovered_device, parentView, false));

            mContext         = parentView.getContext();
            mRootCardView    = (CardView) itemView.findViewById(R.id.CardView_ItemRootLayout);
            mTxvTitle        = (TextView) itemView.findViewById(R.id.TextView_Title);
            mTxvType         = (TextView) itemView.findViewById(R.id.TextView_Type);
            mTxvMAC          = (TextView) itemView.findViewById(R.id.TextView_MAC);
            mTxvCapabilities = (TextView) itemView.findViewById(R.id.TextView_Capabilities);
            mTxvLevelAndFreq = (TextView) itemView.findViewById(R.id.TextView_LevelAndFrequency);
            mImgIcon         = (ImageView) itemView.findViewById(R.id.ImageView_Icon);
            mBtnMore         = (ImageButton) itemView.findViewById(R.id.ImageButton_More);
            mDefaultIcon     = mContext.getResources().getDrawable(R.mipmap.ic_rounter);
            initializeListeners();

            this.itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.e("Cocoonshu", "[onClick] ItemView clicked");
                }
            });
        }

        private void initializeListeners() {
            mBtnMore.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    Log.e("Cocoonshu", "[onClick] ButtonMore");
                }

            });

            mRootCardView.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                Log.e("Cocoonshu", "[onClick] Item");
                    if (mOnItemClickListener != null) {
                        mOnItemClickListener.onItemClick(
                                DeviceViewHolder.this.itemView,
                                DeviceViewHolder.this.getAdapterPosition(),
                                DeviceViewHolder.this.getItemId());
                    }
                }

            });
        }

        public void updateContent(Device device) {
            if (device == null) {
                mTxvTitle.setText("-");
                mTxvType.setText("-");
                mTxvCapabilities.setText("-");
                mTxvLevelAndFreq.setText("- / -");
                mTxvMAC.setText(sDefaultDeviceType);
                mImgIcon.setImageDrawable(mDefaultIcon);
                mImgIcon.setBackgroundTintList(sDefaultDeviceIconTintColor);
                mImgIcon.setTag(sDefaultDeviceIconTintColor);
                mImgIcon.setTag(sDeviceIconHintColorList[0]);
            } else {
                mTxvTitle.setText(device.getSSID());
                mTxvCapabilities.setText(device.getCapabilities());
                mTxvLevelAndFreq.setText(device.getLevel() + " dBm / " + device.getFrequency() + " MHz");
                mTxvMAC.setText(device.getMAC() == null ? "" : device.getMACString());

                if (device.getSSID().startsWith(Config.COSMART_DEVICE_SSID_PREFIX)) {
                    mTxvType.setText(sDeviceTypeNameList[1]);
                    mImgIcon.setImageDrawable(mDefaultIcon);
                    mImgIcon.setBackgroundTintList(ColorStateList.valueOf(sDeviceIconHintColorList[1]));
                    mImgIcon.setTag(sDeviceIconHintColorList[1]);
                } else {
                    mTxvType.setText(sDefaultDeviceType);
                    mImgIcon.setImageDrawable(mDefaultIcon);
                    mImgIcon.setBackgroundTintList(sDefaultDeviceIconTintColor);
                    mImgIcon.setTag(sDeviceIconHintColorList[0]);
                }
            }
        }
    }

    public DiscoverDeviceAdapter(RecyclerView hostView) {
        mHostView = hostView;
        loadDeviceResourceList(hostView.getContext());
    }

    private void loadDeviceResourceList(Context context) {
        Resources resources = context.getResources();
        if (resources != null) {
            sDeviceTypeNameList         = resources.getStringArray(R.array.DeviceTypeName);
            sDeviceIconHintColorList    = resources.getIntArray(R.array.IconTintColor);
            sDefaultDeviceType          = sDeviceTypeNameList.length > 0 ? sDeviceTypeNameList[0] : null;
            sDefaultDeviceIconTintColor = sDeviceIconHintColorList.length > 0 ? ColorStateList.valueOf(sDeviceIconHintColorList[0]) : null;
        }
    }

    public void setOnItemClickListener(OnItemClickListener listener) {
        mOnItemClickListener = listener;
    }

    public Device getItemContent(int position) {
        return mDeviceList.get(position);
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
