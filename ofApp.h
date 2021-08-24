/************************************************************
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"
#include "ofxPostGlitch.h"

#include "sj_common.h"
#include "PARTICLE.h"

/**************************************************
**************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		WINDOW_WIDTH	= 1280,
		WINDOW_HEIGHT	= 720,
	};
	
	/****************************************
	****************************************/
	/********************
	********************/
	PARTICLE_SET ParticleSet;
	
	/********************
	********************/
	ofVideoGrabber WebCam;
	
	const int Cam_id;
	const int CamWidth;
	const int CamHeight;
	const int CamFps;
	
	ofFbo fbo_WebCam;
	ofShader shader_Gray;
	ofImage img;
	
	ofxPostGlitch	myGlitch;
	
	ofFbo fbo_output;
	
	
	/****************************************
	****************************************/
	void setup_Gui();
	void setup_camera();
	void ClearFbo(ofFbo& fbo);
	void Disable_All_Glitch();
	
public:
	/****************************************
	****************************************/
	ofApp(int _Cam_id, int _CamWidth, int _CamHeight, int _CamFps);
	~ofApp();

	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
