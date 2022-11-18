package com.example.cameraapp

import android.Manifest
import android.app.Fragment
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.hardware.camera2.CameraAccessException
import android.hardware.camera2.CameraManager
import android.media.Image
import android.media.ImageReader
import android.media.ImageReader.OnImageAvailableListener
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Base64
import android.util.Log
import android.util.Size
import android.view.Surface
import android.view.View
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import org.eclipse.paho.client.mqttv3.MqttException
import java.io.ByteArrayOutputStream

lateinit var mqttClient: MQTTClient
class MainActivity : AppCompatActivity(), OnImageAvailableListener {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkSelfPermission(Manifest.permission.CAMERA) == PackageManager.PERMISSION_DENIED
            ) {
                val permission = arrayOf(
                    Manifest.permission.CAMERA

                )
                requestPermissions(permission, 1122)
            } else {
                setFragment()

            }
        } else {
            setFragment()
        }

        val buttonConnectToServer = findViewById<Button>(R.id.connect)
        buttonConnectToServer.setOnClickListener {
            connectToServer()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String?>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            setFragment()
        } else {
            finish()
        }
    }

    var previewHeight = 0
    var previewWidth = 0
    var sensorOrientation = 0
    protected fun setFragment() {
        val manager =
            getSystemService(Context.CAMERA_SERVICE) as CameraManager
        var cameraId: String? = null
        try {
            cameraId = manager.cameraIdList[0]
        } catch (e: CameraAccessException) {
            e.printStackTrace()
        }
        val fragment: Fragment
        val camera2Fragment = CameraConnectionFragment.newInstance(
            object :
                CameraConnectionFragment.ConnectionCallback {
                override fun onPreviewSizeChosen(size: Size?, rotation: Int) {
                    previewHeight = size!!.height
                    previewWidth = size.width
                    sensorOrientation = rotation - getScreenOrientation()
                }
            },
            this,
            R.layout.camera_fragment,
            // TODO: HAVE CONTROL PARAMETERS
            Size(640, 480)
        )
        camera2Fragment.setCamera(cameraId)
        fragment = camera2Fragment
        fragmentManager.beginTransaction().replace(R.id.container, fragment).commit()
    }
    protected fun getScreenOrientation(): Int {
        return when (windowManager.defaultDisplay.rotation) {
            Surface.ROTATION_270 -> 270
            Surface.ROTATION_180 -> 180
            Surface.ROTATION_90 -> 90
            else -> 0
        }
    }

    private var isProcessingFrame = false
    private val yuvBytes = arrayOfNulls<ByteArray>(3)
    private var rgbBytes: IntArray? = null
    private var yRowStride = 0
    private var postInferenceCallback: Runnable? = null
    private var imageConverter: Runnable? = null
    private var rgbFrameBitmap: Bitmap? = null
    private var encodedImage: String? = null
    private var imageProcessCounter: Int = 0
    private var fps = 8
    private var frequency = 60/fps
    override fun onImageAvailable(reader: ImageReader) {
        // We need wait until we have some size from onPreviewSizeChosen
//        Log.d("tag", imageProcessCounter.toString())
        imageProcessCounter += 1
        if (imageProcessCounter%frequency != 0) {
            try {
                reader.acquireLatestImage().close()
//                imageProcessCounter = 0
            } catch (e: Exception) {
//                Log.d("TAG", "image process failed")
//                imageProcessCounter = frequency - 1
//                e.printStackTrace()
            }
            return
        }
        if (previewWidth == 0 || previewHeight == 0) {
            return
        }
        if (rgbBytes == null) {
            rgbBytes = IntArray(previewWidth * previewHeight)
        }
        try {
            val image = reader.acquireLatestImage() ?: return
            if (isProcessingFrame) {
                image.close()
                return
            }
            isProcessingFrame = true
            val planes = image.planes
            fillBytes(planes, yuvBytes)
            yRowStride = planes[0].rowStride
            val uvRowStride = planes[1].rowStride
            val uvPixelStride = planes[1].pixelStride
            imageConverter = Runnable {
                ImageUtils.convertYUV420ToARGB8888(
                    yuvBytes[0]!!,
                    yuvBytes[1]!!,
                    yuvBytes[2]!!,
                    previewWidth,
                    previewHeight,
                    yRowStride,
                    uvRowStride,
                    uvPixelStride,
                    rgbBytes!!
                )
            }
            postInferenceCallback = Runnable {
                image.close()
                isProcessingFrame = false
            }
            processImage()
            sendImageToServer(encodedImage)
        } catch (e: Exception) {
            return
        }
    }


    private fun processImage() {
        imageConverter!!.run()
        rgbFrameBitmap = Bitmap.createBitmap(previewWidth, previewHeight, Bitmap.Config.ARGB_8888)
        rgbFrameBitmap?.setPixels(rgbBytes, 0, previewWidth, 0, 0, previewWidth, previewHeight)
        encodedImage = encodeImage(rgbFrameBitmap)
        Log.d("TAG", "processing")
        postInferenceCallback!!.run()
    }

    protected fun fillBytes(
        planes: Array<Image.Plane>,
        yuvBytes: Array<ByteArray?>
    ) {
        // Because of the variable row stride it's not possible to know in
        // advance the actual necessary dimensions of the yuv planes.
        for (i in planes.indices) {
            val buffer = planes[i].buffer
            if (yuvBytes[i] == null) {
                yuvBytes[i] = ByteArray(buffer.capacity())
            }
            buffer[yuvBytes[i]]
        }
    }

    private fun encodeImage(bm: Bitmap?): String? {
        val baos = ByteArrayOutputStream()
        bm?.compress(Bitmap.CompressFormat.JPEG, 100, baos)
        val b = baos.toByteArray()
        return Base64.encodeToString(b, Base64.DEFAULT)
    }

    private var isConnectedToServer = false
    private fun connectToServer() {
        if (isConnectedToServer) return
        val serverURI = "tcp://192.168.200.92:1883"
        val serverUriFromInput = findViewById<EditText>(R.id.server_uri).text.toString()
//        val serverURI = "tcp://$serverUriFromInput:1883"
        val clientId = "phone"
        val username = "test"
        val pwd = "test"

        // Open MQTT Broker communication
        mqttClient = MQTTClient(this, serverURI, clientId)

        try {
            mqttClient.connect(username, pwd)
            isConnectedToServer = true
            Toast.makeText(this, "connected to $serverUriFromInput!", Toast.LENGTH_SHORT).show()
        } catch (e: MqttException) {
            Toast.makeText(this, "failed to connect to $serverUriFromInput, please try again", Toast.LENGTH_SHORT).show()
            isConnectedToServer = false
        }
    }

    private fun sendImageToServer(image: String?) {
        if (!isConnectedToServer) return
        if (imageProcessCounter%frequency == 0) {
            mqttClient.publish("image", image!!)
        }
    }
}