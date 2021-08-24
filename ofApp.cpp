/************************************************************
************************************************************/
#include "ofApp.h"

/************************************************************
************************************************************/

/******************************
******************************/
ofApp::ofApp(int _Cam_id, int _CamWidth, int _CamHeight, int _CamFps)
: Cam_id(_Cam_id)
, CamWidth(_CamWidth)
, CamHeight(_CamHeight)
, CamFps(_CamFps){
}

/******************************
******************************/
ofApp::~ofApp(){
	delete Gui_Global;
	
	if(fp_Log)	fclose(fp_Log);
}

/******************************
******************************/
void ofApp::exit(){
	printf("> Good-bye\n");
	fflush(stdout);
}


/******************************
******************************/
void ofApp::setup(){
	/********************
	********************/
	ofSetWindowTitle("particle test");
	
	ofSetWindowShape( WINDOW_WIDTH, WINDOW_HEIGHT );
	ofSetVerticalSync(false);	// trueとどっちがいいんだろう？
	ofSetFrameRate(30);
	
	ofSetEscapeQuitsApp(false);
	
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	/********************
	********************/
	fp_Log = fopen("../../../data/Log.csv", "w");
	setup_Gui();
	
	/********************
	********************/
	img.load("img.png");
	ParticleSet.setup();
	
	setup_camera();
	fbo_WebCam.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA);
	ClearFbo(fbo_WebCam);
	
	shader_Gray.load( "shader_sj/Gray.vert", "shader_sj/Gray.frag" );
	
	fbo_output.allocate(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA);
	ClearFbo(fbo_output);
	
	/********************
	********************/
	// myGlitch.setup(&fbo_WebCam);
	myGlitch.setup(&fbo_output);
	Disable_All_Glitch();
}

/******************************
******************************/
void ofApp::ClearFbo(ofFbo& fbo)
{
	fbo.begin();
		ofClear(0, 0, 0, 0);
	fbo.end();
}

/******************************
******************************/
void ofApp::setup_camera()
{
	/********************
	********************/
	printf("> setup camera\n");
	fflush(stdout);
	
	/********************
	細かく情報出してもらう
	********************/
	ofSetLogLevel(OF_LOG_VERBOSE);
    WebCam.setVerbose(true);
	
	vector< ofVideoDevice > Devices = WebCam.listDevices();// 上 2行がないと、List表示されない.
	
	/********************
	********************/
	if(Cam_id == -1){
		ofExit();
		return;
	}else{
		if(Devices.size() <= Cam_id) { ERROR_MSG(); ofExit(); return; }
		
		WebCam.setDeviceID(Cam_id);
		printf("\n> WebCam set device id = %d\n", Cam_id);
		printf("> DeviceID     = %d\n", Devices[Cam_id].id);
		printf("> CamName      = %s\n", Devices[Cam_id].deviceName.c_str());
		printf("> HardwareName = %s\n", Devices[Cam_id].hardwareName.c_str());
		printf("> serialID     = %s\n", Devices[Cam_id].serialID.c_str());
		
		printf( "\n> Cam size asked  = (%d, %d)\n", int(CamWidth), int(CamHeight) );
		/*
			https://openframeworks.cc/documentation/video/ofVideoGrabber/#show_setDesiredFrameRate
				Set's the desired frame rate of the grabber. This should be called before initGrabber(), which will try to initialize at the desired frame rate. 
				Not all frame rates will be supported, but this at least gives you some abilitity to try grab at different rates.
		*/
		WebCam.setDesiredFrameRate(CamFps);
		// WebCam.initGrabber(CAM_WIDTH, CAM_HEIGHT);
		WebCam.setup(CamWidth, CamHeight, true); // bool ofVideoGrabber::initGrabber(int w, int h, bool setUseTexture){
		printf( "> Cam size actual = (%d, %d)\n\n", int(WebCam.getWidth()), int(WebCam.getHeight()) );
		fflush(stdout);
	}
}

/******************************
description
	memoryを確保は、app start後にしないと、
	segmentation faultになってしまった。
******************************/
void ofApp::setup_Gui()
{
	/********************
	********************/
	Gui_Global = new GUI_GLOBAL;
	Gui_Global->setup("param", "gui.xml", 10, 10);
}

