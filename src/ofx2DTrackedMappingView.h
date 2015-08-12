#pragma once
#include "ofMain.h"
#include "ofx2DMappingView.h"
#include "ofx2DTrackedMappingController.h"

class ofx2DTrackedMappingView : public ofx2DMappingView {

public:
    ofx2DTrackedMappingView();
    void setup(float x, float y, float w, float h);
    void update();

    void setControl(ofx2DTrackedMappingController* ctrl);

private:

    ofxGuiGroup camPanel;

};
