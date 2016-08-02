package cn.geekiot.sparklebike.ui;

import android.support.v7.widget.CardView;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import cn.geekiot.sparklebike.R;

/**
 * Created by Sardine on 2016/8/2.
 */
public class TestRecycleViewHelper {

    private RecyclerView mHostView = null;
    private TestAdapter  mAdapter  = null;

    public TestRecycleViewHelper(RecyclerView hostView) {
        mHostView = hostView;
        mAdapter  = new TestAdapter();
        mHostView.setAdapter(mAdapter);
        LinearLayoutManager manager = new LinearLayoutManager(mHostView.getContext());
        manager.setOrientation(LinearLayoutManager.VERTICAL);
        mHostView.setLayoutManager(manager);
    }

    private class ItemView extends RecyclerView.ViewHolder {

        public CardView mCardView  = null;
        public TextView mTitleView = null;

        public ItemView(View itemView) {
            super(itemView);
            mCardView  = (CardView) itemView.findViewById(R.id.CardView_ItemRootLayout);
            mTitleView = (TextView) itemView.findViewById(R.id.TextView_Title);
        }

    }

    private class TestAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            View contentView = LayoutInflater.from(mHostView.getContext()).inflate(R.layout.item_test, null);
            return new ItemView(contentView);
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
            TextView title = ((ItemView)(holder)).mTitleView;
            title.setText("#" + position + " Test content for recycler view item");
        }

        @Override
        public int getItemCount() {
            return 100;
        }
    }

}
