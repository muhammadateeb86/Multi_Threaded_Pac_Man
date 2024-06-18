// texture.c
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include <stdio.h>

GLuint loadTexture(const char *filename) {
    GLuint tex = SOIL_load_OGL_texture
(
    filename,
    SOIL_LOAD_AUTO,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA
);
    if (tex == 0) {
        printf("SOIL loading error: '%s'\n", SOIL_last_result());
    }
    return tex;
}

