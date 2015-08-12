#pragma once
#include "ofMain.h"
#include "ofx2DMapping.h"
#include "ofx2DTrackedMappingController.h"
#include "ofx2DTrackedMappingView.h"

class ofx2DTrackedMapping {

public:
    ofx2DTrackedMapping();
    ~ofx2DTrackedMapping();

    void setup();
    void setup(string mapping_path);
    void update();
    void draw();

    void showControls(bool show);

    ofx2DTrackedMappingController* getControl();
    ofx2DTrackedMappingView* getControlView();

    void setOutputShape(float x, float y, float width, float height);
    void setControlShape(float x, float y, float width, float height);

    template <class T>
    ofPtr<T> addTemplate(string name) {
        return ctrl.addTemplate<T>(name);
    }

    ofPtr<ofx2DMappingImage> addImageTemplate(string name, string path);
    ofPtr<ofx2DMappingFbo> addFboTemplate(string name, ofPtr<ofFbo> fbo);
    ofPtr<ofx2DMappingColorShape> addColorTemplate(string name, ofColor color);

private:

    ofx2DTrackedMappingController ctrl;
    ofx2DTrackedMappingView view;

    bool show_controls;

};
