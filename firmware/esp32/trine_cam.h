#ifndef TRINE_CAM_H
#define TRINE_CAM_H

struct Image {
  uint8_t* data;
  size_t N;
  int width;
  int height;

  Image(uint8_t* d, size_t n, int w, int h) : data(d), N(n), width(w), height(h) {}

  String toString();
};

typedef void (*CameraCb)(Image img);

void init_camera_elegoo();
void init_camera(String camModel);
void capture_cam(CameraCb handlerCb);

#endif // TRINE_CAM_H