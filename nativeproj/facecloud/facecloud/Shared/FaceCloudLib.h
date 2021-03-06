#pragma once
#ifndef FACE_CLOUD_LIB_H
#define	FACE_CLOUD_LIB_H
#include <stdio.h>
#include <string>
#include <map>

#include "ogldev_engine_common.h"
#include "ogldev_app.h"
#include "ogldev_camera.h"
#include "ogldev_util.h"
#include "ogldev_pipeline.h"
#include "ogldev_camera.h"
#include "texture.h"
#include "CommonTechnique.h"
#include "SkinningTechnique.h"
#include "UnlitSkinningTechnique.h"
#include "MaskSkinningTechnique.h"
#include "RenderTextureTechnique.h"
#include "ogldev_glut_backend.h"
#include "ogldev_skinned_mesh.h"
#include "BoneUtility.h"
#include <thread>
#include <queue>

using namespace std;

struct CalculateData
{
	string modelID;
	string photoPath;
	string jsonFace;
	string photoPathOut;
	string jsonModelOut;

	bool finished;
	string success;
};

class FaceCloudLib{

public:
	FaceCloudLib(); 
	~FaceCloudLib();
	bool Init(bool offscreen = true);
	bool InitReal(bool offscreen);
	bool InitMat();
	bool Finalize();


	string Calculate(string modelID, string photoPath, string jsonFace, string& photoPathOut, string& jsonModelOut);
	string CalculateReal(string modelID, string photoPath, string jsonFace, string& photoPathOut, string& jsonModelOut);

	string DetectSkinStatus(string photoPath, string jsonFace, string &jsonModelOut);


	void CalculateBone(string modelID, JsonFaceInfo jsonfaceinfo, string& photoPathOut, string& jsonModelOut, Vector3f& centerpos, Vector2f& uvsize, float& yOffset,bool isman);
	bool DrawOnce(string modelID, Vector3f& center, Vector2f& uvsize);

	bool DrawMaskOnce(string modelID, Vector3f& center, Vector2f& uvsize);

	void CombineTexture(GLuint FaceTexture, cv::Mat bgColor, Texture* pMask, string& photoPathOut, bool frontTrans = true);
	void CombineTextureMaskBlend(cv::Mat bgColor, Texture* pforeColor,  string& photoPathOut);
	cv::Mat GLTextureToMat(GLuint texture, unsigned char*& outimagptr);
	Camera * GetCamera() {
		return m_pGameCamera;
	};


	void SaveFile(string& s, string& path);
	void SaveJsonFile(Json::Value jvalue, string& path);
	Json::Value LoadJsonValueFromFile(string filepath);
	string LoadJsonStringFromFile(string filepath);

	void RenderToScreen()
	{
		EndRenderTexture();
	}
	void DisplayGrid();
	PersProjInfo GetPersProjInfo()
	{
		return m_persProjInfo;
	}
	OrthoProjInfo GetOrthoProjInfo()
	{
		return m_orthoProjInfo;
	}

	bool m_bRenderToTexture = true;
	bool m_bHasCalculate = false;


	Vector3f m_Lastcenter;
	Vector2f m_Lastuvsize;
	float m_Lastyoffset = 0;
private:


	Camera * m_pGameCamera;
	PersProjInfo m_persProjInfo;
	OrthoProjInfo m_orthoProjInfo;
	UnlitSkinningTechnique * m_pSkinningRenderer;

	MaskSkinningTechnique * m_pSkinningMaskRenderer;

	CommonTechnique * m_pCommonRenderer;


	map<string, SkinnedMesh*> m_MeshMap;
	map<string, Texture*> m_ColorTextureMap;
	Texture* m_pMaskTexture;
	Texture* m_pCurrentSkinTexture;

	jsonRoles m_JsonRoles;
	BoneUtility m_BoneUtility;

	GLuint m_RenderTexture; 
	GLuint m_FramebufferName;

	int m_Width;
	int m_Height;


	cv::Mat _Facemap_left_Mask;
	cv::Mat _Facemap_right_Mask;
	cv::Mat _BG_left_Mask;
	cv::Mat _BG_right_Mask;
	cv::Mat _sampling_LF_mask;
	cv::Mat _sampling_RT_mask;
	cv::Mat _SampleButtom_LF_mask;
	cv::Mat _SampleButtom_RT_mask;
	cv::Vec3s m_targetcolor;
	cv::Vec3s m_basecolor;


	bool InitCamera();
	bool InitMesh();
	bool InitJson();


	bool CreateRenderTarget();
	bool BeginRenterTexture();
	void EndRenderTexture();
	void SaveTextureToFile(cv::Mat imag, int format, string path, bool flip = false);

	cv::Mat AutoMask(cv::Mat srcMask,cv::Point& center);

	void getMean(cv::Mat img, cv::Vec3s& left, cv::Vec3s& right);

public:
      std::thread m_OpenGLThread;
	  bool m_bOffscreen;
	  bool m_Running;

	  queue<CalculateData*> m_RunningQueue;

	  JsonColorRef m_ColorRef;
};


#endif	/* FACE_CLOUD_LIB_H */
