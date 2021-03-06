#pragma once
#ifndef Mask_SKINNING_TECHNIQUE_H
#define	Mask_SKINNING_TECHNIQUE_H

#include "technique.h"
#include "ogldev_math_3d.h"
class MaskSkinningTechnique : public Technique {
public:

	static const uint MAX_POINT_LIGHTS = 2;
	static const uint MAX_SPOT_LIGHTS = 2;
	static const uint MAX_BONES = 254;

	MaskSkinningTechnique();

	virtual bool Init();

	void SetWVP(const Matrix4f& WVP);
	void SetColorTextureUnit(uint TextureUnit);
	void SetBoneTransform(uint Index, const Matrix4f& Transform);
	void SetDetailTextureUnit(uint TextureUnit);
	void SetMaskTextureUnit(uint TextureUnit);
	void SetUVSize(Vector2f& uvsize);
	void SetYOffset(float yoffset);
private:

	GLuint m_WVPLocation;
	GLuint m_YOffsetLocation;
	GLuint m_UVSizeLocation;
	GLuint m_colorTextureLocation;
	GLuint m_detailTextureLocation;
	GLuint m_maskTextureLocation;
	GLuint m_boneLocation[MAX_BONES];
};


#endif	/* Mask_SKINNING_TECHNIQUE_H */
