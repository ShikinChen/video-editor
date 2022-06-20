package me.shiki.editor

import android.content.res.Resources

/**
 *
 * @author shiki
 * @date 2022/6/13
 *
 */
class Utils {
    companion object {
        @JvmStatic
        fun sp2px(spValue: Float): Int {
            val fontScale: Float = Resources.getSystem().displayMetrics.scaledDensity
            return (spValue * fontScale + 0.5f).toInt()
        }

        @JvmStatic
        fun px2sp(pxValue: Float): Int {
            val fontScale = Resources.getSystem().displayMetrics.scaledDensity
            return (pxValue / fontScale + 0.5f).toInt()
        }
    }
}