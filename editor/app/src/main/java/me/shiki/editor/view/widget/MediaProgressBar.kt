package me.shiki.editor.view.widget

import android.content.Context
import android.graphics.Color
import android.graphics.Rect
import android.net.Uri
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.RelativeLayout
import me.shiki.editor.R
import me.shiki.editor.Utils
import kotlin.math.log


/**
 *
 * @author shiki
 * @date 2022/6/13
 *
 */
class MediaProgressBar @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0,
    defStyleRes: Int = 0
) :
    RelativeLayout(context, attrs, defStyleAttr, defStyleRes) {

    companion object {
        private const val TAG = "MediaProgressBar"
        private const val MoveOffset = 30
    }

    var imgSize: Int = 10
        private set
    private val imgViewList: Array<ImageView?>
    private val pointer: View
    private var pointerWidth = 0
    private var isPointerMove = false

    var onProgressBarChangeListener: ((value: Float) -> Unit)? = null

    init {
        val attrs = context.obtainStyledAttributes(attrs, R.styleable.MediaProgressBar)

        imgSize = attrs.getInt(R.styleable.MediaProgressBar_img_size, imgSize)

        val linearLayout = LinearLayout(context)
        linearLayout.orientation = LinearLayout.HORIZONTAL
        linearLayout.weightSum = imgSize.toFloat()
        addView(linearLayout, LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT))


        val imgLayoutParams = LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT,
            LinearLayout.LayoutParams.MATCH_PARENT
        )
        imgLayoutParams.weight = 1f
        imgViewList = Array(imgSize) {
            null
        }
        for (i in 0 until imgSize) {
            val imgView = ImageView(context)
            imgView.scaleType = ImageView.ScaleType.FIT_XY
            imgViewList[i] = imgView
            linearLayout.addView(imgView, imgLayoutParams)
        }

        pointer = View(context)
        pointer.setBackgroundColor(Color.WHITE)
        pointerWidth = Utils.px2sp(10f)
        pointer.layoutParams = LayoutParams(pointerWidth, LayoutParams.MATCH_PARENT)
        addView(pointer)
        attrs.recycle()
    }


    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        var newHeightMeasureSpec = heightMeasureSpec
        val heightMode = MeasureSpec.getMode(heightMeasureSpec)
        var heightSize = MeasureSpec.getSize(heightMeasureSpec)

        val widthMode = MeasureSpec.getMode(widthMeasureSpec)
        val widthSize = MeasureSpec.getSize(widthMeasureSpec)
        if (heightMode == MeasureSpec.AT_MOST) {
            if (widthMode == MeasureSpec.EXACTLY) {
                heightSize = widthSize / imgSize
            }
            newHeightMeasureSpec = MeasureSpec.makeMeasureSpec(heightSize, MeasureSpec.EXACTLY)
        }
        super.onMeasure(widthMeasureSpec, newHeightMeasureSpec)
    }

    fun setImgView(index: Int, filename: String) {
        imgViewList[index]?.setImageURI(Uri.parse(filename))
    }

    override fun detachViewFromParent(child: View?) {
        for (i in imgViewList.indices) {
            imgViewList[i] = null
        }
        super.detachViewFromParent(child)
    }

    override fun onInterceptTouchEvent(ev: MotionEvent?): Boolean {
        return true
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        val x = event?.x ?: 0f
        val y = event?.y ?: 0f
        when (event?.action) {
            MotionEvent.ACTION_DOWN -> {
                val rect = Rect(pointer.left - MoveOffset, pointer.top, pointer.right + MoveOffset, pointer.bottom)
                isPointerMove = rect.contains(x.toInt(), y.toInt())
            }
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_MOVE -> {
                if (isPointerMove) {
                    val left = (x - pointerWidth)
                    onProgressBarChangeListener?.invoke(left / width)
                    pointer.layout(left.toInt(), pointer.top, (pointerWidth + x).toInt(), pointer.bottom)
                    invalidate()
                }
            }
            MotionEvent.ACTION_UP -> {
                isPointerMove = false
            }
        }
        return true
    }
}