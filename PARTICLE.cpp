/************************************************************
************************************************************/
#include "PARTICLE.h"
#include <time.h>


/************************************************************
PARTICLE methods
************************************************************/

/******************************
��SJ-magic-study/study__rand
	https://github.com/SJ-magic-study/study__rand
******************************/
PARTICLE::PARTICLE(){
	/********************
	********************/
	position.x = ( (double)rand() / ((double)RAND_MAX + 1) ) * ofGetWidth();	// ofRandom(0, ofGetWidth());
	position.y = ( (double)rand() / ((double)RAND_MAX + 1) ) * ofGetHeight();	// ofRandom(0, ofGetHeight());
	
	velocity = ofVec2f(0.0f, 0.0f);
}

/******************************
******************************/
void PARTICLE::resetForce(){
	force.set(0, 0);
}

/******************************
******************************/
void PARTICLE::updateVel(int dt){
	velocity = velocity * Gui_Global->ParticleFriction;
	velocity += force * dt;
}

/******************************
******************************/
void PARTICLE::updatePos(int dt){
	position += velocity * dt;
}

/******************************
******************************/
void PARTICLE::update(int dt){
	updateVel(dt);
	updatePos(dt);
	
	throughOfWalls();
}


/******************************
******************************/
void PARTICLE::throughOfWalls(){
	float minx = 0;
	float miny = 0;
	float maxx = ofGetWidth() - 1;
	float maxy = ofGetHeight() - 1;
	if (position.x < minx) {
		position.x = maxx + position.x;
	}
	if (position.y < miny) {
		position.y = maxy + position.y;
	}
	if (maxx < position.x) {
		position.x = position.x - maxx;
	}
	if (maxy < position.y) {
		position.y = position.y - maxy;
	}
}

/******************************
description
	�����t��������
	
param
	scale
		���S����̋����ɂ����Force�����܂邪�A����ɑ΂���scaling.
		���Ԃ͖��֌W�ł���_�ɒ��ӁB
******************************/
void PARTICLE::addAttractionForce(float x, float y, float radius)
{
	/********************
	********************/
	ofVec2f posOfForce;
	posOfForce.set(x,y);
	
	ofVec2f diff = position - posOfForce;
	float length = diff.length();

	bool b_OutOfRange = false;
	if (radius > 0){
		if (radius < length){
			b_OutOfRange = true;
		}
	}

	if (!b_OutOfRange){
		// float scale = 0.0001 + 0.0001 * cos(2 * 3.14 * ofGetElapsedTimef() / 30.0);
		
		// ��������_�ɂ�����̓x�N�g�����v�Z
		float pct = 1 - (length / radius);
		diff.normalize();
		force.x = - diff.x  * pct * Gui_Global->ParticleForceScale;
		force.y = - diff.y  * pct * Gui_Global->ParticleForceScale;
	}
}

/************************************************************
PARTICLE_SET methods
************************************************************/
/******************************
******************************/
PARTICLE_SET::PARTICLE_SET()
: atraction(false)
{
	/********************
	��)	�ӂ��āAPARTICLE()����srand()��u���Ȃ�����
		PARTICLE�̐����ŁA�s�x srand()������A�����̌��ʂ��S�ē���(�d�Ȃ�)�ɂȂ��Ă��܂��B
	********************/
	srand((unsigned int)time(NULL));
	
}

/******************************
******************************/
PARTICLE_SET::~PARTICLE_SET()
{
}

/******************************
******************************/
void PARTICLE_SET::setup()
{
	/********************
	��C++ std::vector resize��reserve�̈Ⴂ
		https://suzulang.com/c-stdvector-resize%E3%81%A8reserve%E3%81%AE%E9%81%95%E3%81%84-%E5%8A%A0%E7%AD%86%E4%B8%AD%EF%BC%9F/
	
	��std::vector��resize��assign�̈Ⴂ (C++)
		https://minus9d.hatenablog.com/entry/2021/02/07/175159
	********************/
	particles.resize(NUM_PARTICLES);
	// init_particleArray();
	
	/********************
	********************/
	VboSet.setup(NUM_PARTICLES);
	Refresh_vboVerts();
	Refresh_vboColor();
	VboSet.update_vertex_color();
	
	t_ofs__SingedNoise_for_CenterOfForce.x = ( (double)rand() / ((double)RAND_MAX + 1) ) * 1000.0;
	t_ofs__SingedNoise_for_CenterOfForce.y = ( (double)rand() / ((double)RAND_MAX + 1) ) * 1000.0;
}

