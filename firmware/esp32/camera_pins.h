
camera_config_t getCameraConfig(String model) {
  camera_config_t config;
  /*#if defined(CAMERA_MODEL_WROVER_KIT)
  int PWDN_GPIO_NUM    = -1;
  int RESET_GPIO_NUM   = -1;
  int XCLK_GPIO_NUM    = 21;
  int SIOD_GPIO_NUM    = 26;
  int SIOC_GPIO_NUM    = 27;

  int Y9_GPIO_NUM      = 35;
  int Y8_GPIO_NUM      = 34;
  int Y7_GPIO_NUM      = 39;
  int Y6_GPIO_NUM      = 36;
  int Y5_GPIO_NUM      = 19;
  int Y4_GPIO_NUM      = 18;
  int Y3_GPIO_NUM      =  5;
  int Y2_GPIO_NUM      =  4;
  int VSYNC_GPIO_NUM   = 25;
  int HREF_GPIO_NUM    = 23;
  int PCLK_GPIO_NUM    = 22;

  #elif defined(CAMERA_MODEL_ESP_EYE)
  int PWDN_GPIO_NUM    = -1;
  int RESET_GPIO_NUM   = -1;
  int XCLK_GPIO_NUM    = 4;
  int SIOD_GPIO_NUM    = 18;
  int SIOC_GPIO_NUM    = 23;

  int Y9_GPIO_NUM      = 36;
  int Y8_GPIO_NUM      = 37;
  int Y7_GPIO_NUM      = 38;
  int Y6_GPIO_NUM      = 39;
  int Y5_GPIO_NUM      = 35;
  int Y4_GPIO_NUM      = 14;
  int Y3_GPIO_NUM      = 13;
  int Y2_GPIO_NUM      = 34;
  int VSYNC_GPIO_NUM   = 5;
  int HREF_GPIO_NUM    = 27;
  int PCLK_GPIO_NUM    = 25;

  #elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  int PWDN_GPIO_NUM     = -1;
  int RESET_GPIO_NUM    = 15;
  int XCLK_GPIO_NUM     = 27;
  int SIOD_GPIO_NUM     = 25;
  int SIOC_GPIO_NUM     = 23;

  int Y9_GPIO_NUM       = 19;
  int Y8_GPIO_NUM       = 36;
  int Y7_GPIO_NUM       = 18;
  int Y6_GPIO_NUM       = 39;
  int Y5_GPIO_NUM       =  5;
  int Y4_GPIO_NUM       = 34;
  int Y3_GPIO_NUM       = 35;
  int Y2_GPIO_NUM       = 32;
  int VSYNC_GPIO_NUM    = 22;
  int HREF_GPIO_NUM     = 26;
  int PCLK_GPIO_NUM     = 21;*/

  if (model == "M5STACK_WIDE") {
    config.pin_pwdn     = -1;
    config.pin_reset    = 15;
    config.pin_xclk     = 27;
    config.pin_sscb_sda     = 22;
    config.pin_sscb_scl     = 23;

    config.pin_d7       = 19;
    config.pin_d6       = 36;
    config.pin_d5       = 18;
    config.pin_d4       = 39;
    config.pin_d3       =  5;
    config.pin_d2       = 34;
    config.pin_d1       = 35;
    config.pin_d0       = 32;
    config.pin_vsync    = 25;
    config.pin_href     = 26;
    config.pin_pclk     = 21;
  }

  if (model == "AI_THINKER") {
    config.pin_pwdn     = 32;
    config.pin_reset    = -1;
    config.pin_xclk     =  0;
    config.pin_sscb_sda     = 26;
    config.pin_sscb_scl     = 27;

    config.pin_d7       = 35;
    config.pin_d6       = 34;
    config.pin_d5       = 39;
    config.pin_d4       = 36;
    config.pin_d3       = 21;
    config.pin_d2       = 19;
    config.pin_d1       = 18;
    config.pin_d0       =  5;
    config.pin_vsync    = 25;
    config.pin_href     = 23;
    config.pin_pclk     = 22;
  }


  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pixel_format = PIXFORMAT_JPEG;
  config.xclk_freq_hz = 20000000;
  //init with high specs to pre-allocate larger buffers
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_CIF; //_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_CIF; //_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  return config;
}
