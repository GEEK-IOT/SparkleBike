package cn.geekiot.sparklebike.ui;

import com.stylingandroid.prism.Prism;
import com.stylingandroid.prism.filter.Filter;

import java.util.ArrayList;
import java.util.List;

/**
 * ThemeColorHelper
 * @Auther Cocoonshu
 * @Date 2016-09-14 14:01:03
 * Copyright (c) 2016 Cocoonshu
 */
public class ThemeColorHelper {

    private Prism       mThemePrism             = null;
    private List<int[]> mThemedColorPaletteList = null;

    public ThemeColorHelper() {
        mThemedColorPaletteList = new ArrayList<int[]>();
    }

    public Prism.Builder buildPalette() {
        return Prism.Builder.newInstance();
    }

    public void finishPaletteBuild(Prism prism) {
        mThemePrism = prism;
    }

    public void triggerNextColor() {
    }

    public class Design {

        public class PrimaryFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class PrimaryDarkFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class PrimaryLightFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class AccentFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class PrimaryTextFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class SeconderyTextFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class TextIconFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class DividerLineFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class RipplerFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }

        public class BackgroundFilter implements Filter {
            @Override
            public Integer filter(Integer colour) {
                return null;
            }
        }
    }

}
