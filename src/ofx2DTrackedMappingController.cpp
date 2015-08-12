#include "ofx2DTrackedMappingController.h"
#include "ofx2DMappingObject.h"


class triangle {
public:
    int indices[3];
    float area;
    triangle * prev;
    triangle * next;
};

bool compareTri ( triangle * i, triangle * j){

    // important note here:
    // http://stackoverflow.com/questions/12290479/stdsort-fails-on-stdvector-of-pointers

    if (i->area != j->area) return i->area < j->area;
    else return false;

}

//--------------------------------------------------------------
inline float triArea(ofPoint d0, ofPoint d1, ofPoint d2)
{
    double dArea = ((d1.x - d0.x)*(d2.y - d0.y) - (d2.x - d0.x)*(d1.y - d0.y))/2.0;
    return (dArea > 0.0) ? dArea : -dArea;
}



ofx2DTrackedMappingController::ofx2DTrackedMappingController():ofx2DMappingController() {

}

void ofx2DTrackedMappingController::setup(string xml_path) {
    ofx2DMappingController::setup(xml_path);

    camWidth = 320;
    camHeight = 240;

    vector<ofVideoDevice> devices = vidGrabber.listDevices();

    for(unsigned int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }

    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(60);
    vidGrabber.initGrabber(camWidth, camHeight);

    videoTexture.allocate(camWidth, camHeight, OF_PIXELS_RGB);
    ofSetVerticalSync(true);

    colorImage.allocate(camWidth, camHeight);
    grayImage.allocate(camWidth, camHeight);

    threshold.set("threshold", 80, 0, 255);

}

void ofx2DTrackedMappingController::update() {

    vidGrabber.update();

    ofPtr<ofx2DMappingObject> mo = available_shapes.at(0);
    ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(mo);

    if(vidGrabber.isFrameNew() && available_shapes.size() > 0 && shape) {
        colorImage.setFromPixels(vidGrabber.getPixels());
        grayImage = colorImage;
        grayImage.threshold(threshold);
        contourFinder.findContours(grayImage, 20, (camWidth*camHeight)/3, 10, true);

        getProjector(0)->removeAllShapes();
        for (unsigned int i = 0; i < contourFinder.blobs.size(); i++){

            ofxCvBlob blob = contourFinder.blobs[i];
            if(!blob.hole){

                getProjector(0)->copyShape(shape);

                vector<ofPoint> pts = blob.pts;

                shape->dst[0] = blob.boundingRect.getTopLeft() / ofPoint(camWidth, camHeight);
                shape->dst[1] = blob.boundingRect.getTopRight() / ofPoint(camWidth, camHeight);
                shape->dst[2] = blob.boundingRect.getBottomRight() / ofPoint(camWidth, camHeight);
                shape->dst[3] = blob.boundingRect.getBottomLeft() / ofPoint(camWidth, camHeight);

                shape->polyline.clear();
                for(auto p: pts){
                    shape->polyline.addVertex(p / ofPoint(camWidth, camHeight));
                }
                vector<ofPoint> simplified_pts;
                calcSimplificaiton(shape->polyline, simplified_pts);
                shape->polyline.clear();
                for(auto p: simplified_pts){
                    shape->polyline.addVertex(p);
                }
            }
        }
    }

    //check if any object template wants to be copied to stage
    for(auto &e: getOptions()){
        if(e->pleaseCopyMe.get()){
            e->pleaseCopyMe.set(false);
            getProjector(0)->copyShape(e);
        }
    }

    for(uint i = 0; i < projectors.size(); i++) {
        projectors[i].update();
    }

    updateFbo(0);
    updateAreaFbo(0);

}

ofTexture& ofx2DTrackedMappingController::getCamTexture(){
    return vidGrabber.getTexture();
}

ofxCvGrayscaleImage &ofx2DTrackedMappingController::getThresholdImage(){
    return grayImage;
}

ofParameter<int> &ofx2DTrackedMappingController::getThreshold(){
    return threshold;
}

/*
 * copied from ofZack's example https://github.com/ofZach/Visvalingam-Whyatt
 */
void ofx2DTrackedMappingController::calcSimplificaiton( ofPolyline & lineToSimplify, vector < ofPoint > & results){


    results = lineToSimplify.getVertices();


    int total = lineToSimplify.size();


    // if we have 100 points, we have 98 triangles to look at
    int nTriangles = total - 2;


    triangle * triangles[ nTriangles ];

    for (int i = 1; i < total-1; i++){
        triangle * tempTri = new triangle();
        tempTri->indices[0] = i-1;
        tempTri->indices[1] = i;
        tempTri->indices[2] = i+1;
        tempTri->area = triArea(        lineToSimplify[tempTri->indices[0]],
                                        lineToSimplify[tempTri->indices[1]],
                                        lineToSimplify[tempTri->indices[2]]);
        triangles[i-1] = tempTri;
    }

    // set the next and prev triangles, use NULL on either end. this helps us update traingles that might need to be removed
    for (int i = 0; i < nTriangles; i++){
        triangles[i]->prev = (i == 0 ? NULL : triangles[i-1]);
        triangles[i]->next = (i == nTriangles-1 ? NULL : triangles[i+1]);
    }

    std::vector<triangle*> trianglesVec;

    for (int i = 0; i < nTriangles; i++){
        trianglesVec.push_back(triangles[i]);
    }



    int count = 0;
    while ( !trianglesVec.empty()){



        ofSort(trianglesVec,compareTri);

        triangle * tri = trianglesVec[0];

        results[tri->indices[1]].z = total - count;         // store the "importance" of this point in numerical order of removal (but inverted, so 0 = most improtant, n = least important.  end points are 0.
        count ++;


        if (tri->prev != NULL){
            tri->prev->next = tri->next;
            tri->prev->indices[2] = tri->indices[2];  // check!

            tri->prev->area = triArea(      lineToSimplify[tri->prev->indices[0]],
                                            lineToSimplify[tri->prev->indices[1]],
                                            lineToSimplify[tri->prev->indices[2]]);

        }

        if (tri->next != NULL){
            tri->next->prev = tri->prev;
            tri->next->indices[0] = tri->indices[0];  // check!


            tri->next->area = triArea(      lineToSimplify[tri->next->indices[0]],
                                            lineToSimplify[tri->next->indices[1]],
                                            lineToSimplify[tri->next->indices[2]]);


        }

        trianglesVec.erase(trianglesVec.begin());



    }

    // free the memory we just allocated above.
    for (int i = 0; i < nTriangles; i++){
        delete triangles[i];
    }



}
