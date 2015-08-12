#include "ofx2DTrackedMapping.h"

ofx2DTrackedMapping::ofx2DTrackedMapping() {
    show_controls = true;
}

void ofx2DTrackedMapping::setup() {
    setup("mapping/mapping.xml");

}

void ofx2DTrackedMapping::setup(string mapping_path) {
    ctrl.setup(mapping_path);
    view.setControl(&ctrl);
    view.setup(0,0,800,600);
}

void ofx2DTrackedMapping::update() {
    ctrl.update();
    if(show_controls) {
        view.update();
    }
}

ofx2DTrackedMappingController* ofx2DTrackedMapping::getControl() {
    return &ctrl;
}

ofx2DTrackedMappingView* ofx2DTrackedMapping::getControlView() {
    return &view;
}

void ofx2DTrackedMapping::draw() {
    ctrl.getOutput()->draw(ctrl.getOutputRectangle());

    if(show_controls) {
        //draw the control panel
        view.draw();
    }
}

void ofx2DTrackedMapping::setOutputShape(float x, float y, float width, float height) {
    ctrl.setOutputRectangle(ofRectangle(x,y,width,height));
    view.getFormMapping()->setOutputForm(x,y,width,height);
}

void ofx2DTrackedMapping::setControlShape(float x, float y, float width, float height) {
    view.setShape(ofRectangle(x,y,width,height));
}

void ofx2DTrackedMapping::showControls(bool show) {
    show_controls = show;
}

ofPtr<ofx2DMappingImage> ofx2DTrackedMapping::addImageTemplate(string name, string path) {
    ofPtr<ofx2DMappingImage> image = addTemplate<ofx2DMappingImage>(name);
    image->loadImage(path);
    image->setColor(ofColor(0,200,255));
    return image;
}

ofPtr<ofx2DMappingFbo> ofx2DTrackedMapping::addFboTemplate(string name, ofPtr<ofFbo> fbo) {
    ofPtr<ofx2DMappingFbo> content = addTemplate<ofx2DMappingFbo>(name);
    content->setFbo(fbo);
    return content;
}

ofPtr<ofx2DMappingColorShape> ofx2DTrackedMapping::addColorTemplate(string name, ofColor color) {
    ofPtr<ofx2DMappingColorShape> shape = addTemplate<ofx2DMappingColorShape>(name);
    shape->setColor(color);
    return shape;
}

ofx2DTrackedMapping::~ofx2DTrackedMapping() {
}
