#include "rendering.h"
#include <libavcodec/avcodec.h>
#include <stdlib.h>
#include <libavcodec/packet.h>
#include <png.h>
#include <stdio.h>

int frameCount = 0;
FILE *f;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;

/* A picture. */
    
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;

static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

static int save_png_to_file (const char *path, float *data)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  WIDTH,
                  HEIGHT,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, HEIGHT * sizeof (png_byte *));
    for (y = 0; y < HEIGHT; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * WIDTH * pixel_size);
        row_pointers[HEIGHT - 1 -y] = row;
        for (x = 0; x < WIDTH; x++) {
            *row++ = (int)(data[(y*WIDTH + x)*3]*255);
            *row++ = (int)(data[(y*WIDTH + x)*3 + 1]*255);
            *row++ = (int)(data[(y*WIDTH + x)*3 + 2]*255);
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < HEIGHT; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}


void renderFrame(int n)
{
    FILE *fp = fopen("image.ppm", "w");

    fprintf(fp, "P3\n%d %d\n255\n", WIDTH, HEIGHT);

    float *frame = malloc(sizeof(float)*HEIGHT*WIDTH*3);

    for(int i = 0; i < HEIGHT*WIDTH*3; i++)
	frame[i] = 0;

    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_FLOAT, frame);

/*    for(int i = 0; i < HEIGHT*WIDTH; i++)
    {
	if(frame[i*3] == 0 && frame[i*3+1] == 0 && frame[i*3+2] == 0)
	{
	    printf("%d\n", i);
	    exit(0);
	}
	printf("%f %f %f\n", frame[i*3], frame[i*3 + 1], frame[i*3 + 2]);
  
    } */
   
    char fileName[50];
    sprintf(fileName, "frames/frame%d.png", n);
    save_png_to_file(fileName, frame);

    /*
    for(int i = HEIGHT - 1; i >= 0; i--)
    {
	for(int j = 0; j < WIDTH; j++)
	{
	    //fprintf(fp, "%d %d %d ", (int)(frame[(i*WIDTH+j)*3]*255), 
	//	    (int)(frame[(i*WIDTH+j)*3 + 1]*255), (int)(frame[(i*WIDTH+j)*3 + 2]*255));
	}

	fputc('\n', fp);
    } */
}
