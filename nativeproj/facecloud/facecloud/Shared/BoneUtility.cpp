#include "BoneUtility.h"
#include <fstream>
#include <json/json.h>

#include <json/json.h>
#include <fstream>
#include <sstream>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/core/core.hpp> 
#include "ImageOptimizedUtility.h"

void JsonModelFormat::LoadFromFile(string filename)
{

	Json::CharReaderBuilder rbuilder;
	rbuilder["collectComments"] = false;
	std::string errs;
	std::ifstream ifs;
	ifs.open(filename);
	bool ok = Json::parseFromStream(rbuilder, ifs, &root, &errs);
	ifs.close();

	Load(root);
}
void JsonModelFormat::Load(Json::Value root)
{
	Json::Value mesh = root["model"]["meshes"][0];

	Json::Value verts = mesh["verts"];

	Json::Value vertElement = mesh["vertElement"];
	Json::Value vertIndices = vertElement["vertIndices"];
	Json::Value normals = vertElement["normals"];
	Json::Value uvs = vertElement["uvs"];
	Json::Value uv1 = uvs[0];
	Json::Value uv2 = uvs[1];

	Json::Value face = mesh["face"];
	Json::Value vertElementIndices = face["vertElementIndices"];

	Json::Value skin = mesh["skin"];
	Json::Value skinBones = skin["skinBones"];
	Json::Value skinWeights = skin["skinWeights"];



	Json::Value jnodes = root["hierarchy"]["nodes"];
	int nodescount = jnodes.size();

	for (int i = 0 ; i< nodescount;i++)
	{
		Json::Value jnode = jnodes[i];
		JsonModelFormat::node node;
		node.name = jnode["name"].asString();
		node.parent = jnode["parent"].asInt();
		/*node.pos = Vector3f(jnode["pos"][0].asFloat(), jnode["pos"][1].asFloat(), jnode["pos"][2].asFloat());
		node.scl = Vector3f(jnode["scl"][0].asFloat(), jnode["scl"][1].asFloat(), jnode["scl"][2].asFloat());
		node.rot = Vector4f(jnode["rot"][0].asFloat(), jnode["rot"][1].asFloat(), jnode["rot"][2].asFloat(), jnode["rot"][3].asFloat());*/

		node.pos = Vector3f(0, 0, 0);
		node.scl = Vector3f(1, 1, 1);
		node.rot = Vector4f(0, 0 ,0, 1);
		nodemap[node.name] = node;
	}

}
string JsonModelFormat::ToString()
{
	//All Info
	/*Json::Value jnodes = root["hierarchy"]["nodes"];
	for (int i = 0; i < jnodes.size(); i++)
	{
		jnodes[i]["pos"][0] = Json::Value(nodemap[jnodes[i]["name"].asString()].pos.x);
		jnodes[i]["pos"][1] = Json::Value(nodemap[jnodes[i]["name"].asString()].pos.y);
		jnodes[i]["pos"][2] = Json::Value(nodemap[jnodes[i]["name"].asString()].pos.z);
	}*/

	//Tiny Info
	Json::Value jnodes = root["hierarchy"]["nodes"];
	Json::Value oroot;
	Json::Value hierarchy;
	Json::Value nodes;
	for (int i = 0; i < jnodes.size(); i++)
	{
		jnodes[i]["pos"][0] = Json::Value(nodemap[jnodes[i]["name"].asString()].pos.x);
		jnodes[i]["pos"][1] = Json::Value(nodemap[jnodes[i]["name"].asString()].pos.y);
		jnodes[i]["pos"][2] = Json::Value(nodemap[jnodes[i]["name"].asString()].pos.z);

		Json::Value jnewnode;
		jnewnode["name"] = jnodes[i]["name"];
		jnewnode["pos"][0] = jnodes[i]["pos"][0];
		jnewnode["pos"][1] = jnodes[i]["pos"][1];
		jnewnode["pos"][2] = jnodes[i]["pos"][2];

		nodes.append(jnewnode);
	}
	hierarchy["nodes"] = nodes;
	oroot["hierarchy"] = hierarchy;



	//Json Value to String
	Json::StreamWriterBuilder  builder;
	builder.settings_["commentStyle"] = "None";
	//builder.settings_["indentation"] = "All";
	std::string s = Json::writeString(builder, nodes);

	return s;
}
void JsonModelFormat::Save(string filename)
{
	string s = ToString();

	ofstream write;
	write.open(filename.c_str(), ios::out | ios::binary);
	write.write(s.c_str(), s.length());
	write.close();
}
JsonModelFormat::~JsonModelFormat()
{
}




