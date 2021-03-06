#pragma once
#include <stdio.h>

#include "ogldev_util.h"
#include "ogldev_glut_backend.h"
#include "ogldev_pipeline.h"
#include "ogldev_camera.h"

#include <json/json.h>
#include <fstream>
#include "ogldev_skinned_mesh.h"
#include <exception>  
using namespace std;

class JsonModelFormat
{
	class meta
	{
	public: 
		string version;
		string generator;
	};
	class materials
	{

	};
	class node
	{
	public:
		node()
		{
			localpos = Vector3f(0, 0, 0);
			pos = Vector3f(0, 0, 0);
			scl = Vector3f(1, 1, 1);
			rot = Vector4f(0,0,0, 1);
			worldmatrix.InitIdentity();
			localmatrix.InitIdentity();
		}

		string name;
		int parent;
		Vector3f pos;
		Vector3f localpos;
		Vector3f scl;
		Vector4f rot;
		Matrix4f worldmatrix;
		Matrix4f localmatrix;
	};

public: 
	JsonModelFormat() {};
	~JsonModelFormat();

	void LoadFromFile(string filename);
	void Load(Json::Value root);
	void Save(string filename);
	string ToString();
public:
	Json::Value root;
	vector<Vector3f> verts;
	map<string,node> nodemap;

	float eyescalmap;

};

class JsonRole
{
public:
	void Load(Json::Value jsonvalue)
	{

		Json::Value entity = jsonvalue;

		id = entity["id"].asString();
		assetbundle = entity["assetbundle"].asString();

		face_zero_pointy = entity["face_zero_pointy"].asFloat();
		uvsize = entity["uvsize"].asFloat();
		offset_y = entity["offset_y"].asFloat();





		vector<string> namesvec = entity.getMemberNames();

		vector<string> fixvec;
		vector<string> bonesvec;
		vector<string> offsetvec;

		for (vector<string>::iterator it = namesvec.begin(); it != namesvec.end(); it++)
		{
			string key = *it;
			if (startsWith(key, string("fix_")))
			{
				fixvec.push_back(key);
			}
			else if (startsWith(key, string("_")))
			{
				bonesvec.push_back(key);
			}
			else if (startsWith(key, string("offset")))
			{
				offsetvec.push_back(key);
			}
		}
		for (vector<string>::iterator it = fixvec.begin(); it != fixvec.end(); it++)
		{
			float offset = entity[*it].asFloat();
			fix_map[*it] = offset;
		}


		for (vector<string>::iterator it = bonesvec.begin(); it != bonesvec.end(); it++)
		{
			float x = entity[*it]["x"].asFloat();
			float y = entity[*it]["y"].asFloat();
			float z = entity[*it]["z"].asFloat();
			bonespos_map[*it] = Vector3f(x, y, z);
		}
		for (vector<string>::iterator it = offsetvec.begin(); it != offsetvec.end(); it++)
		{
			if (startsWith(*it, "offset_y"))
			{

			}
			else
			{

				float x = entity[*it]["x"].asFloat();
				float y = entity[*it]["y"].asFloat();
				float z = entity[*it]["z"].asFloat();
				offsets_map[*it] = Vector3f(x, y, z);

			}
		}

	}
	bool startsWith(std::string mainStr, std::string toMatch)
	{
		// std::string::find returns 0 if toMatch is found at starting
		if (mainStr.find(toMatch) == 0)
			return true;
		else
			return false;
	}


	string id;
	string assetbundle;
	float face_zero_pointy;
	float uvsize;
	float offset_y;

	map<string, float> fix_map;
	map<string, Vector3f> bonespos_map;
	map<string, Vector3f> offsets_map;
};
class jsonRoles
{
public:
	void LoadFromFile(string filename)
	{

		Json::CharReaderBuilder rbuilder;
		rbuilder["collectComments"] = false;
		std::string errs;
		Json::Value root;
		std::ifstream ifs;
		ifs.open(filename);
		bool ok = Json::parseFromStream(rbuilder, ifs, &root, &errs);
		ifs.close();

		for (int i = 0; i < root.size(); i++)
		{
			Json::Value entity = root[i];
			JsonRole r;
			r.Load(entity);

			roles[r.id] = r;
		}

	}

