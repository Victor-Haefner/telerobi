#include "esp_camera.h"
#include "esp_system.h"
#include "Arduino.h"
#include "base64.h"
#include "trine_cam.h"
#include "camera_pins.h"

bool camInitiated = false;

void init_camera(String camModel) {
  camera_config_t config = getCameraConfig(camModel);

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_CIF);//_SVGA); // 800x600
  s->set_vflip(s, 0);
  s->set_hmirror(s, 0);
  camInitiated = true;
}

void capture_cam(CameraCb handlerCb) {
    if (!camInitiated) return;
      
    size_t _jpg_buf_len = 0;
    uint8_t* _jpg_buf = NULL;

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) { Serial.println("Camera capture failed"); return; }

    int width = fb->width;
    int height = fb->height;

    _jpg_buf = fb->buf;
    _jpg_buf_len = fb->len;
    
    //Serial.println(String(width) + " " + String(height) + " " + String(_jpg_buf_len) + "/" + String(width*height));

    handlerCb(Image(_jpg_buf, _jpg_buf_len, width, height));

    if (fb) {
        esp_camera_fb_return(fb);
        fb = NULL;
        _jpg_buf = NULL;
    } else if (_jpg_buf) {
        free(_jpg_buf);
        _jpg_buf = NULL;
    }
}

String Image::toString() {
  return base64::encode(data, N);
}
