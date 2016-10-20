package cn.geekiot.sparklebike.ui;

import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;

import cn.geekiot.sparklebike.R;

/**
 * Created by Cocoonshu on 2016/5/7.
 */
public class APGuideDialog {

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
}
