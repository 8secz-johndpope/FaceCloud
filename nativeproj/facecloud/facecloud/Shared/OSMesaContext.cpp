#include "OSMesaContext.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/osmesa.h>
#include "Predefined.h"
using namespace std;


#define FORMAT_MSG_BUFFER_SIZE (204800)  

namespace OSMesa
{
	bool LogCreated = false;

	std::string format(const char *fmt, ...)
	{
		char szBuffer[FORMAT_MSG_BUFFER_SIZE + 1] = { 0 };
		va_list args;
		va_start(args, fmt);
		vsnprintf(szBuffer, FORMAT_MSG_BUFFER_SIZE, fmt, args);
		va_end(args);
		std::string strRet(szBuffer);
		return strRet;
	}
	void Log(std::string message)
	{
		return;
		printf(message.c_str());
		std::string file = string(RES_PATH) + string(LOGFILE);
		const char * filepath = file.c_str();
		FILE *pfile;

		if (!LogCreated) {
			pfile = fopen(filepath, "w");
			LogCreated = true;
		}
		else
			pfile = fopen(filepath, "a");

		if (pfile == NULL) {
			if (LogCreated)
				LogCreated = false;
			return;
		}
		else
		{
			fputs(message.c_str(), pfile);
			fclose(pfile);
			pfile = NULL;
		}

		if (pfile)
			fclose(pfile);
	}

	int getMilliCount() {
		timeb tb;
		ftime(&tb);
		int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
		return nCount;
	}

	int getMilliSpan(int nTimeStart) {
		int nSpan = getMilliCount() - nTimeStart;
		if (nSpan < 0)
			nSpan += 0x100000 * 1000;
		return nSpan;
	}
	//OSMesaContext ctx;
	GLfloat *buffer = NULL;
	bool MesaCreateContext(unsigned int Width, unsigned int Height)
	{

		//	/* Create an RGBA-mode context */
		//#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
		//	/* specify Z, stencil, accum sizes */
		//
		//	ctx = OSMesaCreateContextExt(GL_RGBA, 16, 0, 0, NULL);
		//#else
		//	ctx = OSMesaCreateContext(GL_RGBA, NULL);
		//#endif
		//	if (!ctx) {
		//		printf("OSMesaCreateContext failed!\n");
		//		return 0;
		//	}
		//
		//	/* Allocate the image buffer */
		//	buffer = (GLfloat *)malloc(Width * Height * 4 * sizeof(GLfloat));
		//	if (!buffer) {
		//		printf("Alloc image buffer failed!\n");
		//		return 0;
		//	}
		//
		//	/* Bind the buffer to the context and make it current */
		//	if (!OSMesaMakeCurrent(ctx, buffer, GL_FLOAT, Width, Height)) {
		//		printf("OSMesaMakeCurrent failed!\n");
		//		return 0;
		//	}

		return true;
	}
	bool MesaDestroyContext()
	{
		///* free the image buffer */
		//free(buffer);

		///* destroy the context */
		//if (!ctx) {
		//	OSMesaDestroyContext(ctx);
		//	return 0;
		//}

		return false;
	}


	GLuint MesaCreateShader()
	{

		/*PFNGLCREATESHADERPROC proc = (PFNGLCREATESHADERPROC)OSMesaGetProcAddress("glCreateShader");

		if (proc != NULL)
		{
			GLuint rt = proc(GL_FRAGMENT_SHADER);
			if (rt == 0) {

				printf(("Error: Failed to create shader using mesaCreateShader\n"));

			}
			return rt;
		}*/
		return  -1;

	}
	GLuint MesaCreateProgram()
	{

		/*PFNGLCREATEPROGRAMPROC proc = (PFNGLCREATEPROGRAMPROC)OSMesaGetProcAddress("glCreateProgram");

		if (proc != NULL)
		{
			GLuint rt = proc();
			if (rt == 0) {

				printf(("Error: Failed to create shader using mesaCreateShader\n"));

			}
			return rt;
		}*/
		return  -1;

	}
}