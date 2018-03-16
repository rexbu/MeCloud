package rex.com.mecloudcmake

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.text.method.ScrollingMovementMethod
import android.util.Log
import android.view.View
import android.widget.ImageView
import com.rex.mecloud.*
import com.rex.mecloud.MeCloud.shareInstance
import com.rex.medemo.R
import com.rex.utils.DeviceUtil
import kotlinx.android.synthetic.main.activity_main.*
import java.io.IOException


class MainActivity : AppCompatActivity() {
  private var meObjectId: String? = null
  private var filePath: String? = null
  private var aclPre: Long = 0
  private var meUserId: String? = null

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)

    MeCloud.shareInstance().addHttpHeader("name", "super")
    MeCloud.shareInstance().setShowLog(true)
    setContentView(R.layout.activity_main)
    val view = findViewById(R.id.imageView) as ImageView
    MeCloud.shareInstance().setTimeout(5)



    sample_text.movementMethod = ScrollingMovementMethod.getInstance()
    imageView.visibility = View.GONE
    btn_login.setOnClickListener({
      MeCloud.shareInstance().createDeviceId(application)
//      iv.visibility = View.GONE
      login()
//      iv.visibility = View.VISIBLE
      //http://n01.me-yun.com:8000/1.0/file/download/59e42d6f00000067ae6fe123?x-oss-process=image/resize,w_100,h_100
//      Glide
//        .with(applicationContext)
//        .load("http://n01.me-yun.com:8000/1.0/file/download/59e42d6f00000067ae6fe123")
//        .into(iv)
    })

    btn_signUp.setOnClickListener({
      imageView.visibility = View.GONE
      signUp()
    })

    btn_save.setOnClickListener({
      imageView.visibility = View.GONE
      save()
    })

    btn_change_data.setOnClickListener({
      imageView.visibility = View.GONE
      changeData()
    })

    btn_query.setOnClickListener({
      imageView.visibility = View.GONE
      query()
    })

    btn_query_list.setOnClickListener({
      imageView.visibility = View.GONE
      queryList()
    })

    btn_download.setOnClickListener({
      imageView.visibility = View.VISIBLE
      download()
    })

    btn_upload.setOnClickListener({
      upload()
    })

    btn_create_role.setOnClickListener({
      imageView.visibility = View.GONE
      createRole()
    })

    btn_mecloud_save.setOnClickListener {
      imageView.visibility = View.GONE
      meCloudSave()
    }


    btn_mecloud_get.setOnClickListener {
      imageView.visibility = View.GONE
      meCloudGet()
    }


    btn_Join_query.setOnClickListener {
      imageView.visibility = View.GONE
      meCloudQuery()
    }

    btn_get_url.setOnClickListener {
      imageView.visibility = View.GONE
      meCloudGet()
    }

    btn_get_POST.setOnClickListener {
      imageView.visibility = View.GONE
      meCloudQuery()
    }

  }

  private val meCloudServerUrl = "http://n01.me-yun.com:8000/"



  private fun login() {
    //MeCloud.shareInstance().setDeviceId(application)
    val currentUser = MeUser.current()
    shareInstance().setShowLog(true)
    //if (currentUser == null) {
    sample_text.text = "正在登录..."
    shareInstance().login("supernian1", "1234") { response, err ->
      if (err == null) {
        shareInstance().storeJSONToCache(response, "supernian1")
        var newObject = shareInstance().readJSONFromCache("supernian1")
        sample_text.text = newObject.jsonString()
      } else {
        sample_text.text = "login err!\nerrMsg: ${err?.errMsg}\nerrInfo: ${err?.info}"
      }
    }
    //}
  }

  private fun signUp() {
    val currentUser = MeUser.current()
    shareInstance().setShowLog(true)
    //if (currentUser == null) {
    sample_text.text = "正在注册..."
    var password = MeUser.getEncodePassword("supernian1", "1234")
    shareInstance().changePassword("supernian1", password) { response, error ->
      if (error == null) {
        sample_text.text = response?.jsonString()
      } else {
        sample_text.text = "login err!\nerrMsg: ${error?.errMsg}\nerrInfo: ${error?.info}"
      }
    }
  }

  private fun save() {
    val obj = MeObject("Test")
    obj.put("level", 10)
    obj.put("race", "Orc")
    sample_text.text = "saving..."
    MeCloud.shareInstance().save(obj) { response, error ->
      if (error == null) {
        sample_text.text = response?.jsonString()
      } else {
        sample_text.text = "errMsg: ${error?.errMsg} \n errInfo: ${error?.info}"
      }
    }
  }

  private fun postData() {
    try {
      val source = resources.assets.open("demo.jpg")
      val bytes = ByteArray(source.available())
      source.read(bytes)

      val bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
      val width = bitmap.width
      val height = bitmap.height
      val meUser = MeUser()
      val deviceId = DeviceUtil.deviceId(applicationContext)
      MeCloud.shareInstance().postData("http://n01.me-yun.com:9000/login/device/$deviceId", MeCloud.shareInstance().Bitmap2Bytes(bitmap), object : HttpDataCallback {
        override fun done(response: JSONObject?, err: MeException?) {
          sample_text.text = response?.jsonString()
        }

        override fun progress(written: Long, totalWritten: Long, totalExpectWrite: Long) {
          sample_text.text = "progress written=$written totalWritten=$totalWritten totalExpectWrite=$totalExpectWrite";
        }

      })
      //view.setImageBitmap(rgbaToBitmap(out, width, height))
    } catch (e: IOException) {
      e.printStackTrace()
    }
  }

  private fun changeData() {
    val obj = MeObject("29c88f3033c63f78fd492357", "UserInfo")
    obj.put("nickName", "我是神")
    sample_text.text = "changing..."
    MeCloud.shareInstance().save(obj) { response, error ->
      if (error == null) {
        sample_text.text = response?.jsonString()
      } else {
        sample_text.text = "errMsg: ${error?.errMsg} \n errInfo: ${error?.info}"
      }
    }
  }

  private fun query() {
    MeCloud.shareInstance().getObjectWithID("598f8ec1b4b33e5b862d79ee", "Face") { response, error ->
      if (error == null) {
        val sb: StringBuffer = StringBuffer()
        response?.arrayObject("possible")?.forEach{ sb.append(it.jsonString() + "\n") }
        sample_text.text = sb.toString()
      } else {

      }
    }
  }

  private fun queryList() {
    val meQuery = MeQuery("UserInfo")
    meQuery.whereEqualTo("user", "59f301e5ca714307f5df9d19")
    sample_text.text = "querying..."
    MeCloud.shareInstance().getObjectsWithQuery(meQuery) { response, err ->
      if (err == null) {
        val sb: StringBuffer = StringBuffer()
        response?.forEach { sb.append(it.jsonString() + "\n") }
        sample_text.text = sb.toString()
      } else {
        sample_text.text = "errMsg: ${err.errMsg} \nerrInfo:${err.info}"
      }
    }
  }

  private fun download() {
    val url = "https://timgsa.baidu.com/timg?image&quality=80&size=b9999_10000&sec=1504519316491&di=4524e90707dde3fabbbffe1ebddac2b9&imgtype=0&src=http%3A%2F%2Fb.hiphotos.baidu.com%2Fzhidao%2Fpic%2Fitem%2F1f178a82b9014a90e7eb9d17ac773912b21bee47.jpg"
    val name = "fromBaidu"
    sample_text.text = "downloading..."
    shareInstance().download(name, url, object : HttpFileCallback {
      override fun done(obj: JSONObject?, err: MeException?) {
        try {
          if (err == null) {
            sample_text.text = "download done!"
//            filePath = localPath
            Log.e("TAG", "filePath=" + filePath + "thread=" + Thread.currentThread().name)
            sample_text.text = "filePath = $filePath"
            this@MainActivity.filePath = filePath
            val bmp = BitmapFactory.decodeFile(filePath)
            imageView.setImageBitmap(bmp)
            println("download  success")
          } else {
            sample_text.text = "download err!\nerrMsg:${err.errMsg}\nerrInfo:${err.info}"
          }
        } catch (e: Exception) {
          e.printStackTrace()
        }
      }

      override fun progress(written: Long, totalWritten: Long, totalExpectWrite: Long) {
        sample_text.text = "download progress:\nwriten:${written}\ntotleWriten:${totalWritten}\ntotalExpectWrite:${totalExpectWrite}"
      }
    })
  }

  private fun upload() {
    val source = resources.assets.open("demo.jpg")
    val bytes = ByteArray(source.available())
    sample_text.text = "uploading..."
    MeCloud.shareInstance().uploadData("jpg", bytes, object : HttpFileCallback {
      override fun done(obj: JSONObject?, err: MeException?) {
        sample_text.text = "upload done!\n${obj?.jsonString()}"
      }

      override fun progress(written: Long, totalWritten: Long, totalExpectWrite: Long) {
        sample_text.text = "upload progress:\nwriten:${written}\ntotleWriten:${totalWritten}\ntotalExpectWrite:${totalExpectWrite}"
      }
    })
  }

  private fun createRole() {
    val meRole = MeRole("CoorChice")
    meRole.setUser(meUserId)
    sample_text.text = "saving..."

    MeCloud.shareInstance().save(meRole) { response, error ->
      if (error == null) {
        sample_text.text = response?.jsonString()
      } else {
        sample_text.text = "errMsg: ${error?.errMsg} \n errInfo: ${error?.info}"
      }
    }
  }

  private fun meCloudSave() {
    val meObject = MeObject("Hero")
    meObject.put("level", 20)
    meObject.put("race", "human")
    sample_text.text = "saving..."
//    shareInstance().save(meObject, object : MeCallback {
//      override fun done(response: JSONObject?, err: MeException?) {
//        if (err == null && response != null) {
//          meObjectId = response.stringValue("_id")
//          sample_text.text = response?.jsonString()
//        } else {
//          sample_text.text = "errMsg: ${err?.errMsg} \n errInfo: ${err?.info}"
//        }
//      }
//    })
  }

  private fun meCloudGet() {
    MeCloud.shareInstance().getWithUrl("http://n01.me-yun.com:9000/profile/59ef0437ca71437ccf7d7fbb/list", null) { response, error ->
      if (error == null) {
        var intArray = response?.intArrayValue("action_id")
        sample_text.text = response?.jsonString()
      } else {
        sample_text.text = "errMsg: ${error.errMsg} \nerrInfo:${error.info}"
      }
    }
  }

  private fun meCloudQuery() {
    var joinQuery = MeJoinQuery("Followee")
    joinQuery.matchEqualTo("user", "59c79e48ca714365e3cf1784")
    joinQuery.addForeignTable("UserInfo", "user", "followee", "UserInfo")
    MeCloud.shareInstance().findWithJoin(joinQuery) { response, error ->
      if (error == null) {
        val sb: StringBuffer = StringBuffer()
        response?.forEach { sb.append(it.jsonString() + "\n") }
        sample_text.text = sb.toString()
      } else {
        sample_text.text = "errMsg: ${error.errMsg} \nerrInfo:${error.info}"
      }
    }

//    val jsonObject = com.rex.mecloud.JSONObject()
//    jsonObject.put("limit", 20)
//    jsonObject.put("skip", 5)
//    sample_text.text = "query..."
//    shareInstance().queryList("1.0/class/Hero", jsonObject, object : MeListCallback {
//      override fun done(objs: Array<JSONObject?>?, err: MeException?) {
//        if (err == null) {
//          val sb = StringBuffer()
//          objs?.forEach { sb.append(it!!.toString() + "\n") }
//          sample_text.text = sb.toString()
//        } else {
//          sample_text.text = "errMsg: ${err.errMsg} \nerrInfo:${err.info}"
//        }
//      }
//
//    })
  }

  private fun rgbaToBitmap(rgba: ByteArray, width: Int, height: Int): Bitmap {
    val bitout = IntArray(rgba.size / 4)
    for (i in bitout.indices) {
      val r = rgba[i * 4].toInt()
      val g = rgba[i * 4 + 1].toInt()
      val b = rgba[i * 4 + 2].toInt()
      bitout[i] = -0x1000000 + (r shl 16) + (g shl 8) + b
    }
    val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
    bitmap.setPixels(bitout, 0, width, 0, 0, width, height)
    return bitmap
  }
}