/******************************
******************************/
void ofApp::update(){
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	/********************
	********************/
	WebCam.update();
	if(WebCam.isFrameNew()){
		fbo_WebCam.begin();
			if(Gui_Global->b_WebCam_Gray) shader_Gray.begin();
				ofBackground(0);
				
				ofSetColor(Gui_Global->WebCam_Alpha);
				WebCam.draw(0, 0, fbo_WebCam.getWidth(), fbo_WebCam.getHeight());
			if(Gui_Global->b_WebCam_Gray) shader_Gray.end();
		fbo_WebCam.end();
	}
	
	/********************
	********************/
	ParticleSet.update();
	
	/********************
	********************/
	fbo_output.begin();
		ofBackground(0);
		
		ofSetColor(255);
		fbo_WebCam.draw(0, 0, ofGetWidth(), ofGetHeight());
		
		ofSetColor(Gui_Global->img_Alpha);
		img.draw(0, 0, fbo_WebCam.getWidth(), fbo_WebCam.getHeight());
		
		ofSetColor(255);
		ParticleSet.draw();
	fbo_output.end();
	
	myGlitch.generateFx();
}

/******************************
******************************/
void ofApp::draw(){
	/********************
	********************/
	ofBackground(0);
	
	/********************
	********************/
	ofEnableAlphaBlending();
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	// ofEnableSmoothing();
	
	/********************
	********************/
	ofSetColor(255);
	fbo_output.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	/********************
	********************/
	if(Gui_Global->b_Disp){
		/********************
		********************/
		Gui_Global->gui.draw();
		
		/********************
		********************/
		{
			ofSetColor(255);
			string info;
			info += "FPS = " + ofToString(ofGetFrameRate(), 2) + "\n";
			ofDrawBitmapString(info, 30, 30);
		}
		
		/********************
		********************/
		{
			string info = "";
			
			info += "1 - 0 : Apply glitch effects.\n";
			info += "q - u : Apply color remap effects.\n";
		
			ofSetColor(255);
			ofDrawBitmapString(info, 200,30);
		}
	}

}

/******************************
******************************/
void ofApp::Disable_All_Glitch(){
	myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, false);
	myGlitch.setFx(OFXPOSTGLITCH_GLOW			, false);
	myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, false);
	myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, false);
	myGlitch.setFx(OFXPOSTGLITCH_TWIST			, false);
	myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, false);
	myGlitch.setFx(OFXPOSTGLITCH_NOISE			, false);
	myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, false);
	myGlitch.setFx(OFXPOSTGLITCH_SWELL			, false);
	myGlitch.setFx(OFXPOSTGLITCH_INVERT			, false);
	
	myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
	myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, false);
	myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, false);
	myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, false);
	myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, false);
	myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, false);
	myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, false);
}

/******************************
******************************/
void ofApp::keyPressed(int key){
	/********************
	********************/
	switch(key){
		case 'd':
			Gui_Global->b_Disp = !Gui_Global->b_Disp;
			break;
			
		case 'm':
			Gui_Global->gui.minimizeAll();
			break;
	}
	
	/********************
	********************/
	if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, true);
	if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, true);
	if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, true);
	if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, true);
	if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, true);
	if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, true);
	if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, true);
	if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, true);
	if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, true);
	if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, true);

	if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
	if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, true);
	if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, true);
	if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, true);
	if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, true);
	if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, true);
	if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, true);
}

/******************************
******************************/
void ofApp::keyReleased(int key){
	if (key == '1') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE	, false);
	if (key == '2') myGlitch.setFx(OFXPOSTGLITCH_GLOW			, false);
	if (key == '3') myGlitch.setFx(OFXPOSTGLITCH_SHAKER			, false);
	if (key == '4') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER		, false);
	if (key == '5') myGlitch.setFx(OFXPOSTGLITCH_TWIST			, false);
	if (key == '6') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE		, false);
	if (key == '7') myGlitch.setFx(OFXPOSTGLITCH_NOISE			, false);
	if (key == '8') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN		, false);
	if (key == '9') myGlitch.setFx(OFXPOSTGLITCH_SWELL			, false);
	if (key == '0') myGlitch.setFx(OFXPOSTGLITCH_INVERT			, false);

	if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
	if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE	, false);
	if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE	, false);
	if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE	, false);
	if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT	, false);
	if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT	, false);
	if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT	, false);
}

/******************************
******************************/
void ofApp::mouseMoved(int x, int y ){

}

/******************************
******************************/
void ofApp::mouseDragged(int x, int y, int button){

}

/******************************
******************************/
void ofApp::mousePressed(int x, int y, int button){
	ParticleSet.set_attractive(true);
}

/******************************
******************************/
void ofApp::mouseReleased(int x, int y, int button){
	ParticleSet.set_attractive(false);
}

/******************************
******************************/
void ofApp::mouseEntered(int x, int y){

}

/******************************
******************************/
void ofApp::mouseExited(int x, int y){

}

/******************************
******************************/
void ofApp::windowResized(int w, int h){

}

/******************************
******************************/
void ofApp::gotMessage(ofMessage msg){

}

/******************************
******************************/
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