void BoneUtility::Init()
{
	hasMoveBones = false;
	pairs.LoadFromFile("data/face/kp.json");
	jsonModelFormat.LoadFromFile("data/face/women_head_fix.JD");
}

int BoneUtility::ReadJsonFromFile(const char* filename)
{

	Json::Value root; // Json::Value��һ�ֺ���Ҫ�����ͣ����Դ����������͡���int, string, object, array         


					  // Here, using a specialized Builder, we discard comments and
					  // record errors as we parse.
	Json::CharReaderBuilder rbuilder;
	rbuilder["collectComments"] = false;
	std::string errs;



	std::ifstream ifs;
	ifs.open(filename);
	bool ok = Json::parseFromStream(rbuilder, ifs, &root, &errs);
	ifs.close();


	Json::StreamWriterBuilder wbuilder;
	wbuilder["indentation"] = "\t";
	std::string document = Json::writeString(wbuilder, root);



	float face_rectangle = root["face_rectangle"]["top"].asFloat();





	return 0;
}
Texture* BoneUtility::CalculateSkin(GLuint texture,bool isman, JsonRole bonedef, JsonFaceInfo& faceinfo)
{
	GLint wtex, htex, comp, rs, gs, bs, as;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &wtex);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &htex);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &comp);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &rs);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &gs);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &bs);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &as);


	long size = wtex * htex * 4;
	unsigned char* output_image = new unsigned char[size];


	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, output_image);

	GLenum error = glGetError();
	const GLubyte * eb = gluErrorString(error);
	string errorstring((char*)eb);
	cv::Mat srcimg = cv::Mat(wtex, htex, CV_8UC4, (unsigned*)output_image);

	cv::Mat srcimg32;
	srcimg.convertTo(srcimg32, CV_32FC4);



	ImageOptimizedUtility iou;

	//do photo pre process
	cv::Mat img = iou.FacePhotoProcess(faceinfo, bonedef, srcimg32);


	iou.SaveTextureToFile(img, GL_RGBA, "data/export/affinetest.jpg");

	Mat rgbimg;
	cv::cvtColor(img, rgbimg, CV_RGBA2RGB);

	Mat rtmat;

	Vector3f ref_color;

	if (isman)
	{
		ref_color = Vector3f(171, 133, 97);
	}
	else {
		ref_color = Vector3f(170, 140, 106);
	}



	Vector2f leftpoint(350, 340);
	Vector2f rightpoint(475, 420);

	Vector3f rgb = iou.UpdateRefSkin(rgbimg, ref_color, 1.0f, rtmat, leftpoint, rightpoint);
	

	Texture *ptexture = new Texture();
	ptexture->FromCVMat(GL_TEXTURE_2D,rtmat);
	
	SAFE_DELETE(output_image);

	return ptexture;
}

