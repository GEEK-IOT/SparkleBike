package com.cobox.ui.charts;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;

/**
 * Created by Cocoonshu on 2016-04-07.
 */
public class Meter extends View {

    public Meter(Context context) {
        this(context, null);
    }

    public Meter(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public Meter(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }
}
