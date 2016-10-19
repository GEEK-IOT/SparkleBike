package cn.geekiot.sparklebike.ui;

import android.app.Dialog;
import android.content.DialogInterface;
import android.graphics.drawable.ColorDrawable;
import android.support.v7.app.AlertDialog;
import android.content.Context;

import cn.geekiot.sparklebike.R;

/**
 * Created by Cocoonshu on 2016/5/7.
 */
public class WiFiEnableDialog {

    public static AlertDialog createDialog(Context context, DialogInterface.OnClickListener onClickListener) {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setIcon(R.mipmap.ic_wifi_perm)
               .setInverseBackgroundForced(true)
               .setTitle(R.string.WiFiEnabledDialog_Title)
               .setMessage(R.string.WiFiEnabledDialog_Message)
               .setPositiveButton(R.string.WiFiEnabledDialog_Confirm, onClickListener)
               .setNegativeButton(R.string.WiFiEnabledDialog_Cancel, onClickListener);
        return builder.create();
    }
}