void BoneUtility::CalculateFaceBone(SkinnedMesh* pmesh, JsonRole bonedef, JsonFaceInfo faceinfo,string& outOffsetJson,Vector3f& centerpos,Vector2f& uvsize,float& yoffset)
{

	Matrix4f tooth_MID = pmesh->GetBoneNode("face_mouthLip_up_joint2");
	Matrix4f toothup_Lf = pmesh->GetBoneNode("face_mouthLip_Lf_joint1");
	Matrix4f toothdown_Rt = pmesh->GetBoneNode("face_mouthLip_Rt_joint1");

	Vector3f tooth_MID_pos1 = tooth_MID.ExtractTranslation();
	Vector3f toothup_Lf_pos1 = toothup_Lf.ExtractTranslation();
	Vector3f toothdown_Rt_pos1 = toothdown_Rt.ExtractTranslation();

	//��ͷƫ��

	Matrix4f forehead_RT = pmesh->GetBoneNode("face_temple_Rt_joint1");
	Matrix4f forehead_LF = pmesh->GetBoneNode("face_temple_Lf_joint1");


	Vector3f forhead_RT_pos1 = forehead_RT.ExtractTranslation();
	Vector3f forhead_LF_pos1 = forehead_LF.ExtractTranslation();

	//üë����

	Matrix4f brow_LF02 = pmesh->GetBoneNode("face_brow_Lf_joint2");
	Matrix4f brow_LF03 = pmesh->GetBoneNode("face_brow_Lf_joint3");

	Vector3f brow_LF02_pos1 = brow_LF02.ExtractTranslation();
	Vector3f brow_LF03_pos1 = brow_LF03.ExtractTranslation();

	//��������
	Matrix4f nose_tr = pmesh->GetBoneNode("face_nose_joint2");
	Vector3f nose_tr_pos1 = nose_tr.ExtractTranslation();


	//�������

	Vector3f mouth_tr_pos1 = pmesh->GetBoneNode("face_mouthLip_up_joint0").ExtractTranslation();

	//�۾�����
	float righteye_conner_pos1 = pmesh->GetBoneNode("face_eyeLids_Rt_joint1").ExtractTranslation().x;
	float righteye_conner_pos2 = pmesh->GetBoneNode("face_eyeLids_Rt_joint2").ExtractTranslation().x;
	float Lefteye_conner_pos1 = pmesh->GetBoneNode("face_eyeLids_Lf_joint1").ExtractTranslation().x;
	float Lefteye_conner_pos2 = pmesh->GetBoneNode("face_eyeLids_Lf_joint2").ExtractTranslation().x;

	float Eye_Rt_dis = righteye_conner_pos1 - righteye_conner_pos2;
	float Eye_LF_dis = Lefteye_conner_pos1 - Lefteye_conner_pos2;


	Vector3f fixmouthlipdistance01 = pmesh->GetBoneNode("face_mouthLip_Lf_joint5").ExtractTranslation() - pmesh->GetBoneNode("face_mouthLip_Lf_joint7").ExtractTranslation();
	Vector3f fixmouthlipdistance02 = pmesh->GetBoneNode("face_mouthLip_Rt_joint5").ExtractTranslation() - pmesh->GetBoneNode("face_mouthLip_Rt_joint7").ExtractTranslation();
	Vector3f fixmouthlipdistance03 = pmesh->GetBoneNode("face_mouthLip_dn_joint2").ExtractTranslation() - pmesh->GetBoneNode("face_mouthLip_up_joint2").ExtractTranslation();


	//  Debug.Log("Eye_Rt_dis" + Eye_Rt_dis+ "Eye_LF_dis"+ Eye_LF_dis);

	Vector3f fixnose_lf1 = pmesh->GetBoneNode("face_nosewing_Lf_joint1").ExtractTranslation();
	Vector3f fixnose_RT1 = pmesh->GetBoneNode("face_nosewing_Rt_joint1").ExtractTranslation();


	Matrix4f  face_eyeLids_Lf_joint1  = pmesh->GetBoneNode("face_eyeLids_Lf_joint1");
	Matrix4f face_eyeBall_Lf_joint1 = pmesh->GetBoneNode("face_eyeBall_Lf_joint1");

	Vector3f leftpos = pmesh->GetBoneNode("face_eyeLids_Lf_joint1").ExtractTranslation();
	Vector3f rightpos = pmesh->GetBoneNode("face_eyeLids_Rt_joint1").ExtractTranslation();
	Vector3f headCenter = (leftpos + rightpos) * 0.5f;


	rtjson.clear();
	for (int i = 0; i < pairs.pairs.size(); i++)
	{
		KP kp = pairs.pairs[i];
		MoveBone(pmesh, kp.bonename, faceinfo, kp.facekeypointname, bonedef, kp.offsetname, headCenter);
	}
	
	//MoveUV(pmesh, bonedef);

	outOffsetJson = jsonModelFormat.ToString();
	yoffset = bonedef.offset_y / 100;
	centerpos = headCenter;
	uvsize = Vector2f(bonedef.uvsize, bonedef.uvsize);

	/*string s = "data/export/bonertdb.json";
	m_JsonDB.Save(s);*/



	/*


	Vector3f brow_LF02_pos2 = brow_LF02.localPosition;
	Vector3f brow_LF03_pos2 = brow_LF03.localPosition;

	float  brow_LF02_offset = brow_LF02_pos2.x - brow_LF02_pos1.x;
	float  brow_LF03_offset = brow_LF02_pos2.x - brow_LF02_pos1.x;


	_bones["face_brow_Lf_joint2"].localPosition += new Vector3f(0, 0, brow_LF02_offset / 2);
	_bones["face_brow_Lf_joint3"].localPosition += new Vector3f(0, 0, brow_LF03_offset / 2);
	_bones["face_brow_Rt_joint2"].localPosition += new Vector3f(0, 0, brow_LF02_offset / 2);
	_bones["face_brow_Rt_joint3"].localPosition += new Vector3f(0, 0, brow_LF03_offset / 2);


	Vector3f nose_tr_pos2 = nose_tr.localPosition;
	float nose_offset = nose_tr_pos2.y - nose_tr_pos1.y;
	//  _bones["face_nose_joint0"].localPosition += new Vector3f(0, nose_offset/3f, 0);

	Vector3f mouth_tr_pos2 = mouth_tr.localPosition;
	float mouth_offset = mouth_tr_pos2.y - mouth_tr_pos1.y;
	//  _bones["face_mouthLip_joint0"].localPosition -= new Vector3f(0, mouth_offset/3f, 0);



	// Debug.Log("Eye_Rt_dis2" + Eye_Rt_dis2 + "Eye_LF_dis2" + Eye_LF_dis2);
	// float eyescal = ((Eye_Rt_dis2 / Eye_Rt_dis) + (Eye_LF_dis2 / Eye_LF_dis)) / 2 ;
	// Debug.Log("eyescal" + eyescal);



	Vector3f tooth_MID_pos2 = tooth_MID.localPosition;
	Vector3f toothup_Lf_pos2 = toothup_Lf.localPosition;
	Vector3f toothdown_Rt_pos2 = toothdown_Rt.localPosition;

	float toothpos_y = tooth_MID_pos2.y - tooth_MID_pos1.y;
	float toothLf_x = toothup_Lf_pos2.x - toothup_Lf_pos1.x;
	float toothRT_z = toothdown_Rt_pos2.x - toothdown_Rt_pos2.x;

	_bones["face_tooth_down_joint1"].localPosition += new Vector3f(0, toothpos_y, 0);
	_bones["face_tooth_up_joint3"].localPosition += new Vector3f(-toothpos_y, 0, 0);
	_bones["face_tooth_down_joint3"].localPosition += new Vector3f(toothLf_x, toothpos_y, 0);
	_bones["face_tooth_up_joint1"].localPosition += new Vector3f(-toothpos_y, 0, toothLf_x);
	_bones["face_tooth_down_joint2"].localPosition += new Vector3f(toothLf_x, toothpos_y, 0);
	_bones["face_tooth_up_joint2"].localPosition += new Vector3f(-toothpos_y, 0, toothLf_x);

	//��������

	Vector3f forhead_RT_pos2 = forehead_RT.localPosition;
	Vector3f forhead_LF_pos2 = forehead_LF.localPosition;

	Vector3f forhead_RT_offset = forhead_RT_pos2 - forhead_RT_pos1;
	Vector3f forhead_LF_offset = forhead_LF_pos2 - forhead_LF_pos1;


	_bones["face_temple_Lf_joint2"].localPosition -= forhead_LF_offset;
	_bones["face_temple_Lf_joint3"].localPosition -= forhead_LF_offset;
	_bones["face_forehead_Lf_joint1"].localPosition -= forhead_LF_offset;
	_bones["face_forehead_Lf_joint4"].localPosition -= forhead_LF_offset;
	_bones["face_forehead_Lf_joint2"].localPosition -= forhead_LF_offset;


	_bones["face_temple_Rt_joint2"].localPosition += forhead_RT_offset;
	_bones["face_temple_Rt_joint3"].localPosition += forhead_RT_offset;
	_bones["face_forehead_Rt_joint1"].localPosition += forhead_RT_offset;
	_bones["face_forehead_Rt_joint2"].localPosition += forhead_RT_offset;
	_bones["face_forehead_Lf_joint3"].localPosition += forhead_RT_offset;


	float face_calvaria_joint1updown = forhead_RT_pos2.z - forhead_RT_pos1.z;


	if (face_calvaria_joint1updown < 0) {

		_bones["face_calvaria_joint1"].localPosition += new Vector3f(face_calvaria_joint1updown, 0, 0);


		_bones["face_temple_Lf_joint3"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);
		_bones["face_temple_Rt_joint3"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);

		_bones["face_forehead_joint1"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);
		_bones["face_forehead_Lf_joint3"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);
		_bones["face_forehead_Lf_joint2"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);

		_bones["face_forehead_Lf_joint6"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);
		_bones["face_forehead_joint2"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);
		_bones["face_forehead_Lf_joint5"].localPosition += new Vector3f(0, face_calvaria_joint1updown, 0);

	}




	fixfacedistans_x(_bones["face_temple_Lf_joint1"], _bones["face_temple_Rt_joint1"], 0);
	fixfacedistans_x(_bones["face_temple_Lf_joint2"], _bones["face_temple_Rt_joint2"], 0);
	fixfacedistans_y(_bones["face_temple_Lf_joint1"], _bones["face_temple_Lf_joint1"]);
	fixfacedistans_y(_bones["face_temple_Lf_joint2"], _bones["face_temple_Rt_joint2"]);
	fixfacedistans_z(_bones["face_temple_Lf_joint1"], _bones["face_temple_Lf_joint1"], 0);
	fixfacedistans_z(_bones["face_temple_Lf_joint2"], _bones["face_temple_Rt_joint2"], 0);

	fixfacedistans_x(_bones["face_forehead_Lf_joint1"], _bones["face_forehead_Rt_joint1"], 0);
	fixfacedistans_x(_bones["face_forehead_Lf_joint2"], _bones["face_forehead_Lf_joint3"], 0);
	fixfacedistans_x(_bones["face_forehead_Lf_joint4"], _bones["face_forehead_Rt_joint2"], 0);
	fixfacedistans_x(_bones["face_forehead_Lf_joint5"], _bones["face_forehead_Lf_joint6"], 0);

	fixfacedistans_y(_bones["face_forehead_Lf_joint1"], _bones["face_forehead_Rt_joint1"]);
	fixfacedistans_y(_bones["face_forehead_Lf_joint2"], _bones["face_forehead_Lf_joint3"]);
	fixfacedistans_y(_bones["face_forehead_Lf_joint4"], _bones["face_forehead_Rt_joint2"]);
	fixfacedistans_y(_bones["face_forehead_Lf_joint5"], _bones["face_forehead_Lf_joint6"]);

	fixfacedistans_z(_bones["face_forehead_Lf_joint1"], _bones["face_forehead_Rt_joint1"], 0);
	fixfacedistans_z(_bones["face_forehead_Lf_joint2"], _bones["face_forehead_Lf_joint3"], 0);
	fixfacedistans_z(_bones["face_forehead_Lf_joint4"], _bones["face_forehead_Rt_joint2"], 0);
	fixfacedistans_z(_bones["face_forehead_Lf_joint5"], _bones["face_forehead_Lf_joint6"], 0);

	*/


}
void BoneUtility::MoveUV(SkinnedMesh* pmesh, JsonRole bonedef)
{

	uint NumVertices = 0;
	int len = 0;
	int startpos;
	int meshid = 0;
	for (uint i = 0; i < pmesh->m_Entries.size(); i++) {

		if (pmesh->m_Entries[i].NumBones > 80)
		{
			meshid = i;
			len = pmesh->m_Entries[i].BaseVertex;
			startpos = NumVertices;
		}


		NumVertices += pmesh->m_pScene->mMeshes[i]->mNumVertices;
	}

	vector<Vector2f> uvs2;
	uvs2.reserve(len);
	for (int i = 0 ; i< len;i++)
	{
		float uvsize = bonedef.uvsize;
		float offset_y = bonedef.offset_y / 100;

		Vector3f vertex = pmesh->TotalPositions[startpos + i];

		uint VertexID = startpos + i;
		Matrix4f BoneTransform;
		BoneTransform.SetZero();
		SkinnedMesh::VertexBoneData vertexboneinfo = pmesh->TotalBones[VertexID];
		for (int i = 0; i < sizeof(*(vertexboneinfo.IDs)); i++ )
		{
			int boneid = vertexboneinfo.IDs[i];
			string bonename = pmesh->m_BoneInfo[boneid].Name;

			if (pmesh->m_BoneGlobalTrasMap.find(bonename) != pmesh->m_BoneGlobalTrasMap.end())
			{
				Matrix4f globaltran = pmesh->m_BoneGlobalTrasMap[bonename];
				Matrix4f offset = pmesh->m_BoneInfo[boneid].BoneOffset;
				//BoneTransform = BoneTransform + globaltran * offset * vertexboneinfo.Weights[i];

			}
		}

		Vector4f vertexafter = BoneTransform * Vector4f( vertex.x, vertex.y, vertex.z,1.0);

		Vector2f uv(vertex.x / uvsize + 0.5f, -vertex.y / uvsize - offset_y);
		uvs2.push_back(uv);
	}

	vector<Vector2f> newuv2 = pmesh->TotalTexCoords2;

	for (int i = 0; i < len; i++)
	{
		newuv2[startpos + i] = uvs2[i];
	}

	//pmesh->RefreshUV2(newuv2);
}


