#pragma once
#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofxSortableList.h"
#include "ofx2DFormMapping.h"

class ofx2DMappingView : public ofxPanel {

public:
	ofx2DMappingView(const ofJson& config = ofJson());
	~ofx2DMappingView();
	void setup(float x, float y, float w, float h);
	void setMappingBackground(ofFbo_ptr fbo);
	void showSource(bool show);
	void setEditMode(bool &);

	void setControl(ofx2DMappingController* ctrl);

	void importSvg();

	ofx2DFormMapping* getFormMapping();


protected:

	bool setup_done;

	ofx2DMappingController* ctrl;

	void updateObjectList();
	void selectAllObjects();
	void deselectAllObjects();
	void removeAllObjects();

	void addedObject(bool&clickstart);

	ofParameter<bool> direct_edit;

//	ofxSortableList *object_list;
	ofxSortableList *object_list;

	ofPoint pos_main_options, pos_calibration_options, pos_add_buttons, pos_shape_list;

	ofxGuiGroup *add_buttons_panel;
	ofx2DFormMapping *mapping_forms;
	ofxGuiGroup *main_panel, *list_panel;
	ofxGuiGroup *calibration_options, *list_options;

	void removeForm(RemovedElementData& data);
	void reorderForm(MovingElementData& data);

	//ofxButton save_btn, import_btn, select_all_btn, deselect_all_btn, delete_all_btn;
	ofParameter<void> save, import, select_all, deselect_all, delete_all;

	float zoom;
	ofPoint zoom_pos;

};
