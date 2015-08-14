#include "ofx2DTrackedMappingView.h"
#include "ofxGuiGraphics.h"

ofx2DTrackedMappingView::ofx2DTrackedMappingView():ofx2DMappingView() {

}

void ofx2DTrackedMappingView::setup(float x, float y, float w, float h) {

    ofx2DMappingView::setup(x,y,w,h);

    camPanel.setup("Camera settings");
    camPanel.add(((ofx2DTrackedMappingController*)ctrl)->getThreshold());
    ofxGuiGraphics::Config c;
    c.graphics = &((ofx2DTrackedMappingController*)ctrl)->getCamTexture();
    c.canvasName = "cam";
    camPanel.add<ofxGuiGraphics>(c);
    ofxGuiGraphics::Config c2;
    c2.graphics = &((ofx2DTrackedMappingController*)ctrl)->getThresholdImage();
    c2.canvasName = "threshold";
    camPanel.add<ofxGuiGraphics>(c2);

    main_panel.add(camPanel);

}

void ofx2DTrackedMappingView::update() {

    mapping_forms.updateForms();
    object_list.clear();
    updateObjectList();

}

void ofx2DTrackedMappingView::setControl(ofx2DTrackedMappingController *ctrl) {
    this->ctrl = ctrl;
}
