package cn.geekiot.sparklebike.graphic;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.util.Log;

import java.io.InputStream;

/**
 * BlurDrawable
 * @Auther Cocoonshu
 * @Date 2016-05-10 17:36
 * Copyright (c) 2016 Cocoonshu
 */
public class BlurDrawable extends BitmapDrawable {

    private static final float  DIMEN_SCALE = 0.125f;
    private static final int    MIN_DIMEN   = 1;
    private static final int    BLUR_RADIUS = 2;

    public BlurDrawable() {
        super();
    }

    public BlurDrawable(Resources res) {
        super(res);
    }

    public BlurDrawable(Bitmap bitmap) {
        super(createBlurBitmap(bitmap, BLUR_RADIUS));
    }

    public BlurDrawable(Resources res, Bitmap bitmap) {
        super(res, createBlurBitmap(bitmap, BLUR_RADIUS));
    }

    public BlurDrawable(String filepath) {
        super(createBlurBitmap(filepath, BLUR_RADIUS));
    }

    public BlurDrawable(Resources res, String filepath) {
        super(res, createBlurBitmap(filepath, BLUR_RADIUS));
    }

    public BlurDrawable(InputStream is) {
        super(createBlurBitmap(is, BLUR_RADIUS));
    }

    public BlurDrawable(Resources res, InputStream is) {
        super(res, createBlurBitmap(is, BLUR_RADIUS));
    }

    public BlurDrawable(Bitmap bitmap, int blurRadius) {
        super(createBlurBitmap(bitmap, BLUR_RADIUS));
    }

    private static Bitmap createBlurBitmap(Bitmap bitmap, int blurRadius) {
        Bitmap srcBitmap = Bitmap.createScaledBitmap(
                bitmap,
                clampAndScaleDimension(bitmap.getWidth(), DIMEN_SCALE, MIN_DIMEN),
                clampAndScaleDimension(bitmap.getHeight(), DIMEN_SCALE, MIN_DIMEN),
                true);
        return FastBlur.doBlur(srcBitmap, blurRadius, true);
    }

    private static Bitmap createBlurBitmap(String filepath, int blurRadius) {
        Bitmap srcBitmap = BitmapFactory.decodeFile(filepath);
        srcBitmap = Bitmap.createScaledBitmap(
                srcBitmap,
                clampAndScaleDimension(srcBitmap.getWidth(), DIMEN_SCALE, MIN_DIMEN),
                clampAndScaleDimension(srcBitmap.getHeight(), DIMEN_SCALE, MIN_DIMEN),
                true);
        return FastBlur.doBlur(srcBitmap, blurRadius, true);
    }

    private static Bitmap createBlurBitmap(InputStream is, int blurRadius) {
        Bitmap srcBitmap = BitmapFactory.decodeStream(is);
        srcBitmap = Bitmap.createScaledBitmap(
                srcBitmap,
                clampAndScaleDimension(srcBitmap.getWidth(), DIMEN_SCALE, MIN_DIMEN),
                clampAndScaleDimension(srcBitmap.getHeight(), DIMEN_SCALE, MIN_DIMEN),
                true);
        return FastBlur.doBlur(srcBitmap, blurRadius, true);
    }

    private static int clampAndScaleDimension(int src, float scale, int ref) {
        int dest = (int) (src * scale);
        return dest < ref ? ref : dest;
    }

    public static class FastBlur {

        private static final String TAG = "FastBlur";

        public static Bitmap doBlur(Bitmap sentBitmap, int radius, boolean canReuseInBitmap) {

            // Stack Blur v1.0 from
            // http://www.quasimondo.com/StackBlurForCanvas/StackBlurDemo.html
            //
            // Java Author: Mario Klingemann
            // http://incubator.quasimondo.com
            // created Feburary 29, 2004
            // Android port : Yahel Bouaziz
            // http://www.kayenko.com
            // ported april 5th, 2012

            // This is a compromise between Gaussian Blur and Box blur
            // It creates much better looking blurs than Box Blur, but is
            // 7x faster than my Gaussian Blur implementation.
            //
            // I called it Stack Blur because this dejava.lang.Stringscribes best how this
            // filter works internally: it creates a kind of moving stack
            // of colors whilst scanning through the image. Thereby it
            // just has to add one new block of color to the right side
            // of the stack and remove the leftmost color. The remaining
            // colors on the topmost layer of the stack are either added on
            // or reduced by one, depending on if they are on the right or
            // on the left side of the stack.
            //
            // If you are using this algorithm in your code please add
            // the following line:
            //
            // Stack Blur Algorithm by Mario Klingemann

            Bitmap bitmap = null;

            if (canReuseInBitmap) {
                bitmap = sentBitmap;
            } else {
                bitmap = sentBitmap.copy(sentBitmap.getConfig(), true);
            }

            if (radius < 1) {
                return bitmap;
            }

            int   width  = bitmap.getWidth();
            int   height = bitmap.getHeight();
            int[] pixels = new int[width * height];
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height);

            int wm = width - 1;
            int hm = height - 1;
            int wh = width * height;
            int div = radius + radius + 1;

            int r[] = new int[wh];
            int g[] = new int[wh];
            int b[] = new int[wh];
            int rSum, gSum, bSum, x, y, i, p, yp, yi, yw;
            int VMin[] = new int[Math.max(width, height)];

