#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <GL/glew.h>

#ifndef RENDERING_H
#define RENDERING_H

#define FPS 30
#define OUTPUT_FILE "output.mp4"
#define WIDTH 1920
#define HEIGHT 1080




void initializeFFmpeg();
void renderFrame(int n);

#endif
