package cn.geekiot.sparklebike;

import android.content.Context;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.zip.Inflater;

/**
 * RecyclerViewHelper
 * @Auther Cocoonshu
 * @Date 2016-04-14 10:29
 * Copyright (c) 2016 Cocoonshu
 */
public class RecyclerViewHelper {

    private static final String TAG = "RecyclerViewHelper";

    private RecyclerView        mHostView      = null;
    private InternalAdapter     mAdapter       = null;
    private LinearLayoutManager mLayoutManager = null;

    public RecyclerViewHelper(RecyclerView host) {
        mHostView = host;
        mLayoutManager = new LinearLayoutManager(mHostView.getContext());
        mAdapter = new InternalAdapter(mHostView.getContext());
        Log.e(TAG, "[RecyclerViewHelper]");
    }

    public void setupRecycleView() {
        mHostView.setLayoutManager(mLayoutManager);
        mHostView.setMotionEventSplittingEnabled(true);
        mHostView.setClickable(true);
        mHostView.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                Log.e(TAG, "[onClick]");
            }

        });

        mHostView.addOnItemTouchListener(new RecyclerView.OnItemTouchListener() {

            @Override
            public boolean onInterceptTouchEvent(RecyclerView rv, MotionEvent e) {
                Log.e(TAG, "[onInterceptTouchEvent]");
                return false;
            }

            @Override
            public void onTouchEvent(RecyclerView rv, MotionEvent e) {
                Log.e(TAG, "[onTouchEvent]");
            }

            @Override
            public void onRequestDisallowInterceptTouchEvent(boolean disallowIntercept) {
                Log.e(TAG, "[onRequestDisallowInterceptTouchEvent]");
            }

        });

        mHostView.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                Log.e(TAG, "[onTouchEvent] mHostView onTouchEvent");
                return false;
            }

        });
    }

    public void fillDataSet() {
        mHostView.setAdapter(mAdapter);
    }

    private class InternalAdapter extends RecyclerView.Adapter {

        private Context mContext = null;

        public InternalAdapter(Context context) {
            mContext = context;
        }

        private String[] mContentSet = new String[] {
                "@刀锋偏冷 如果是纯色的判断点击位置的像素点应该可以实现、",
                "@[QQ红包]请使用新版手机QQ查收红包。  可否知道视频点播 可有第三方框架、？",
                "https://github.com/SinaVDDeveloper",
                "好的 我看看"
        };

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            final LayoutInflater     inflater    = LayoutInflater.from(mContext);
            final View               contentView = inflater.inflate(R.layout.item_text_layout, null);
            final InternalViewHolder viewHolder  = new InternalViewHolder(contentView);
            contentView.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    Log.e(TAG, "[onClick] position = " + viewHolder.getAdapterPosition());
                }

            });
            return viewHolder;
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
            TextView contentView = (TextView) (holder.itemView.findViewById(R.id.TextView_TextContent));
            contentView.setText(mContentSet[position]);
        }

        @Override
        public int getItemCount() {
            return mContentSet.length;
        }

    }

    private class InternalViewHolder extends RecyclerView.ViewHolder {

        public InternalViewHolder(View itemView) {
            super(itemView);
        }

    }
}
