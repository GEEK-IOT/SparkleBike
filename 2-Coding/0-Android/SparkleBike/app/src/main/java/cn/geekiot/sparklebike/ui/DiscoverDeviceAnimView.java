package cn.geekiot.sparklebike.ui;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.RippleDrawable;
import android.util.AttributeSet;
import android.view.View;

import cn.geekiot.sparklebike.R;

/**
 * Created by Cocoonshu on 2016/4/17.
 */
public class DiscoverDeviceAnimView extends View {

    private RippleDrawable mDblRippler          = null;
    private Drawable       mDblMasterDevice     = null;
    private Drawable       mDblSlaverDevice     = null;
    private PointF         mMasterPosition      = new PointF();
    private PointF         mSlaverPosition      = new PointF();
    private float          mMasterSweepDistance = 0;
    private float          mSlaverSweepDistance = 0;

    public DiscoverDeviceAnimView(Context context) {
        this(context, null);
    }

    public DiscoverDeviceAnimView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public DiscoverDeviceAnimView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr, 0);
        initialize(context, attrs, defStyleAttr);
    }

    private void initialize(Context context, AttributeSet attrs,  int defStyleAttr) {
        TypedArray typedArray = context.obtainStyledAttributes(attrs, R.styleable.DiscoverDeviceAnimView, defStyleAttr, 0);
        int indexCount = typedArray.getIndexCount();
        for (int i = 0; i < indexCount; i++) {
            int key = typedArray.getIndex(i);
            switch (key) {
                case R.styleable.DiscoverDeviceAnimView_master_device_drawable:
                    mDblMasterDevice = typedArray.getDrawable(i);
                    break;
                case R.styleable.DiscoverDeviceAnimView_slaver_device_drawable:
                    mDblSlaverDevice = typedArray.getDrawable(i);
                    break;
                default:
                    break;
            }
        }
        typedArray.recycle();
        mDblRippler = new RippleDrawable(ColorStateList.valueOf(0x99000000), null, null);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        int backgroundWidth  = 0;
        int backgroundHeight = 0;
        int widthSpecMode    = MeasureSpec.getMode(widthMeasureSpec);
        int heightSpecMode   = MeasureSpec.getMode(heightMeasureSpec);
        int widthSpecSize    = MeasureSpec.getSize(widthMeasureSpec);
        int heightSpecSize   = MeasureSpec.getSize(heightMeasureSpec);
        int measuredWidth    = 0;
        int measuredHeight   = 0;

        final Drawable background = getBackground();
        if (background != null) {
            backgroundWidth = background.getIntrinsicWidth();
            backgroundHeight = background.getIntrinsicHeight();
        }

        // Width
        switch (widthSpecMode) {
            case MeasureSpec.UNSPECIFIED:
                measuredWidth = backgroundWidth;
                break;
            case MeasureSpec.AT_MOST:
                measuredWidth = backgroundWidth < widthSpecSize ? backgroundWidth : widthSpecSize;
                break;
            case MeasureSpec.EXACTLY:
                measuredWidth = widthSpecSize;
                break;
        }

        // Height
        switch (heightSpecMode) {
            case MeasureSpec.UNSPECIFIED:
                measuredHeight = backgroundHeight;
                break;
            case MeasureSpec.AT_MOST:
                measuredHeight = backgroundHeight < heightSpecSize ? backgroundHeight : heightSpecSize;
                break;
            case MeasureSpec.EXACTLY:
                measuredHeight = heightSpecSize;
                break;
        }

        setMeasuredDimension(measuredWidth, measuredHeight);
        refreshMasterAndSlaverPosition();
        mMasterSweepDistance = computeLongthSweepPath(mMasterPosition.x, mMasterPosition.y);
        mSlaverSweepDistance = computeLongthSweepPath(mSlaverPosition.x, mSlaverPosition.y);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        mDblRippler.setRadius(50);
        canvas.translate(mMasterPosition.x, mMasterPosition.y);
        mDblRippler.draw(canvas);
    }

    private float computeLongthSweepPath(float currentX, float currentY) {
        float viewWidth    = getWidth();
        float viewHeight   = getHeight();
        float minXBorder   = getPaddingLeft();
        float maxXBorder   = viewWidth - getPaddingRight();
        float minYBorder   = getPaddingTop();
        float maxYBorder   = viewHeight - getPaddingBottom();
        float centerX      = (maxXBorder - minXBorder) * 0.5f + minXBorder;
        float centerY      = (maxYBorder - minYBorder) * 0.5f + minYBorder;
        float destinationX = (currentX < centerX) ? minXBorder : maxXBorder;
        float destinationY = (currentY < centerY) ? minYBorder : maxYBorder;
        return (float)Math.hypot(Math.abs(destinationX - currentX), Math.abs(destinationY - currentY));
    }

    private void refreshMasterAndSlaverPosition() {
        float viewWidth    = getWidth();
        float viewHeight   = getHeight();
        float minXBorder   = getPaddingLeft();
        float maxXBorder   = viewWidth - getPaddingRight();
        float minYBorder   = getPaddingTop();
        float maxYBorder   = viewHeight - getPaddingBottom();
        float centerY      = (maxYBorder - minYBorder) * 0.5f + minYBorder;
        float masterWidth  = mDblMasterDevice != null ? mDblMasterDevice.getIntrinsicWidth() : 0;
        float slaverWidth  = mDblSlaverDevice != null ? mDblSlaverDevice.getIntrinsicWidth() : 0;
        mMasterPosition.x = minXBorder + masterWidth * 0.5f;
        mSlaverPosition.x = maxXBorder - slaverWidth * 0.5f;
        mMasterPosition.y = mSlaverPosition.y = centerY;

        if (mDblMasterDevice != null) {
            mDblMasterDevice.setBounds(0, 0, (int)viewWidth, (int)viewHeight);
        }
        if (mDblSlaverDevice != null) {
            mDblSlaverDevice.setBounds(0, 0, (int)viewWidth, (int)viewHeight);
        }
        mDblRippler.setBounds(0, 0, (int)viewWidth, (int)viewHeight);
    }
}
