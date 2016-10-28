package com.cobox.utils;

import com.cobox.cosmart.devicebridge.Device;

import java.util.Comparator;

/**
 * Device comparator
 * @Auther Cocoonshu
 * @Date 2016-09-29 14:04:59
 * Copyright (c) 2016 Cocoonshu
 */
public class DeviceComparator implements Comparator<Device> {

    private boolean mIsNameASC = false;

    public void setNameOrder(boolean isASC) {
        mIsNameASC = isASC;
    }

    @Override
    public int compare(Device lhs, Device rhs) {
        if (lhs == rhs) {
            return 0;
        } else if (lhs == null) {
            return -1;
        } else if (rhs == null) {
            return 1;
        } else {
            int titleCompare = mIsNameASC ? lhs.getSSID().compareTo(rhs.getSSID()) : rhs.getSSID().compareTo(lhs.getSSID());
            int rssiCompare  = lhs.getLevel() - rhs.getLevel();
            return titleCompare == 0 ? rssiCompare : titleCompare;
        }
    }
}