package cn.geekiot.sparklebike.page;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.ui.TestRecycleViewHelper;

/**
 * AbsFragment
 * @Auther 80074591
 * @Date 2016-06-12 16:50
 * Copyright (c) 2016 80074591
 */
public class AbsFragment extends Fragment {

    private static final String TAG = "AbsFragment";

    protected View                  mRootView          = null;
    protected FragmentControlLinker mControlLinker     = null;
    protected TestRecycleViewHelper mRecycleViewHelper = null;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mRootView = inflater.inflate(R.layout.main_activity_default_fragment_layout, container, false);
        mRecycleViewHelper = new TestRecycleViewHelper((RecyclerView) mRootView.findViewById(R.id.RecyclerView_ContentList));
        return mRootView;
    }

    public void setControlLinker(FragmentControlLinker linker) {
        mControlLinker = linker;
    }

    protected FragmentControlLinker getControlLinker() {
        return mControlLinker;
    }

    protected View getRootView() {
        return mRootView;
    }

}