void BoneUtility::ResetBone()
{

}
//�沿��������
void BoneUtility::MoveBonePYR(SkinnedMesh* pmesh,string bonename, JsonFaceInfo faceinfo, string facekeypoint, JsonRole bonedef, string boneoffsetname, Vector3f headCenter,float offsetrate)
{
	SkinnedMesh::BoneInfo boneinfo = pmesh->GetBoneInfo(bonename);



	Matrix4f finaltransold = boneinfo.BoneOffset;
	Vector3f trspos, trsscl;
	Matrix4f trsrot;
	finaltransold.MatrixDecompose(trspos, trsscl, trsrot);

	float zepos = bonedef.face_zero_pointy;

	// Vector3f  planpos = _faceplan.Matrix4f.position; 
	//�ض������

	Vector3f planpos = headCenter;


	float sizeuv = bonedef.uvsize;// 30.0f;
	float scale_1024_to_model = sizeuv / 1024;
	float zero_px = planpos.x - sizeuv / 2;
	float zero_py = planpos.y - sizeuv / 2;


	Vector3f zero(zero_px, zero_py, trspos.z);
	//Debug.Log(pmesh->GetBoneInfo("chin_joint1"].position);

	Vector3f OriPosition = trspos;

	float yaw_angle = (float)(faceinfo.yaw_angle);          //ҡͷ
	float roll_angle = (float)(faceinfo.roll_angle);        //ƽ����ת
	float pitch_angle = (float)(faceinfo.pitch_angle);   //̧ͷ


	float x0 = OriPosition.x;
	float y0 = OriPosition.y;
	float z0 = OriPosition.z;

	float sinx = sin(yaw_angle); float cosx = cos(yaw_angle);
	float siny = sin(pitch_angle); float cosy = cos(pitch_angle);
	float sinz = sin(roll_angle); float cosz = cos(roll_angle);


	float x1 = x0 * (cosy * cosz - sinx * siny * sinz) - y0 * cosx * sinz + z0 * (siny * cosz + sinx * cosy * sinz);
	float y1 = x0 * (cosy * sinz + sinx * siny * cosz) + y0 * cosx * cosz + z0 * (siny * sinz - sinx * cosy * cosz);
	float z1 = x0 * (-cosx * siny) + y0 * sinx + z0 * cosx * cosy;

	Vector3f RotatOriglepostion(x1, y1, z1);


	//  Debug.LogError(OriPosition +"  "+ RotatOriglepostion);


	Vector2f face2dpos = faceinfo.landmarkdata[facekeypoint];
	Vector3f Newpositon = zero + Vector3f(face2dpos.x * scale_1024_to_model, face2dpos.y * scale_1024_to_model, 0);

	//   Debug.LogError(Newpositon);

	//float xdis = Newpositon.x - RotatOriglepostion.x;
	//float ydis = Newpositon.y - RotatOriglepostion.y;
	//float zdis = Newpositon.z - RotatOriglepostion.z;


	float xdis = Newpositon.x - OriPosition.x;
	float ydis = Newpositon.y - OriPosition.y;
	float zdis = Newpositon.z - OriPosition.z;



	float sinx2 = sin(-yaw_angle); float cosx2 = cos(-yaw_angle);
	float siny2 = sin(-pitch_angle); float cosy2 = cos(-pitch_angle);
	float sinz2 = sin(-roll_angle); float cosz2 =cos(-roll_angle);


	float xf = xdis * (cosy2 * cosz2 - sinx2 * siny2 * sinz2) - ydis * cosx2 * sinz2 + zdis * (siny2 * cosz2 + sinx2 * cosy2 * sinz2);
	float yf = xdis * (cosy2 * sinz2 + sinx2 * siny2 * cosz2) + ydis * cosx2 * cosz2 + zdis * (siny2 * sinz2 - sinx2 * cosy2 * cosz2);
	float zf = xdis * (-cosx2 * siny2) + ydis * sinx2 + zdis * cosx2 * cosy2;



	Vector3f faceOffset(xf, yf, zf);

	
	Vector3f finalpos = zero + Vector3f(face2dpos.x * scale_1024_to_model, face2dpos.y * scale_1024_to_model,0);


	
}


