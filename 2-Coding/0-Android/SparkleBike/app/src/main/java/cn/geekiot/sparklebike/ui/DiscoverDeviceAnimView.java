package cn.geekiot.sparklebike.ui;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.wifi.WifiManager;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.graphic.BlurDrawable;
import cn.geekiot.sparklebike.graphic.WaveDrawable;

/**
 * Created by Cocoonshu on 2016/4/17.
 */
public class DiscoverDeviceAnimView extends View {

    private static final String TAG          = "DiscoverDeviceAnimView";

    private WaveDrawable   mDblWave               = null;
    private Drawable       mDblMasterDevice       = null;
    private Drawable       mDblSlaveDevice        = null;
    private int            mWaveColor             = 0xFF000000;
    private int            mBackgroundBlurRadius  = 5;
    private PointF         mMasterPosition        = new PointF();
    private PointF         mSlavePosition         = new PointF();
    private float          mMasterSweepDistance   = 0;
    private float          mSlaveSweepDistance    = 0;

    private long           mMasterAnimStartTime   = 0;
    private long           mSlaveAnimStartTime    = 0;
    private boolean        mIsDiscoverAnimationOn = false;
    private float          mWaveSpeed             = 1f;   // pixel/ms
    private long           mMasterWaveInterval    = 1000; // ms
    private boolean        mIsSlaveResponeActived = false;

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
                    if (mDblMasterDevice != null) {
                        mDblMasterDevice.setBounds(0, 0, mDblMasterDevice.getIntrinsicWidth(), mDblMasterDevice.getIntrinsicHeight());
                    }
                    break;
                case R.styleable.DiscoverDeviceAnimView_slaver_device_drawable:
                    mDblSlaveDevice = typedArray.getDrawable(i);
                    if (mDblSlaveDevice != null) {
                        mDblSlaveDevice.setBounds(0, 0, mDblSlaveDevice.getIntrinsicWidth(), mDblSlaveDevice.getIntrinsicHeight());
                    }
                    break;
                case R.styleable.DiscoverDeviceAnimView_wave_color:
                    mWaveColor = typedArray.getColor(key, mWaveColor);
                    break;
                case R.styleable.DiscoverDeviceAnimView_background_blur_radius:
                    mBackgroundBlurRadius = typedArray.getInt(key, mBackgroundBlurRadius);
                    break;
                default:
                    break;
            }
        }
        typedArray.recycle();
        mDblWave = new WaveDrawable();
        mDblWave.setWaveColor(mWaveColor);
        replaceBackgroundDrawable();
    }

    private void replaceBackgroundDrawable() {
        BlurDrawable   blurDrawable       = null;
        BitmapDrawable backgroundDrawable = null;

        if (getBackground() != null && getBackground() instanceof BitmapDrawable) {
            backgroundDrawable = (BitmapDrawable) getBackground();
            blurDrawable       = new BlurDrawable(backgroundDrawable.getBitmap(), mBackgroundBlurRadius);
            setBackground(blurDrawable);
        }
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
        refreshMasterWaveTimeLine();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        boolean hasMoreFrame = false;
        hasMoreFrame |= drawDiscoverWave(canvas);
        hasMoreFrame |= drawDeviceResponseWave(canvas);
        hasMoreFrame |= drawDevices(canvas);
        if (hasMoreFrame) {
            postInvalidateOnAnimation();
        }
    }

    private boolean drawDiscoverWave(Canvas canvas) {
        if (mIsDiscoverAnimationOn) {
            if (mMasterAnimStartTime == 0) {
                mMasterAnimStartTime = AnimationUtils.currentAnimationTimeMillis();
            }

            long  currentAnimTime           = AnimationUtils.currentAnimationTimeMillis();
            long  deltaAnimTime             = currentAnimTime - mMasterAnimStartTime;
            float currentMasterWaveDistance = mWaveSpeed * deltaAnimTime;
            float masterWaveIterator        = currentMasterWaveDistance;
            float tempMaxDistance           = (float) Math.hypot(getWidth(), getHeight());
            
            tempMaxDistance += tempMaxDistance * mDblWave.getThicknessPercent();
            while (masterWaveIterator > tempMaxDistance) {
                masterWaveIterator -= mWaveSpeed * mMasterWaveInterval;
            }
            while (masterWaveIterator > 0) {
                mDblWave.setPivotPoint(mMasterPosition.x, mMasterPosition.y);
                mDblWave.setRadius(masterWaveIterator);
                mDblWave.draw(canvas);

                masterWaveIterator -= mWaveSpeed * mMasterWaveInterval;
            }
        }

        return true;
    }

    private boolean drawDeviceResponseWave(Canvas canvas) {
        if (mIsDiscoverAnimationOn && mIsSlaveResponeActived) {
            long  currentAnimTime           = AnimationUtils.currentAnimationTimeMillis();
            long  deltaAnimTime             = currentAnimTime - mSlaveAnimStartTime;
            float currentSlaveWaveDistance  = mWaveSpeed * deltaAnimTime;
            float tempMaxDistance           = (float) Math.hypot(getWidth(), getHeight());

            tempMaxDistance += tempMaxDistance * mDblWave.getThicknessPercent();
            if (currentSlaveWaveDistance > tempMaxDistance) {
                mIsSlaveResponeActived = false;
                return false;
            } else {
                mDblWave.setPivotPoint(mSlavePosition.x, mSlavePosition.y);
                mDblWave.setRadius(currentSlaveWaveDistance);
                mDblWave.draw(canvas);
            }
        }

        return true;
    }

    private boolean drawDevices(Canvas canvas) {
        if (mDblMasterDevice != null) {
            float drawableWidth  = mDblMasterDevice.getIntrinsicWidth();
            float drawableHeight = mDblMasterDevice.getIntrinsicHeight();
            canvas.save();
            canvas.translate(mMasterPosition.x - drawableWidth * 0.5f, mMasterPosition.y - drawableHeight * 0.5f);
            mDblMasterDevice.draw(canvas);
            canvas.restore();
        }
        if (mDblSlaveDevice != null) {
            float drawableWidth  = mDblSlaveDevice.getIntrinsicWidth();
            float drawableHeight = mDblSlaveDevice.getIntrinsicHeight();
            canvas.save();
            canvas.translate(mSlavePosition.x - drawableWidth * 0.5f, mSlavePosition.y - drawableHeight * 0.5f);
            mDblSlaveDevice.draw(canvas);
            canvas.restore();
        }
        return false;
    }

    private float computeLongthSweepPath(float currentX, float currentY, float thicknessPercent) {
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
        return (float)Math.hypot(Math.abs(destinationX - currentX), Math.abs(destinationY - currentY))
                * (1.0f + thicknessPercent);
    }

    private int computeMasterSweepCount() {
        float wholeDistanceDurationMS = mMasterSweepDistance / mWaveSpeed;
        float sweepCount              = wholeDistanceDurationMS / mMasterWaveInterval;
        return (int) Math.ceil(sweepCount);
    }

    private void refreshMasterWaveTimeLine() {
        mMasterSweepDistance = computeLongthSweepPath(mMasterPosition.x, mMasterPosition.y, mDblWave.getThicknessPercent());
        mSlaveSweepDistance  = computeLongthSweepPath(mSlavePosition.x, mSlavePosition.y, mDblWave.getThicknessPercent());
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
        float slaverWidth  = mDblSlaveDevice != null ? mDblSlaveDevice.getIntrinsicWidth() : 0;
        mMasterPosition.x = minXBorder + masterWidth * 0.5f;
        mSlavePosition.x  = maxXBorder - slaverWidth * 0.5f;
        mMasterPosition.y = mSlavePosition.y = centerY;
        mDblWave.setBounds(0, 0, (int)viewWidth, (int)viewHeight);
    }
    
    public void startDiscover() {
        mIsDiscoverAnimationOn = true;
        postInvalidateOnAnimation();
    }
    
    public void stopDiscover() {
        mIsDiscoverAnimationOn = false;
    }
    
    public void toggleDeviceResponse() {
        if (!mIsSlaveResponeActived) {
            mIsSlaveResponeActived = true;
            mSlaveAnimStartTime = AnimationUtils.currentAnimationTimeMillis();
            postInvalidateOnAnimation();
        }
    }
}
