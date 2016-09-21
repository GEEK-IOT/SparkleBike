package cn.geekiot.sparklebike.theme;

import android.content.Context;
import android.graphics.Color;
import android.util.Log;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.util.HashMap;
import java.util.Map;

/**
 * Material Design theme resource file parser
 * @Auther Cocoonshu
 * @Date 2016-09-21 11:58:47
 * Copyright (c) 2016 Cocoonshu
 */
public class MaterialDesignThemeParser implements ThemeParser {

    public static final String TAG                     = "MaterialDesignParser";
    public static final String KEY_THEME_XML_RESOURCES = "resources";
    public static final String KEY_THEME_XML_COLOR     = "color";
    public static final String KEY_THEME_XML_NAME      = "name";

    public MaterialDesignThemeParser() {

    }

    public Map<String, Integer> parseThemeAssetsResource(Context context, String asset) {
        XmlPullParserFactory factory = null;
        XmlPullParser        parser  = null;
        InputStreamReader    reader  = null;
        try {
            factory = XmlPullParserFactory.newInstance();
            parser  = factory.newPullParser();
            reader  = new InputStreamReader(context.getAssets().open(asset), Charset.defaultCharset().name());
            parser.setInput(reader);
            return innerParseImpl(parser);
        } catch (IOException exp) {
            Log.e(TAG, "Asset = " + asset + " is not found", exp);
        } catch (XmlPullParserException exp) {
            Log.e(TAG, "Asset = " + asset + " is parse error", exp);
        } catch (IllegalArgumentException exp) {
            Log.e(TAG, "Asset = " + asset + " is parse error, color format is illegal", exp);
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

    public Map<String, Integer> parseThemeFileResource(String file) {
        XmlPullParserFactory factory = null;
        XmlPullParser        parser  = null;
        InputStreamReader    reader  = null;
        try {
            factory = XmlPullParserFactory.newInstance();
            parser  = factory.newPullParser();
            reader  = new InputStreamReader(new FileInputStream(file), Charset.defaultCharset().name());
            parser.setInput(reader);
            return innerParseImpl(parser);
        } catch (IOException exp) {
            Log.e(TAG, "File = " + file + " is not found", exp);
        } catch (XmlPullParserException exp) {
            Log.e(TAG, "File = " + file + " is parse error", exp);
        } catch (IllegalArgumentException exp) {
            Log.e(TAG, "File = " + file + " is parse error, color format is illegal", exp);
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        return null;
    }

    private Map<String, Integer> innerParseImpl(XmlPullParser parser) throws IOException,
                                                                             XmlPullParserException,
                                                                             IllegalArgumentException {
        Map<String, Integer> map = new HashMap<String, Integer>();
        do {
            int xmlEvent = parser.getEventType();
            switch (xmlEvent) {
                case XmlPullParser.START_DOCUMENT: {
                    // Do nothing
                }
                break;

                case XmlPullParser.START_TAG: {
                    // Create Solution and Picture
                    String tagName = parser.getName();
                    if (tagName.equalsIgnoreCase(KEY_THEME_XML_RESOURCES)) {

                    } else if (tagName.equalsIgnoreCase(KEY_THEME_XML_COLOR)) {
                        String attrName  = parser.getAttributeValue(null, KEY_THEME_XML_NAME);
                        String textValue = parser.nextText();
                        Log.i(TAG, String.format("[innerParseImpl] <%s %s=\"%s\">%s</%s>",
                                tagName, KEY_THEME_XML_NAME, attrName, textValue, tagName));
                        if (MaterialDesignTheme.hasAttributeKey(attrName)) {
                            Integer value = Color.parseColor(textValue);
                            map.put(attrName.trim().toLowerCase(), value);
                        }
                    }
                }
                break;

                case XmlPullParser.END_TAG: {
                    // Do nothing
                }
                break;

                case XmlPullParser.END_DOCUMENT: {
                    // Do nothing, it will exit loop later
                }
                break;
            }
        } while (parser.next() != XmlPullParser.END_DOCUMENT);

        return map;
    }
}