            int divSum = (div + 1) >> 1;
            divSum *= divSum;
            int dv[] = new int[256 * divSum];
            for (i = 0; i < 256 * divSum; i++) {
                dv[i] = (i / divSum);
            }

            yw = yi = 0;

            int[][] stack = new int[div][3];
            int stackPointer;
            int stackStart;
            int[] sir;
            int rbs;
            int r1 = radius + 1;
            int rOutSum, gOutSum, bOutSum;
            int rInSum, gInSum, bInSum;

            for (y = 0; y < height; y++) {
                rInSum = gInSum = bInSum = rOutSum = gOutSum = bOutSum = rSum = gSum = bSum = 0;
                for (i = -radius; i <= radius; i++) {
                    p = pixels[yi + Math.min(wm, Math.max(i, 0))];
                    sir = stack[i + radius];
                    sir[0] = (p & 0xff0000) >> 16;
                    sir[1] = (p & 0x00ff00) >> 8;
                    sir[2] = (p & 0x0000ff);
                    rbs = r1 - Math.abs(i);
                    rSum += sir[0] * rbs;
                    gSum += sir[1] * rbs;
                    bSum += sir[2] * rbs;
                    if (i > 0) {
                        rInSum += sir[0];
                        gInSum += sir[1];
                        bInSum += sir[2];
                    } else {
                        rOutSum += sir[0];
                        gOutSum += sir[1];
                        bOutSum += sir[2];
                    }
                }
                stackPointer = radius;

                for (x = 0; x < width; x++) {

                    r[yi] = dv[rSum];
                    g[yi] = dv[gSum];
                    b[yi] = dv[bSum];

                    rSum -= rOutSum;
                    gSum -= gOutSum;
                    bSum -= bOutSum;

                    stackStart = stackPointer - radius + div;
                    sir = stack[stackStart % div];

                    rOutSum -= sir[0];
                    gOutSum -= sir[1];
                    bOutSum -= sir[2];

                    if (y == 0) {
                        VMin[x] = Math.min(x + radius + 1, wm);
                    }
                    p = pixels[yw + VMin[x]];

                    sir[0] = (p & 0xff0000) >> 16;
                    sir[1] = (p & 0x00ff00) >> 8;
                    sir[2] = (p & 0x0000ff);

                    rInSum += sir[0];
                    gInSum += sir[1];
                    bInSum += sir[2];

                    rSum += rInSum;
                    gSum += gInSum;
                    bSum += bInSum;

                    stackPointer = (stackPointer + 1) % div;
                    sir = stack[(stackPointer) % div];

                    rOutSum += sir[0];
                    gOutSum += sir[1];
                    bOutSum += sir[2];

                    rInSum -= sir[0];
                    gInSum -= sir[1];
                    bInSum -= sir[2];

                    yi++;
                }
                yw += width;
            }
            for (x = 0; x < width; x++) {
                rInSum = gInSum = bInSum = rOutSum = gOutSum = bOutSum = rSum = gSum = bSum = 0;
                yp = -radius * width;
                for (i = -radius; i <= radius; i++) {
                    yi = Math.max(0, yp) + x;

                    sir = stack[i + radius];

                    sir[0] = r[yi];
                    sir[1] = g[yi];
                    sir[2] = b[yi];

                    rbs = r1 - Math.abs(i);

                    rSum += r[yi] * rbs;
                    gSum += g[yi] * rbs;
                    bSum += b[yi] * rbs;

                    if (i > 0) {
                        rInSum += sir[0];
                        gInSum += sir[1];
                        bInSum += sir[2];
                    } else {
                        rOutSum += sir[0];
                        gOutSum += sir[1];
                        bOutSum += sir[2];
                    }

                    if (i < hm) {
                        yp += width;
                    }
                }
                yi = x;
                stackPointer = radius;
                for (y = 0; y < height; y++) {
                    // Preserve alpha channel: ( 0xff000000 & pix[yi] )
                    pixels[yi] = (0xff000000 & pixels[yi]) | (dv[rSum] << 16) | (dv[gSum] << 8) | dv[bSum];

                    rSum -= rOutSum;
                    gSum -= gOutSum;
                    bSum -= bOutSum;

                    stackStart = stackPointer - radius + div;
                    sir = stack[stackStart % div];

                    rOutSum -= sir[0];
                    gOutSum -= sir[1];
                    bOutSum -= sir[2];

                    if (x == 0) {
                        VMin[y] = Math.min(y + r1, hm) * width;
                    }
                    p = x + VMin[y];

                    sir[0] = r[p];
                    sir[1] = g[p];
                    sir[2] = b[p];

                    rInSum += sir[0];
                    gInSum += sir[1];
                    bInSum += sir[2];

                    rSum += rInSum;
                    gSum += gInSum;
                    bSum += bInSum;

                    stackPointer = (stackPointer + 1) % div;
                    sir = stack[stackPointer];

                    rOutSum += sir[0];
                    gOutSum += sir[1];
                    bOutSum += sir[2];

                    rInSum -= sir[0];
                    gInSum -= sir[1];
                    bInSum -= sir[2];

                    yi += width;
                }
            }

            bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
            return bitmap;
        }
    }
}
