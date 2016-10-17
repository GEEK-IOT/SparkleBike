package com.cobox.utils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;

/**
 * Device list differ
 * @Auther Cocoonshu
 * @Date 2016-10-17 13:59:31
 * Copyright (c) 2016 Cocoonshu
 */
public abstract class Differ<T> implements Comparator<T> {

    private int[]         mAddRecords    = null;
    private int[]         mRemoveRecords = null;
    private Comparator<T> mComparator    = this;

    public void setComparator(Comparator<T> comparator) {
        mComparator = comparator;
    }

    @Override
    public int compare(T lhs, T rhs) {
        return lhs.equals(rhs) ? 0 : 1;
    }

    /**
     * Diff the current list with reference list
     * @param reference
     * @param current
     * @return true if reference is different from current
     */
    public boolean diff(List<T> reference, List<T> current) {
        int           referenceSize     = reference.size();
        int           currentSize       = current.size();
        List<Wrapper> diffReferenceList = new ArrayList<Wrapper>(referenceSize);
        List<Wrapper> diffCurrentList   = new ArrayList<Wrapper>(currentSize);
        for (int i = 0; i < referenceSize; i++) {
            diffReferenceList.add(new Wrapper(reference.get(i), i));
        }
        for (int i = 0; i < currentSize; i++) {
            diffCurrentList.add(new Wrapper(current.get(i), i));
        }
        Collections.sort(reference, mComparator);
        Collections.sort(current, mComparator);
        mAddRecords    = null;
        mRemoveRecords = null;

        // Remove the same elements
        Iterator<Wrapper> currentItr   = diffCurrentList.iterator();
        Iterator<Wrapper> referenceItr = diffReferenceList.iterator();
        while (currentItr.hasNext()) {
            T element = currentItr.next().element;
            while (referenceItr.hasNext()) {
                if (isSame(element, referenceItr.next().element)) {
                    referenceItr.remove();
                    currentItr.remove();
                    break;
                }
            }
        }

        // Fill result
        int counter = 0;
        mRemoveRecords = new int[diffReferenceList.size()];
        for (Wrapper wrapper : diffReferenceList) {
            mRemoveRecords[counter] = wrapper.index;
            counter++;
        }
        diffReferenceList.clear();

        counter = 0;
        mAddRecords = new int[diffCurrentList.size()];
        for (Wrapper wrapper : diffCurrentList) {
            mAddRecords[counter] = wrapper.index;
            counter++;
        }
        diffCurrentList.clear();

        return (mRemoveRecords != null && mRemoveRecords.length > 0)
                || (mAddRecords != null && mAddRecords.length > 0);
    }

    protected abstract boolean isSame(T current, T reference);

    public int[] getAdds() {
        return mAddRecords;
    }

    public int[] getRemoves() {
        return mRemoveRecords;
    }

    private Wrapper wrap(T element, int index) {
        return new Wrapper(element, index);
    }

    private class Wrapper {
        int index;
        T   element;

        public Wrapper(T element, int index) {
            this.element = element;
            this.index   = index;
        }
    }

}
