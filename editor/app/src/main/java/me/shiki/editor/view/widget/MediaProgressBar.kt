package me.shiki.editor.view.widget

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Rect
import android.net.Uri
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.RelativeLayout
import me.shiki.editor.R
import me.shiki.editor.Utils


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
    private var imgWidth = 0

    private lateinit var imgViewList: Array<ImageView?>
    private lateinit var imgListLayout: LinearLayout

    private lateinit var pointer: View
    private val pointerRect by lazy {
        Rect()
    }
    private var pointerWidth = 0
    private var isPointerMove = false

    private lateinit var leftPointer: View
    private var isLeftPointerMove = false
    private val leftPointerRect by lazy {
        Rect()
    }

    private lateinit var rightPointer: View
    private var isRightPointerMove = false
    private val rightPointerRect by lazy {
        Rect()
    }

    private var borderWidth = 0
    private var borderColor = Color.RED
    private lateinit var borderPaint: Paint


    var onProgressBarPointChangeListener: ((value: Float) -> Unit)? = null

    var onProgressBarLeftRightPointChangeListener: ((leftValue: Float, rightValue: Float) -> Unit)? = null

    init {
        val attrs = context.obtainStyledAttributes(attrs, R.styleable.MediaProgressBar)

        imgSize = attrs.getInt(R.styleable.MediaProgressBar_img_size, imgSize)
        borderWidth = Utils.px2sp(10f)


        initImgViewList(context)
        initPointer(context)
        initLeftRightPointer(context)

        attrs.recycle()
    }

    private fun initLeftRightPointer(context: Context) {

        borderPaint = Paint()
        borderPaint.color = borderColor
        borderPaint.strokeWidth = borderWidth.toFloat()
        borderPaint.isAntiAlias = true
        borderPaint.style = Paint.Style.FILL

        leftPointer = View(context)
        leftPointer.setBackgroundColor(borderColor)
        val leftLayoutParams = LayoutParams(borderWidth, LayoutParams.MATCH_PARENT)
        leftLayoutParams.addRule(ALIGN_PARENT_LEFT, TRUE)
        leftPointer.layoutParams = leftLayoutParams
        addView(leftPointer)

        rightPointer = View(context)
        rightPointer.setBackgroundColor(borderColor)
        val rightLayoutParams = LayoutParams(borderWidth, LayoutParams.MATCH_PARENT)
        rightLayoutParams.addRule(ALIGN_PARENT_RIGHT, TRUE)
        rightPointer.layoutParams = rightLayoutParams
        addView(rightPointer)
    }

    private fun initImgViewList(context: Context) {
        imgListLayout = LinearLayout(context)
        imgListLayout.orientation = LinearLayout.HORIZONTAL
        imgListLayout.weightSum = imgSize.toFloat()
        val layoutParams = LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT)
        layoutParams.setMargins(borderWidth, borderWidth, borderWidth, borderWidth)
        addView(imgListLayout, layoutParams)


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
            imgListLayout.addView(imgView, imgLayoutParams)
        }
    }

    private fun initPointer(context: Context) {
        pointer = View(context)
        pointer.setBackgroundColor(Color.WHITE)
        pointerWidth = Utils.px2sp(10f)
        val layoutParams = LayoutParams(pointerWidth, LayoutParams.MATCH_PARENT)
        layoutParams.setMargins(borderWidth, borderWidth, borderWidth, borderWidth)
        pointer.layoutParams = layoutParams
        addView(pointer)
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
        imgWidth = width / imgSize
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
                pointerRect.set(pointer.left - MoveOffset, pointer.top, pointer.right + MoveOffset, pointer.bottom)
                isPointerMove = pointerRect.contains(x.toInt(), y.toInt())

                leftPointerRect.set(
                    leftPointer.left - MoveOffset,
                    leftPointer.top,
                    leftPointer.right + MoveOffset,
                    leftPointer.bottom
                )
                isLeftPointerMove = leftPointerRect.contains(x.toInt(), y.toInt())

                rightPointerRect.set(
                    rightPointer.left - MoveOffset,
                    rightPointer.top,
                    rightPointer.right + MoveOffset,
                    rightPointer.bottom
                )
                isRightPointerMove = rightPointerRect.contains(x.toInt(), y.toInt())
            }
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_MOVE -> {
                if (isLeftPointerMove) {
                    val pointerX = moveLeftRightPointer(leftPointer, x)
                    if (leftPointer.left >= pointer.left) {
                        movePointer(pointerX + borderWidth, true)
                    }
                } else if (isRightPointerMove) {
                    val pointerX = moveLeftRightPointer(rightPointer, x)
                    if (rightPointer.right <= pointer.right) {
                        movePointer(pointerX - borderWidth, true)
                    }
                } else if (isPointerMove) {
                    movePointer(x)
                }

                if (isLeftPointerMove || isPointerMove || isRightPointerMove) {
                    invalidate()
                }
            }
            MotionEvent.ACTION_UP -> {
                isPointerMove = false
                isLeftPointerMove = false
                isRightPointerMove = false
            }
        }
        return true
    }

    private fun moveLeftRightPointer(pointer: View, x: Float): Float {
        var left = x - borderWidth / 2
        var right = left + borderWidth
        when (pointer) {
            leftPointer -> {
                if (right > rightPointer.left - imgWidth) {
                    left = pointer.left.toFloat()
                    right = pointer.right.toFloat()
                }
            }
            rightPointer -> {
                if (left < leftPointer.right + imgWidth) {
                    left = pointer.left.toFloat()
                    right = pointer.right.toFloat()
                }
            }
            else -> {
            }
        }
        pointer.layout(left.toInt(), pointer.top, right.toInt(), pointer.bottom)
        val width = imgListLayout.width.toFloat()
        onProgressBarLeftRightPointChangeListener?.invoke(
            leftPointer.right / width,
            rightPointer.left / width
        )
        return if (pointer == leftPointer) {
            left
        } else {
            right
        }
    }

    private fun movePointer(x: Float, isMoveLeftRightPointer: Boolean = false) {
        val left = x - pointerWidth / 2
        val right = left + pointerWidth
        if ((leftPointer.right < x && x < rightPointer.left) || isMoveLeftRightPointer) {
            onProgressBarPointChangeListener?.invoke(left / width)
            pointer.layout(left.toInt(), pointer.top, right.toInt(), pointer.bottom)
        }
    }

    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas)
        canvas?.save()
        canvas?.drawLine(leftPointer.x, 0.5f, rightPointer.x, borderWidth.toFloat(), borderPaint)
        canvas?.drawLine(leftPointer.x, height - borderWidth.toFloat(), rightPointer.x, height.toFloat(), borderPaint)
        canvas?.restore()
    }
}