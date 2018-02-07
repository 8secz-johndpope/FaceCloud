#include "OSMesaContext.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/osmesa.h>
OSMesaContext ctx;
GLfloat *buffer = NULL;
bool MesaCreateContext(unsigned int Width, unsigned int Height)
{

	/* Create an RGBA-mode context */
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
	/* specify Z, stencil, accum sizes */

	ctx = OSMesaCreateContextExt(GL_RGBA, 16, 0, 0, NULL);
#else
	ctx = OSMesaCreateContext(GL_RGBA, NULL);
#endif
	if (!ctx) {
		printf("OSMesaCreateContext failed!\n");
		return 0;
	}

	/* Allocate the image buffer */
	buffer = (GLfloat *)malloc(Width * Height * 4 * sizeof(GLfloat));
	if (!buffer) {
		printf("Alloc image buffer failed!\n");
		return 0;
	}

	/* Bind the buffer to the context and make it current */
	if (!OSMesaMakeCurrent(ctx, buffer, GL_FLOAT, Width, Height)) {
		printf("OSMesaMakeCurrent failed!\n");
		return 0;
	}

	return true;
}
bool MesaDestroyContext()
{
	/* free the image buffer */
	free(buffer);

	/* destroy the context */
	if (!ctx) {
		OSMesaDestroyContext(ctx);
		return 0;
	}

}


GLuint MesaCreateShader()
{

	PFNGLCREATESHADERPROC proc = (PFNGLCREATESHADERPROC)OSMesaGetProcAddress("glCreateShader");

	if (proc != NULL)
	{
		GLuint rt = proc(GL_FRAGMENT_SHADER);
		if (rt == 0) {

			printf(("Error: Failed to create shader using mesaCreateShader\n"));
	
		}
		return rt;
	}
	return  -1;

}
GLuint MesaCreateProgram()
{

	PFNGLCREATEPROGRAMPROC proc = (PFNGLCREATEPROGRAMPROC)OSMesaGetProcAddress("glCreateProgram");

	if (proc != NULL)
	{
		GLuint rt = proc();
		if (rt == 0) {

			printf(("Error: Failed to create shader using mesaCreateShader\n"));

		}
		return rt;
	}
	return  -1;

}