package cn.geekiot.sparklebike.theme;

import android.content.Context;

import java.util.Map;

/**
 * Theme Parser interface
 * @Auther Cocoonshu
 * @Date 2016-09-14 14:01:03
 * Copyright (c) 2016 Cocoonshu
 */
public interface ThemeParser<K, V> {
    public Map<K, V> parseThemeAssetsResource(Context context, String asset);
    public Map<K, V> parseThemeFileResource(String file);
}
