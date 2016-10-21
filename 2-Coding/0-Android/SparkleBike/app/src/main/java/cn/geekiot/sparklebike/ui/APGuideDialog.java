package cn.geekiot.sparklebike.ui;

import android.content.Context;
import android.content.DialogInterface;
import android.support.design.widget.TextInputLayout;
import android.support.v7.app.AlertDialog;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.PopupWindow;

import com.cobox.cosmart.devicebridge.Device;
import com.cobox.utils.DeviceComparator;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import cn.geekiot.sparklebike.R;

/**
 * A dialog allow user to guide device
 * to the specified AP
 * @Auther Cocoonshu
 * @Date 2016-10-21 09:48:37
 * Copyright (c) 2016 Cocoonshu
 */
public class APGuideDialog extends AlertDialog {

    private TextInputLayout mSSIDInputLayout     = null;
    private TextInputLayout mPasswordInputLayout = null;
    private EditText        mEdtSSID             = null;
    private EditText        mEdtPassword         = null;
    private CheckBox        mChkRememberAP       = null;
    private PopupWindow     mAutoCompleteWindow  = null;
    private ListView        mLstAutoCompleteList = null;
    private APListAdapter   mAdapter             = null;

    public static AlertDialog createDialog(Context context, DialogInterface.OnClickListener onClickListener) {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setIcon(R.mipmap.ic_wifi_perm)
                .setInverseBackgroundForced(true)
                .setTitle(R.string.APGuideDialog_Title)
                .setView(R.layout.layout_ap_guide_dialog)
                .setPositiveButton(R.string.WiFiEnabledDialog_Confirm, onClickListener)
                .setNegativeButton(R.string.WiFiEnabledDialog_Cancel, onClickListener);
        return builder.create();
    }

    protected APGuideDialog(Context context) {
        super(context);
        setup();
    }

    protected APGuideDialog(Context context, int theme) {
        super(context, theme);
        setup();
    }

    protected APGuideDialog(Context context, boolean cancelable, OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
        setup();
    }

    private void setup() {
        // Fill auto complete window
        mAutoCompleteWindow  = new PopupWindow(getContext());
        mLstAutoCompleteList = new ListView(getContext());
        mAutoCompleteWindow.setContentView(mLstAutoCompleteList);
        mLstAutoCompleteList.setAdapter(mAdapter);

        // Guide dialog layout
        mSSIDInputLayout     = (TextInputLayout) findViewById(R.id.TextInputLayout_SSID);
        mPasswordInputLayout = (TextInputLayout) findViewById(R.id.TextInputLayout_Password);
        mEdtSSID             = (EditText) findViewById(R.id.EditText_SSID);
        mEdtPassword         = (EditText) findViewById(R.id.EditText_Password);
        mChkRememberAP       = (CheckBox) findViewById(R.id.CheckBox_RememberAP);

        mEdtSSID.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        });

        mEdtSSID.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (hasFocus) {
                    mAdapter.filter(mEdtSSID.getText().toString());
                    mAutoCompleteWindow.showAsDropDown(v, 0, 0, Gravity.BOTTOM);
                } else {
                    mAutoCompleteWindow.dismiss();
                }
            }
        });

        mEdtPassword.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        });
    }

    public void setAPList(List<Device> apList) {
        mAdapter.setAPList(apList);
    }

    private class APListAdapter extends BaseAdapter {

        private Context          mContext        = null;
        private String           mFilter         = null;
        private List<Device>     mAPList         = null;
        private List<Device>     mFilteredAPList = null;
        private DeviceComparator mComparator     = null;

        public APListAdapter(Context context) {
            mContext        = context;
            mFilteredAPList = new ArrayList<Device>();
            mComparator     = new DeviceComparator();
            mComparator.setNameOrder(true);
        }

        public void filter(String filter) {
            mFilter = filter;
            updateDisplayData();
        }

        private void updateDisplayData() {
            synchronized (mFilteredAPList) {
                mFilteredAPList.clear();
                synchronized (mAPList) {
                    for (Device device : mAPList) {
                        if (device.getSSID().contains(mFilter)) {
                            mFilteredAPList.add(device);
                        }
                    }
                }
            }
            this.notifyDataSetChanged();
        }

        public void setAPList(List<Device> apList) {
            if (apList == null) {
                mAPList = null;
            } else {
                mAPList = new ArrayList<>(apList);
            }
            Collections.sort(mAPList, mComparator);
            updateDisplayData();
        }

        @Override
        public int getCount() {
            synchronized (mFilter) {
                if (mFilter != null && !mFilter.isEmpty()) {
                    return mFilteredAPList == null ? 0 : mFilteredAPList.size();
                }
            }

            synchronized (mAPList) {
                return mAPList == null ? 0 : mAPList.size();
            }
        }

        @Override
        public Object getItem(int position) {
            return null;
        }

        @Override
        public long getItemId(int position) {
            return 0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            return null;
        }
    }
}
