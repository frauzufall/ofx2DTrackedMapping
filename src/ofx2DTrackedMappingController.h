#pragma once

#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofxOpenCv.h"
	
class ofx2DTrackedMappingController : public ofx2DMappingController {

    public:

        ofx2DTrackedMappingController();
        ~ofx2DTrackedMappingController() {}

        void setup(string xml_path);
        void update();
        ofTexture& getCamTexture();
        ofxCvGrayscaleImage& getThresholdImage();

        ofParameter<int> & getThreshold();

private:

        void  calcSimplificaiton( ofPolyline & lineToSimplify, vector < ofPoint > & results);

        ofVideoGrabber vidGrabber;
        ofTexture videoTexture;
        ofParameter<int> camWidth;
        ofParameter<int> camHeight;

        ofxCvColorImage colorImage;
        ofxCvGrayscaleImage grayImage;
        ofxCvContourFinder contourFinder;
        ofParameter<int> threshold;
};


