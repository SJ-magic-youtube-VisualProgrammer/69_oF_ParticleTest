/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxGui.h"

#include "ofxVboSet.h"

#include "sj_common.h"


/************************************************************
class
************************************************************/

/**************************************************
description
	äePARTICLE(1ó±)ÇÃ
	- position
	- velocity
	- îÌforce
	Çä«óù.
	
	ï`âÊã@î\ÇÕï€éùÇµÇ»Ç¢.
	
unit	
	ms
	pixel
**************************************************/
class PARTICLE {
private:
	/****************************************
	****************************************/
	/********************
	********************/
	ofVec2f position;
	ofVec2f velocity;
	ofVec2f force;
	
	/****************************************
	****************************************/
	void updateVel(int dt);
	void updatePos(int dt);
	void throughOfWalls();
	
public:
	/****************************************
	****************************************/
	PARTICLE();

	void resetForce();
	void update(int dt);
	void addAttractionForce(float x, float y, float radius);
	
	float get_Speed()	{ return velocity.length(); }
	const ofVec2f& get_pos()	{ return position; }
};

/**************************************************
**************************************************/
class PARTICLE_SET {
private:
	/********************
	********************/
	enum{
		// NUM_PARTICLES = 100000,
		// NUM_PARTICLES = 60000,
		NUM_PARTICLES = 30000,
		// NUM_PARTICLES = 20000,
	};
	
	/********************
	********************/
	vector<PARTICLE> particles;
	ofx__VBO_SET VboSet;
	bool atraction;
	
	ofVec2f CenterOfForce = ofVec2f(0, 0);
	ofVec2f t_ofs__SingedNoise_for_CenterOfForce = ofVec2f(0, 0);
	const float T_ForcePos = 8.0f;
	
	/********************
	********************/
	// void clear_VboSetting_gl();
	void Refresh_vboVerts();
	void Refresh_vboColor();
	

public:
	PARTICLE_SET();
	~PARTICLE_SET();
	
	void setup();
	void update();
	void draw();

	void set_attractive(bool b_val);
};

