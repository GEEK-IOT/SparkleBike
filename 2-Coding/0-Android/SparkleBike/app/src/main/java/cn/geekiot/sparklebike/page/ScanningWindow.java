package cn.geekiot.sparklebike.page;

import android.os.Bundle;
import android.support.annotation.Nullable;

import cn.geekiot.sparklebike.ColorActivity;
import cn.geekiot.sparklebike.R;

/**
 * Scan smart device window
 * @Auther Cocoonshu
 * @Date 2016-10-03 15:56:08
 * Copyright (c) 2016 Cocoonshu
 */
public class ScanningWindow extends ColorActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setupWindowStyle();
        setContentView(R.layout.activity_scaning);
        setupActionBar();
        findViews();
    }

    private void setupWindowStyle() {
    }

    private void setupActionBar() {
    }

    private void findViews() {
    }
}
