package me.shiki.editor

import android.util.Log

/**
 *
 * @author shiki
 * @date 2022/6/8
 *
 */
class MediaEditor {
    companion object {
        private val TAG = "MediaEditor"

        init {
            System.loadLibrary("media_editor")
        }
    }

    var dumpImageCallback: ((filename: String, index: Int) -> Unit)? = null

    fun init() {
        native_Init()
    }

    fun encodingDecodingInfo() {
        native_EncodingDecodingInfo()
    }

    fun open(filename: String) {
        native_Open(filename)
    }

    fun dumpImageList(
        imgSize: Int, imgWidth: Int,
        imgHeight: Int, outFilename: String, startTime: Long = 0, endTime: Long = duration(),
    ) {
        native_DumpImageList(startTime, endTime, imgSize, imgWidth, imgHeight, outFilename)
    }

    private fun dumpImageListCallback(filename: String, index: Int) {
        Log.d(TAG, "filename:$filename,index:$index")
        dumpImageCallback?.invoke(filename, index)
    }

    fun duration(): Long {
        return native_Duration()
    }

    private external fun native_Open(filename: String)

    private external fun native_EncodingDecodingInfo()

    private external fun native_DumpImageList(
        startTime: Long, endTime: Long, imgSize: Int, imgWidth: Int,
        imgHeight: Int,
        outFilename: String
    )

    private external fun native_Duration(): Long

    private external fun native_Init()
}