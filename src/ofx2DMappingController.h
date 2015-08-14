#pragma once

#include "ofMain.h"
#include "ofx2DMappingProjector.h"
#include "ofxXmlSettings.h"

typedef ofPtr<ofxXmlSettings> ofxXmlSettings_ptr;
typedef ofPtr<ofFbo> ofFbo_ptr;
typedef ofPtr<ofImage> ofImage_ptr;
typedef ofPtr<ofTexture> ofTexture_ptr;
	
class ofx2DMappingController {

    public:

        ofx2DMappingController();
        ~ofx2DMappingController() {
            projectors.clear();
        }

        void						setup(string xml_path);
        void						update();
        void                        updateFbo(int projector_id);
        void                        updateAreaFbo(int projector_id);

        void						setupMapping();
        void						reloadMapping(ofxXmlSettings_ptr xml);
        void                        setRectangle(ofxXmlSettings_ptr xml, ofPoint* rectangle);
        ofPoint                     getPoint(ofxXmlSettings_ptr xml);
        ofPolyline                  getPolyline(ofxXmlSettings_ptr xml);

        void                        drawCalibration(ofx2DMappingProjector *p);

        void						addProjector(float w, float h);
        ofx2DMappingProjector*      getProjector(int id);

        ofPoint                     getPointInMappedArea(ofPoint last_p,ofPoint next_p);
        ofPoint                     intersectionPointPolyline(ofPoint last_p, ofPoint next_p, ofPolyline polyline);

        ofPoint                     getIntersection(ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4);
        bool                        isLeft(ofPoint p1, ofPoint p2, ofPoint p_test);
        bool                        isOnLine(ofPoint p1, ofPoint p2, ofPoint p_test);

        ofParameter<bool>           &getCalibrating();

        ofParameter<float>          &getCalBorder();
        ofParameter<int>            &getCalGrey();

        ofParameter<float> & contentWidth();
        ofParameter<float> & contentHeight();
        ofParameter<float> & controlWidth();
        ofParameter<float> & controlHeight();
        ofParameter<float> & vidMaxWidth();
        ofParameter<float> & vidMaxHeight();

        ofFbo_ptr &getOutput();
        ofFbo_ptr					getArea();

        ofPoint                     controlpoint;

        void keyPressed  (ofKeyEventArgs &args);
        void keyReleased(ofKeyEventArgs &args);

        void saveOutputImage();

        void                        saveMapping(string path, string path_svg, string path_png);
        void                        saveMappingDefault();
        void                        saveMappingAsPng(string path);
        void                        saveMappingAsPng();
        void                        saveMappingAsSvg();
        void                        importSvg();
        void                        importSvg(const std::string path);

        void addTemplate(ofPtr<ofx2DMappingObject> obj);
        template <class T>
        ofPtr<T> addTemplate(string name) {
            ofPtr<T> obj  = ofPtr<T>(new T());
            addTemplate(obj);
            obj->name = name;
            return obj;
        }

        vector<ofPtr<ofx2DMappingObject>> getOptions();

        ofRectangle    getOutputRectangle();
        void           setOutputRectangle(ofRectangle r);

    protected:

        ofPtr<ofx2DMappingObject> createShape(ofx2DMappingProjector* projector, string type, string name);

        bool use_mapping;

        string xml_mapping, svg_mapping, png_mapping;

        ofParameter<float> content_w, content_h;
        ofParameter<float> control_w, control_h;
        ofParameter<float> vid_max_w, vid_max_h;

        vector<ofx2DMappingProjector>			projectors;
        GLfloat						matrix[16];
        ofPoint						plane[4];

        ofParameter<bool>           is_cal;
        ofParameter<float>          cal_border;
        ofParameter<int>            cal_grey;

        ofFbo_ptr           		mapped_content_fbo;
        ofFbo_ptr                   mapped_area_fbo;

        void                        mappedContentToFbo(ofx2DMappingProjector *p);
        void                        mappedAreaToFbo(ofx2DMappingProjector *p);

        vector<ofPtr<ofx2DMappingObject>>   available_shapes;

        ofRectangle                 output_rectangle;

};


