package me.shiki.editor

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {
    init {
        System.loadLibrary("editor")
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        native_CodecInfo()
    }

    external fun native_CodecInfo()
}
