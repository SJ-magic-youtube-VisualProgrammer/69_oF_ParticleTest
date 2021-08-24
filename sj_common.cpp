/************************************************************
************************************************************/
#include "sj_common.h"

/************************************************************
************************************************************/
/********************
********************/
int GPIO_0 = 0;
int GPIO_1 = 0;

/********************
********************/
GUI_GLOBAL* Gui_Global = NULL;

FILE* fp_Log = nullptr;


/************************************************************
func
************************************************************/
/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha_dt, double dt)
{
	double Alpha;
	if((Alpha_dt <= 0) || (Alpha_dt < dt))	Alpha = 1;
	else									Alpha = 1/Alpha_dt * dt;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double LPF(double LastVal, double CurrentVal, double Alpha)
{
	if(Alpha < 0)		Alpha = 0;
	else if(1 < Alpha)	Alpha = 1;
	
	return CurrentVal * Alpha + LastVal * (1 - Alpha);
}

/******************************
******************************/
double sj_max(double a, double b)
{
	if(a < b)	return b;
	else		return a;
}

/******************************
******************************/
bool checkIf_ContentsExist(char* ret, char* buf)
{
	if( (ret == NULL) || (buf == NULL)) return false;
	
	string str_Line = buf;
	Align_StringOfData(str_Line);
	vector<string> str_vals = ofSplitString(str_Line, ",");
	if( (str_vals.size() == 0) || (str_vals[0] == "") ){ // no_data or exist text but it's",,,,,,,".
		return false;
	}else{
		return true;
	}
}

/******************************
******************************/
void Align_StringOfData(string& s)
{
	size_t pos;
	while((pos = s.find_first_of(" 　\t\n\r")) != string::npos){ // 半角・全角space, \t 改行 削除
		s.erase(pos, 1);
	}
}

/******************************
******************************/
void print_separatoin()
{
	printf("---------------------------------\n");
}

/************************************************************
class
************************************************************/

/******************************
******************************/
void GUI_GLOBAL::setup(string GuiName, string FileName, float x, float y)
{
	/********************
	********************/
	gui.setup(GuiName.c_str(), FileName.c_str(), x, y);
	
	/********************
	********************/
	Group_Particle.setup("Particle");
		Group_Particle.add(ParticleFriction.setup("friction", 0.995, 0.95, 0.999));
		Group_Particle.add(ParticleForceScale.setup("scale", 0.00019, 0.0001, 0.001));
		Group_Particle.add(Particle_PointSize.setup("PointSize", 2.0, 1.0, 4));
		{
			ofColor initColor = ofColor(10, 100, 255);
			ofColor minColor = ofColor(0, 0, 0, 0);
			ofColor maxColor = ofColor(255, 255, 255, 255);
			Group_Particle.add(Particle_col.setup("color", initColor, minColor, maxColor));
		}
		Group_Particle.add(Particle_SpeedThresh.setup("SpeedThresh", 0.06, 0.01, 0.1));
		Group_Particle.add(b_Particle_Fade.setup("b_Particle_Fade", true));
		Group_Particle.add(b_Particle_UseMouse.setup("UseMouse", true));
		Group_Particle.add(b_ParticleAttraction.setup("b_Attraction", false));
	gui.add(&Group_Particle);
	
	Group_WebCam.setup("WebCam");
		Group_WebCam.add(WebCam_Alpha.setup("Alpha", 65, 0, 255));
		Group_WebCam.add(b_WebCam_Gray.setup("Gray", false));
	gui.add(&Group_WebCam);
	
	Group_Img.setup("img");
		Group_Img.add(img_Alpha.setup("Alpha", 65, 0, 255));
	gui.add(&Group_Img);
	


	/********************
	********************/
	gui.minimizeAll();
}

