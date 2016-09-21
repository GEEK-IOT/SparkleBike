package cn.geekiot.sparklebike.page;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.widget.SwipeRefreshLayout;
import android.support.v7.widget.CardView;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.Layout;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import cn.geekiot.sparklebike.R;
import cn.geekiot.sparklebike.ui.TestRecycleViewHelper;

/**
 * AbsFragment
 * @Auther Cocoonshu
 * @Date 2016-06-12 16:50
 * Copyright (c) 2016 Cocoonshu
 */
public class AbsFragment extends Fragment {

    private static final String TAG = "AbsFragment";
    public static final int SPAN_COUNT = 2;

    protected View                      mRootView        = null;
    protected FragmentControlLinker     mControlLinker   = null;
    private   SwipeRefreshLayout        mRefreshLayout   = null;
    private   RecyclerView              mRecyclerView    = null;
    private   GridLayoutManager         mLayoutManager   = null;
    private   SimpleRecyclerViewAdapter mRecyclerAdapter = null;
    private   Handler                   mUiHandler       = null;

    public AbsFragment() {
        mUiHandler = new Handler(Looper.getMainLooper());
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        mRootView        = inflater.inflate(R.layout.main_activity_default_fragment_layout, container, false);
        mRefreshLayout   = (SwipeRefreshLayout) mRootView.findViewById(R.id.SwipeRefreshLayout);
        mRecyclerView    = (RecyclerView) mRootView.findViewById(R.id.RecyclerView_Content);
        mRecyclerAdapter = SimpleRecyclerViewAdapter.createFakeDataSetAdapter(getContext());
        mLayoutManager   = new GridLayoutManager(getActivity(), SPAN_COUNT, GridLayoutManager.VERTICAL, false);
        mRecyclerView.setLayoutManager(mLayoutManager);
        mRecyclerView.setAdapter(mRecyclerAdapter);
        setupListeners();
        return mRootView;
    }

    private void setupListeners() {
        mRefreshLayout.setOnRefreshListener(new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                mUiHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mRefreshLayout.setRefreshing(false);
                    }
                }, 2000);
            }
        });
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


    /**
     * Simple adapter with CardView including an image and an text
     * in it for AbsFragment
     * @Auther Cocoonshu
     * @Date 2016-06-12 16:50
     * Copyright (c) 2016 Cocoonshu
     */
    private static class SimpleRecyclerViewAdapter extends RecyclerView.Adapter {

        Context mContext = null;

        private static SimpleRecyclerViewAdapter createFakeDataSetAdapter(Context context) {
            SimpleRecyclerViewAdapter adapter = new SimpleRecyclerViewAdapter(context) {
                @Override
                public int getItemCount() {
                    return Integer.MAX_VALUE;
                }
                @Override
                public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
                    SimpleRecyclerViewHolder viewHolder = (SimpleRecyclerViewHolder) holder;
                    viewHolder.setText("# " + position);
                }
            };
            return adapter;
        }

        public SimpleRecyclerViewAdapter(Context context) {
            mContext = context;
        }

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            View itemView = LayoutInflater.from(mContext).inflate(R.layout.simple_recycler_view_card_item, null);
            itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {

                }
            });
            return new SimpleRecyclerViewHolder(itemView);
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
            SimpleRecyclerViewHolder viewHolder = (SimpleRecyclerViewHolder) holder;
            viewHolder.setText("# " + position);
        }

        @Override
        public int getItemCount() {
            return 0;
        }

    }

    private static class SimpleRecyclerViewHolder extends RecyclerView.ViewHolder {
        private CardView  mRootView = null;
        private ImageView mIgvImage = null;
        private TextView  mTxtTitle = null;

        public SimpleRecyclerViewHolder(View itemView) {
            super(itemView);
            mRootView = (CardView)  itemView.findViewById(R.id.CardView);
            mIgvImage = (ImageView) itemView.findViewById(R.id.ImageView_Image);
            mTxtTitle = (TextView)  itemView.findViewById(R.id.TextView_Title);
        }

        public void setImageDrawable(Drawable drawable) {
            mIgvImage.setImageDrawable(drawable);
        }

        public void setImageBitmap(Bitmap bitmap) {
            mIgvImage.setImageBitmap(bitmap);
        }

        public void setText(int resid) {
            mTxtTitle.setText(resid);
        }

        public void setText(CharSequence text) {
            mTxtTitle.setText(text);
        }
    }

}