	map<string,JsonRole> roles;
};
class JsonColorRef
{
public:
	bool LoadFromFile(string filename)
	{
		try
		{

			Json::CharReaderBuilder rbuilder;
			rbuilder["collectComments"] = false;
			std::string errs;
			Json::Value root;
			std::ifstream ifs;
			ifs.open(filename);
			bool ok = Json::parseFromStream(rbuilder, ifs, &root, &errs);
			ifs.close();

			Load(root);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool Load(Json::Value& root)
	{
		try
		{
			manBaseColor[0] = root["manBaseColor_r"].asFloat();
			manBaseColor[1] = root["manBaseColor_g"].asFloat();
			manBaseColor[2] = root["manBaseColor_b"].asFloat();


			manTargetColor[0] = root["manTargetColor_r"].asFloat();
			manTargetColor[1] = root["manTargetColor_g"].asFloat();
			manTargetColor[2] = root["manTargetColor_b"].asFloat();

			womanBaseColor[0] = root["womanBaseColor_r"].asFloat();
			womanBaseColor[1] = root["womanBaseColor_g"].asFloat();
			womanBaseColor[2] = root["womanBaseColor_b"].asFloat();


			womanTargetColor[0] = root["womanTargetColor_r"].asFloat();
			womanTargetColor[1] = root["womanTargetColor_g"].asFloat();
			womanTargetColor[2] = root["womanTargetColor_b"].asFloat();
		
			return true;
		}
		catch (...)
		{
			return false;
		}

	}


	void SaveFile(string& s, string& path)
	{
		ofstream write;

		write.open(path.c_str(), ios::out | ios::binary);
		write.write(s.c_str(), s.length());
		write.close();
	}
	void SaveJsonFile(string& path)
	{
		Json::Value jvalue;
		jvalue["manBaseColor_r"] = manBaseColor[0];
		jvalue["manBaseColor_g"] = manBaseColor[1];
		jvalue["manBaseColor_b"] = manBaseColor[2];
		jvalue["manTargetColor_r"] = manTargetColor[0];
		jvalue["manTargetColor_g"] = manTargetColor[1];
		jvalue["manTargetColor_b"] = manTargetColor[2];
		jvalue["womanBaseColor_r"] = womanBaseColor[0];
		jvalue["womanBaseColor_g"] = womanBaseColor[1];
		jvalue["womanBaseColor_b"] = womanBaseColor[2];
		jvalue["womanTargetColor_r"] = womanTargetColor[0];
		jvalue["womanTargetColor_g"] = womanTargetColor[1];
		jvalue["womanTargetColor_b"] = womanTargetColor[2];





		Json::StreamWriterBuilder  builder;
		builder.settings_["commentStyle"] = "All";
		std::string s = Json::writeString(builder, jvalue);

		SaveFile(s, path);
	}
	cv::Vec3s manBaseColor;
	cv::Vec3s manTargetColor;
	cv::Vec3s womanBaseColor;
	cv::Vec3s womanTargetColor;
};
class JsonFaceInfo
{
public:
	bool LoadFromString(string jsonstring,bool israw = true)
	{
		Json::Value root;
		Json::CharReaderBuilder rbuilder;
		Json::CharReader * reader = rbuilder.newCharReader();
		string errors;
		bool ok = reader->parse(jsonstring.c_str(), jsonstring.c_str() + jsonstring.size(), &root, &errors);

		delete reader;
		if (ok)
		{
			try
			{
				if (israw)
				{
					return LoadRaw(root);

				}
				else {

					return Load(root);
				}

				return true;
			}
			catch (...)
			{
				return false;
			}
			
		}
		
		return false;
	}
	bool LoadFromFile(string filename)
	{
		try
		{

			Json::CharReaderBuilder rbuilder;
			rbuilder["collectComments"] = false;
			std::string errs;
			Json::Value root;
			std::ifstream ifs;
			ifs.open(filename);
			bool ok = Json::parseFromStream(rbuilder, ifs, &root, &errs);
			ifs.close();

			Load(root);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	bool LoadRaw(Json::Value& root)
	{
		try
		{
			if (root["faces"].size() <= 0)
			{
				return false;
			}

			Json::Value froot = root["faces"][0];
			face_token = froot["face_token"].asString();

			float top = froot["face_rectangle"]["top"].asFloat();
			float width = froot["face_rectangle"]["width"].asFloat();
			float left = froot["face_rectangle"]["left"].asFloat();
			float height = froot["face_rectangle"]["height"].asFloat();

			face_rectangle.x = top;
			face_rectangle.y = left;
			face_rectangle.z = width;
			face_rectangle.w = height;

			yaw_angle = froot["attributes"]["headpose"]["yaw_angle"].asDouble();
			roll_angle = froot["attributes"]["headpose"]["roll_angle"].asDouble();
			pitch_angle = froot["attributes"]["headpose"]["pitch_angle"].asDouble();

			gender = froot["attributes"]["gender"]["value"].asString();


			age = froot["attributes"]["age"]["value"].asDouble();

			beauty_male = froot["attributes"]["beauty"]["male_score"].asDouble();
			beauty_female = froot["attributes"]["beauty"]["female_score"].asDouble();


			dark_circle = froot["attributes"]["skinstatus"]["dark_circle"].asDouble();
			stain = froot["attributes"]["skinstatus"]["stain"].asDouble();
			acne = froot["attributes"]["skinstatus"]["acne"].asDouble();
			health = froot["attributes"]["skinstatus"]["health"].asDouble();


			Json::Value landmarkdataValue = froot["landmark"];
			vector<string> namesvec = landmarkdataValue.getMemberNames();
			for (vector<string>::iterator it = namesvec.begin(); it != namesvec.end(); it++)
			{
				float x = landmarkdataValue[*it]["x"].asFloat();
				float y = landmarkdataValue[*it]["y"].asFloat();
				landmarkdata[*it] = Vector2f(x, y);
			}

			return true;
		}
		catch (...)
		{
			return false;
		}

	
	}
	bool Load(Json::Value& root)
	{
		try
		{
			face_token = root["face_token"].asString();

			float top = root["face_rectangle"]["top"].asFloat();
			float width = root["face_rectangle"]["width"].asFloat();
			float left = root["face_rectangle"]["left"].asFloat();
			float height = root["face_rectangle"]["height"].asFloat();

			face_rectangle.x = top;
			face_rectangle.y = left;
			face_rectangle.z = width;
			face_rectangle.w = height;

			yaw_angle = root["face_pose"]["yaw_angle"].asFloat();
			roll_angle = root["face_pose"]["roll_angle"].asFloat();
			pitch_angle = root["face_pose"]["pitch_angle"].asFloat();

			Json::Value landmarkdataValue = root["landmark"]["data"];
			vector<string> namesvec = landmarkdataValue.getMemberNames();
			for (vector<string>::iterator it = namesvec.begin(); it != namesvec.end(); it++)
			{
				float x = landmarkdataValue[*it]["x"].asFloat();
				float y = 1024 - landmarkdataValue[*it]["y"].asFloat();
				landmarkdata[*it] = Vector2f(x, y);
			}	
			return true;
		}
		catch (...)
		{
			return false;
		}

	}

	

public:
	string face_token;
	Vector4f face_rectangle;

	float yaw_angle;
	float roll_angle;
	float pitch_angle;

	string gender;

	float beauty_female;
	float beauty_male;
	float age;
	float dark_circle;
	float stain;
	float acne;
	float health;


	map<string, Vector2f> landmarkdata;


};

class KP
{
public:
	KP() {};
	~KP() {};

	string bonename;
	string facekeypointname;
	string offsetname;

private:

};

class  JsonDebug
{
public:
	JsonDebug()
	{

	};
	~JsonDebug()
	{

	};
	void AddNode(vector<string>& keys, vector<string>& values)
	{
		Json::Value jnode;

		for (int i = 0; i < keys.size(); i++)
		{
			jnode[keys[i]] = values[i];
			root.append(jnode);
		}

	}
	void Save(string& path)
	{
		SaveJsonFile(root, path);
	}
	void Clear()
	{
		root.clear();
	}
private:
	void SaveFile(string& s, string& path)
	{
		ofstream write;

		write.open(path.c_str(), ios::out | ios::binary);
		write.write(s.c_str(), s.length());
		write.close();
	}
	void SaveJsonFile(Json::Value jvalue, string& path)
	{
		Json::StreamWriterBuilder  builder;
		builder.settings_["commentStyle"] = "All";
		std::string s = Json::writeString(builder, jvalue);

		SaveFile(s, path);
	}

	Json::Value root;
};
class JsonKeyPointBonePairs
{
	
public:
	void LoadFromFile(string filename)
	{

		Json::CharReaderBuilder rbuilder;
		rbuilder["collectComments"] = false;
		std::string errs;
		Json::Value root;
		std::ifstream ifs;
		ifs.open(filename);
		bool ok = Json::parseFromStream(rbuilder, ifs, &root, &errs);
		ifs.close();

		int count = root.size();

		vector<string> fixvec;
		vector<string> bonesvec;
		vector<string> offsetvec;

		for (int i = 0;i< count;i++)
		{
			KP kp;
			kp.bonename = root[i][0].asString();
			kp.facekeypointname = root[i][1].asString();
			kp.offsetname = root[i][2].asString();

			pairs.push_back(kp);
		}
		

	}
	
public:
	vector<KP> pairs;

};

class BoneUtility
{
public:
	JsonModelFormat jsonModelFormat;
	JsonModelFormat jsonModelFormatMan;
	JsonModelFormat jsonModelFormatWoman;
	JsonKeyPointBonePairs pairs;
	Json::Value rtjson;

	bool hasMoveBones;
public:
	void Init();
	int ReadJsonFromFile(const char* filename);

	Texture* CalculateSkin(GLuint texture, cv::Mat& refMat, bool isman, JsonRole bonedef, JsonFaceInfo& faceinfo,bool isFrontOrBg = true);
	void CalculateFaceBone(SkinnedMesh* pmesh, JsonRole bonfdef, JsonFaceInfo faceinfo, string& outOffsetJson, Vector3f& centerpos, Vector2f& uvsize,float& yOffset,bool isman);

	void ResetBone();
	void MoveBone(SkinnedMesh* pmesh, string bonename, JsonFaceInfo faceinfo, string facekeypoint, JsonRole bonedef, string boneoffsetname, Vector3f headCenter, float offsetrate = 0.01f);
	void MoveBonePYR(SkinnedMesh* pmesh, string bonename, JsonFaceInfo faceinfo, string facekeypoint, JsonRole bonedef, string boneoffsetname, Vector3f headCenter, float offsetrate = 0.01f);

	void MoveUV(SkinnedMesh* pmesh, JsonRole bonedef);

	Matrix4f GetLocalMatrixFromGlobal(SkinnedMesh* pmesh ,string bonename,Matrix4f globalmat);

	JsonDebug m_JsonDB;


private:
	Matrix4f GetLocalMatrix(Matrix4f totalTrs, Matrix4f parentTrs);
	Vector3f GetLocalPosition(Matrix4f totalTrs, Matrix4f parentTrs);
	Vector3f GetLocalPosition(SkinnedMesh* pmesh, string boneName);
	void SetLocalPosition(SkinnedMesh* pmesh, string boneName, Vector3f localpos);
	void SetLocalPositionOffset(SkinnedMesh* pmesh, string boneName, Vector3f offset, float rate = 1);
	void fixfacedistans_half_x(SkinnedMesh* pmesh, string bons1, string bons2);
	void fixfacedistans_half_z(SkinnedMesh* pmesh, string bons1, string bons2, float  div);
	void fixfacedistans_y(SkinnedMesh* pmesh, string bons1, string bons2);
	void fixfacedistans_x(SkinnedMesh* pmesh, string bons1, string bons2, float add);
	void fixfacedistans_z(SkinnedMesh* pmesh, string bons1, string bons2, float add);

	void eyemapscale(float scale);
};



