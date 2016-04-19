package cn.geekiot.sparklebike.graphic;

import android.graphics.Canvas;
import android.graphics.ColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.PointF;
import android.graphics.RadialGradient;
import android.graphics.Shader;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.ShapeDrawable;

/**
 * WaveDrawable
 * @Auther Cocoonshu
 * @Date 2016-04-18 13:30
 * Copyright (c) 2016 Cocoonshu
 */
public class WaveDrawable extends Drawable {

    private static final float   FloatAlphaUnit     = 1.0f / 255.0f;
    private              float   mDrawableAlpha     = 1.0f;
    private              Paint   mWavePaint         = new Paint();
    private              Shader  mWaveShader        = null;
    private              Matrix  mWaveShaderMatrix  = new Matrix();
    private              boolean mIsWaveShaderDirty = true;
    private              float   mRadius            = 0;
    private              PointF  mPivotPoint        = new PointF();
    private              int     mDarkColor         = 0xFF000000;
    private              int     mLightColor        = 0x00000000;
    private              float   mWaveThickness     = 0.5f;

    public WaveDrawable() {
        mWavePaint.setAntiAlias(true);
        mWavePaint.setStyle(Paint.Style.FILL);
    }

    @Override
    public void draw(Canvas canvas) {
        rebuildRadialGradientIfNeeded();

        canvas.save();
        canvas.clipRect(getBounds());

        mWaveShaderMatrix.reset();
        mWaveShaderMatrix.setScale(mRadius, mRadius);
        mWaveShaderMatrix.postTranslate(mPivotPoint.x, mPivotPoint.y);
        mWaveShader.setLocalMatrix(mWaveShaderMatrix);
        mWavePaint.setShader(mWaveShader);
        canvas.drawCircle(mPivotPoint.x, mPivotPoint.y, mRadius, mWavePaint);

        canvas.restore();
    }

    public PointF getPivotPoint() {
        return mPivotPoint;
    }

    public void setPivotPoint(float x, float y) {
        mPivotPoint.set(x, y);
    }

    public void setRadius(float radius) {
        mRadius = radius;
    }

    public float getRadius() {
        return mRadius;
    }

    public void setThicknessPercent(float thickness) {
        mWaveThickness = thickness;
        mIsWaveShaderDirty = true;
    }

    public float getThicknessPercent() {
        return mWaveThickness;
    }

    @Override
    public void setAlpha(int alpha) {
        mDrawableAlpha = alpha * FloatAlphaUnit;
    }

    @Override
    public void setColorFilter(ColorFilter colorFilter) {
        mWavePaint.setColorFilter(colorFilter);
    }

    @Override
    public int getOpacity() {
        if (mDrawableAlpha <= 0.0f) {
            return PixelFormat.TRANSPARENT;
        } else if (mDrawableAlpha >= 1.0f) {
            return PixelFormat.OPAQUE;
        } else {
            return PixelFormat.TRANSLUCENT;
        }
    }

    private void rebuildRadialGradientIfNeeded() {
        if (mIsWaveShaderDirty) {
            mWaveShader = new RadialGradient(
                    0, 0, 1,                            // centerX, centerY, radius
                    new int[]{mLightColor, mDarkColor}, // color group
                    new float[]{mWaveThickness, 1.0f},  // color stops
                    Shader.TileMode.CLAMP);             // edge draw method
            mIsWaveShaderDirty = false;
        }
    }
}