Matrix4f BoneUtility::GetLocalMatrixFromGlobal(SkinnedMesh* pmesh,string bonename, Matrix4f globalmat)
{
	Matrix4f parentMat = pmesh->GetNodeGlobalTransformation(pmesh->m_BoneNodeMap[bonename]->mParent);

	Matrix4f parentinv = parentMat;
	parentinv.Inverse();

	Matrix4f x = parentMat * globalmat;
	return x;

}

//�沿��������
void BoneUtility::MoveBone(SkinnedMesh* pmesh, string bonename, JsonFaceInfo faceinfo, string facekeypoint, JsonRole bonedef, string boneoffsetname, Vector3f headCenter, float offsetrate)
{
	Matrix4f transformtochange;
	Matrix4f currentlocalMat;

	transformtochange = pmesh->GetNodeGlobalTransformation(pmesh->m_BoneNodeMap[bonename]);

	aiMatrix4x4 aicurrentMat = pmesh->m_BoneNodeMap[bonename]->mTransformation;
	currentlocalMat = Matrix4f(aicurrentMat);



	if (bonename == "face_chin_joint3")
	{
		printf("");
	}


	Vector3f trspos, trsscl;
	Matrix4f trsrot;
	transformtochange.MatrixDecompose(trspos, trsscl, trsrot);





	float zepos = bonedef.face_zero_pointy;

	float sizeuv = bonedef.uvsize;
	float scale_1024_to_model = sizeuv / 1024;

	float zero_px = headCenter.x + sizeuv / 2;//open �������� unity �������� 
	float zero_py = headCenter.y - sizeuv / 2;

	Vector3f zero(zero_px, zero_py, trspos.z);
	trspos = zero + Vector3f(-faceinfo.landmarkdata[facekeypoint].x, faceinfo.landmarkdata[facekeypoint].y, 0) *scale_1024_to_model;


	//Vector3f offset = bonedef.offsets_map[boneoffsetname] * offsetrate;
	//trspos += offset;

	Matrix4f parentMat = pmesh->GetNodeGlobalTransformation(pmesh->m_BoneNodeMap[bonename]->mParent);


	Matrix4f parentinv = parentMat;
	parentinv.Inverse();

	Matrix4f prelocal = parentinv * transformtochange;

	Pipeline ps;
	ps.Scale(trsscl);


	Pipeline pt;
	pt.WorldPos(trspos);
	Matrix4f mt = pt.GetWorldTrans();


	Matrix4f mat = mt  * trsrot * ps.GetWorldTrans();


	/*uint index = pmesh->m_BoneMapping[bonename];
	pmesh->m_BoneInfo[index].BoneOffset = mat;*/


	Matrix4f identity;
	identity.InitIdentity();

	/*uint index = pmesh->m_BoneMapping[bonename];
	pmesh->m_BoneInfo[index].BoneOffset = identity;*/


	Vector3f cpos, cscale;
	Matrix4f crot;
	currentlocalMat.MatrixDecompose(cpos, cscale, crot);

	Pipeline cp;
	cp.Scale(cscale);
	Matrix4f crs = crot * cp.GetWorldTrans();
	Matrix4f crsinv = crs;
	crsinv.Inverse();



	Matrix4f x = parentinv * mat;

	Matrix4f tx = x * crsinv;



	Vector3f offset = bonedef.offsets_map[boneoffsetname];
	tx.m[0][3] += -offset.x;
	tx.m[1][3] += offset.y;
	tx.m[2][3] += offset.z;

	/*Matrix4f txoffset;
	txoffset.InitTranslationTransform(offset.x,-offset.y,-offset.z);*/
	Vector3f localoffset = tx.ExtractTranslation();
	Matrix4f final = tx * crs;



	/*char str[255];
	vector<string> keys;
	vector<string> values;
	keys.push_back("name");
	values.push_back(bonename);
	keys.push_back("afteroffsetlocal");
	sprintf(str, "afteroffsetlocal: x : %f y: %f z: %f", localoffset.x, localoffset.y, localoffset.z);
	values.push_back(string(str));
	m_JsonDB.AddNode(keys, values);*/



	Vector3f aftloct = final.ExtractTranslation();
	if (pmesh->m_BoneNodeMap.find(bonename) != pmesh->m_BoneNodeMap.end())
	{
		//pmesh->m_BoneNodeMap[bonename]->mTransformation = final.GetaiMatrix4x4();
		
		Matrix4f totalfinal = parentMat * final;
		pmesh->m_BoneGlobalTrasMap[bonename] = totalfinal;
		Vector3f diff = totalfinal.ExtractTranslation() - transformtochange.ExtractTranslation();

		jsonModelFormat.nodemap[bonename].pos = diff;
		
	}
	else
	{
		printf("");
	}

	//if (pmesh->m_BoneMapping.find(bonename) != pmesh->m_BoneMapping.end())
	//{
	//	uint index = pmesh->m_BoneMapping[bonename];
	//	Matrix4f matinv = mat;
	//	matinv.Inverse();
	//	//pmesh->m_BoneInfo[index].BoneOffset = matinv;


	//	Matrix4f globalm = Matrix4f(pmesh->GetNodeGlobalTransformation(pmesh->m_BoneNodeMap[bonename]));

	//	Matrix4f test = globalm * pmesh->m_BoneInfo[index].BoneOffset;
	//	printf("");
	//}
	//else
	//{
	//	printf("");
	//}

	return;

	//int i = 0;

	//if (boneinfo.pMeshVec.size() > 0)
	//{
	//	for (vector<aiMesh*>::iterator it = boneinfo.pMeshVec.begin(); it < boneinfo.pMeshVec.end(); it++)
	//	{
	//		if (*it != NULL)
	//		{
	//			//(*it)->mBones[boneinfo.BoneIndexVec[i]]->mOffsetMatrix = boneinfo.BoneOffset.GetaiMatrix4x4();

	//			/*Matrix4f identity;
	//			identity.InitIdentity();
	//			(*it)->mBones[boneinfo.BoneIndexVec[i]]->mOffsetMatrix = identity.GetaiMatrix4x4();
	//			boneinfo.BoneOffset = identity;*/


	//			//pmesh->m_BoneInfo[pmesh->m_BoneMapping[bonename]].NodeTransformation = mat;
	//			//pmesh->m_NodeMap[bonename]->mTransformation = mat.GetaiMatrix4x4();



	//			//pmesh->m_BoneInfo[pmesh->m_BoneMapping[bonename]] = boneinfo;
	//		}
	//		i++;

	//	}

	//	boneinfo.BoneOffset = mat;// .Inverse();
	//	pmesh->m_BoneInfo[pmesh->m_BoneMapping[bonename]] = boneinfo;
	//}
	//else
	//{
	//	Matrix4f identity;
	//	identity.InitIdentity();
	//	boneinfo.BoneOffset = mat;


	//	//printf(bonename.c_str());
	//}




	//boneinfo.FinalTransformation = boneinfo.GlobalInverseTransform * boneinfo.Parentformation * boneinfo.NodeTransformation * boneinfo.BoneOffset;

}