/******************************
1--2
|  |
0--3
******************************/
void PARTICLE_SET::Refresh_vboVerts()
{
	for(int i = 0; i < NUM_PARTICLES; i++){
		VboSet.set_vboVerts(i, particles[i].get_pos().x, particles[i].get_pos().y);
	}
}

/******************************
color��Speed down�Ƌ��Ƀ����������Ă����悤�ɂ����B
******************************/
void PARTICLE_SET::Refresh_vboColor()
{
	ofColor BaseCol = Gui_Global->Particle_col;
	if(!Gui_Global->b_Particle_Fade){
		VboSet.set_singleColor(BaseCol);
	}else{
		const double Alpha_max = double(BaseCol.a) / 255;
		double tan = Alpha_max / Gui_Global->Particle_SpeedThresh;
		
		for (int i = 0; i < particles.size(); i++) {
			float Alpha;
			if(Gui_Global->Particle_SpeedThresh < particles[i].get_Speed())	Alpha = Alpha_max;
			else															Alpha = tan * particles[i].get_Speed();
			
			ofColor col = BaseCol;
			col.a = Alpha * 255.0;
			
			VboSet.set_vboColor(i, col);
		}
	}
	
	return;
}

/******************************
��How to use mouseX in a class?
	https://forum.openframeworks.cc/t/how-to-use-mousex-in-a-class/3281
******************************/
void PARTICLE_SET::update()
{
	/********************
	********************/
	static int LastINT = ofGetElapsedTimeMillis();
	int now = ofGetElapsedTimeMillis();
	int dt = ofClamp(now - LastINT, 0, 100);
	
	LastINT = now;
	
	/********************
	********************/
	{
		const float scale = 1.0;
		CenterOfForce.x = ofGetWidth()/2  + ofSignedNoise( double(now)/1000 / T_ForcePos + t_ofs__SingedNoise_for_CenterOfForce.x) * (ofGetWidth() * scale);
		CenterOfForce.y = ofGetHeight()/2 + ofSignedNoise( double(now)/1000 / T_ForcePos + t_ofs__SingedNoise_for_CenterOfForce.y) * (ofGetHeight() * scale);
	}
	
	/********************
	update the position of all particle.
	********************/
	for (int i = 0; i < particles.size(); i++){
		particles[i].resetForce();
		if (Gui_Global->b_ParticleAttraction || atraction){
			if(Gui_Global->b_Particle_UseMouse)	particles[i].addAttractionForce(ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY, ofGetWidth());
			else								particles[i].addAttractionForce(CenterOfForce.x, CenterOfForce.y, ofGetWidth());
		}
		
		particles[i].update(dt);
	}
	
	/********************
	********************/
	Refresh_vboVerts();
	VboSet.update_vertex();
	
	Refresh_vboColor();
	VboSet.update_color();
}

/******************************
******************************/
void PARTICLE_SET::draw()
{
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	/********************
	********************/
	ofSetColor(255, 255, 255, 255);
	glPointSize(Gui_Global->Particle_PointSize);
	glEnable(GL_POINT_SMOOTH);
	VboSet.draw(GL_POINTS);
	
	/********************
	********************/
	// �d�͂̓_��`��
	ofFill();
	if (Gui_Global->b_ParticleAttraction || atraction)	ofSetColor(255, 0, 0);
	else												ofSetColor(0, 255, 255);
	
	if(Gui_Global->b_Particle_UseMouse)	ofDrawCircle(ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY, 4);
	else								ofDrawCircle(CenterOfForce.x, CenterOfForce.y, 4);
}

/******************************
******************************/
void PARTICLE_SET::set_attractive(bool b_val)
{
	atraction = b_val;
}

