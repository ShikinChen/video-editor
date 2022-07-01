package me.shiki.editor

import android.Manifest
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.Settings
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.widget.Button
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.permissionx.guolindev.PermissionX
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import me.shiki.editor.view.widget.MediaProgressBar
import me.shiki.editor.view.widget.media.IjkVideoView
import tv.danmaku.ijk.media.player.IjkMediaPlayer
import java.io.File


class MainActivity : AppCompatActivity() {

    companion object {
        private val TAG = "MainActivity"
    }

    private val mediaEditor by lazy {
        MediaEditor()
    }

    private val mediaProgressBar: MediaProgressBar by lazy {
        findViewById(R.id.media_progress_bar)
    }

    private val leftRotateBtn: Button by lazy {
        findViewById(R.id.left_rotate_btn)
    }

    private val rightRotateBtn: Button by lazy {
        findViewById(R.id.right_rotate_btn)
    }

    private val videoView: IjkVideoView by lazy {
        findViewById(R.id.video_view)
    }

    private val progressTitleTv: TextView by lazy {
        findViewById(R.id.progress_title_tv)
    }

    private val encodeBar: ProgressBar by lazy {
        findViewById(R.id.encode_bar)
    }

    private val sdPath by lazy {
        Environment.getExternalStorageDirectory().absolutePath
    }
    private val inFilename by lazy {
        "${sdPath}/sample_1280x720_surfing_with_audio.mp4"
    }
    private val saveFilename by lazy {
        "${sdPath}/save.mp4"
    }

    private var duration: Long = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


        initIjk()

        mediaEditor.init()
        mediaEditor.encodingDecodingInfo()
        mediaEditor.dumpImageCallback = { filename, index ->
            lifecycleScope.launch(Dispatchers.Main) {
                mediaProgressBar.setImgView(index, filename)
            }
        }
        mediaEditor.muxingCallback = { filename, currMillisecond, totalMillisecond ->
            Log.d(TAG, "currMillisecond:$currMillisecond,totalMillisecond:$totalMillisecond")
            val progress = ((currMillisecond.toFloat() / totalMillisecond) * 100).toInt()
            lifecycleScope.launch(Dispatchers.Main) {
                progressTitleTv.text = "${resources.getString(R.string.save_progress)}:${progress}%"
                encodeBar.progress = progress
                if (progress >= 100) {
                    Toast.makeText(this@MainActivity, "保存文件路径:${saveFilename}", Toast.LENGTH_LONG).show()
                }
            }
        }
        mediaProgressBar.onProgressBarPointChangeListener = {
            Log.d(TAG, "value:$it")
            val duration = mediaEditor.duration()
            videoView.seekTo((duration * it).toInt())
        }
        mediaProgressBar.onProgressBarLeftRightPointChangeListener = { leftValue, rightValue ->
            mediaEditor.startTime = (duration * leftValue).toLong()
            mediaEditor.endTime = (duration * rightValue).toLong()
            Log.d(TAG, "left:$leftValue\tright:$rightValue")
            Log.d(TAG, "startTime:${mediaEditor.startTime}\tendTime:${mediaEditor.endTime}")
        }

        rightRotateBtn.setOnClickListener {
            videoView.setVideoRotation(videoView.videoRotationDegree + 90)
            mediaEditor.rotateDegrees += 90
        }

        leftRotateBtn.setOnClickListener {
            videoView.setVideoRotation(videoView.videoRotationDegree - 90)
            mediaEditor.rotateDegrees -= 90
        }

        PermissionX.init(this)
            .permissions(
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
            ).request { allGranted, grantedList, deniedList ->
                if (allGranted) {
                    checkStorageManagerPermission()
                    mediaEditor.open(inFilename)
                    val outFile = File("${sdPath}/tmp/out")
                    if (!outFile.exists()) {
                        outFile.mkdirs()
                    }
                    duration = mediaEditor.duration()
                    mediaEditor.endTime = duration
                    mediaEditor.dumpImageList(mediaProgressBar.imgSize, 640, 320, outFile.absolutePath)
                    Log.d(TAG, "duration:${mediaEditor.duration()}")
                }
            }
    }

    private fun initIjk() {
        IjkMediaPlayer.loadLibrariesOnce(null)
        IjkMediaPlayer.native_profileBegin("libijkplayer.so")
        videoView.setVideoPath(inFilename)
        //TODO 暂时使用TextureView为了预览时候可以旋转
        videoView.setRender(IjkVideoView.RENDER_TEXTURE_VIEW)
//        videoView.start()
    }

    private fun checkStorageManagerPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R &&
            !Environment.isExternalStorageManager()
        ) {
            Toast.makeText(this, R.string.storage_manager_permission_tip, Toast.LENGTH_LONG).show()
            val intent = Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            startActivity(intent)
        }
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.menu, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            R.id.save -> {
                Log.d(TAG, "rotateDegrees:${mediaEditor.rotateDegrees}")
                //TODO 为测试方便暂时写死截取时间段
                mediaEditor.save(saveFilename, 20000, 30000)
            }
        }
        return super.onOptionsItemSelected(item)
    }
}