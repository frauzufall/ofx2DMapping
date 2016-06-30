#include "ofx2DMappingView.h"

ofx2DMappingView::ofx2DMappingView(const string &name, const ofJson &config):ofxGuiPanel(name) {

	zoom = 1;
	zoom_pos = ofPoint(0.5,0.5);
	direct_edit.set("direct edit", false);
	save.set("save");
	import.set("import svg");
	select_all.set("select all");
	deselect_all.set("deselect all");
	delete_all.set("delete all");
	setup_done = false;
	mapping_forms = nullptr;
	object_list = nullptr;

	_setConfig(config);

}

ofx2DMappingView::~ofx2DMappingView(){
	if(setup_done){
		vector<ofPtr<ofx2DMappingObject>> options = ctrl->getOptions();
		for(unsigned int i = 0; i < options.size(); i++) {
			options.at(i)->pleaseCopyMe.removeListener(this, &ofx2DMappingView::addedObject);
		}
		save.removeListener(ctrl, &ofx2DMappingController::saveMappingDefault);
		import.removeListener(this, &ofx2DMappingView::importSvg);
		direct_edit.removeListener(this, &ofx2DMappingView::setEditMode);
		select_all.removeListener(this, &ofx2DMappingView::selectAllObjects);
		deselect_all.removeListener(this, &ofx2DMappingView::deselectAllObjects);
		delete_all.removeListener(this, &ofx2DMappingView::removeAllObjects);
		ofRemoveListener(object_list->elementRemoved, this, &ofx2DMappingView::removeForm);
		ofRemoveListener(object_list->elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);
	}
}

void ofx2DMappingView::setControl(ofx2DMappingController *ctrl) {
	this->ctrl = ctrl;
}

void ofx2DMappingView::setup(float x, float y, float w, float h) {

	setup_done = true;

	setConfig(ofJson({
						 {"align-items", "stretch"},
						 {"flex-direction", "row"}
					 }));

	ofxGuiGroup* main_panel = this->addPanel("MAPPING", ofJson({{"flex-direction", "column"}, {"width", 200}}));
	ofxGuiGroup* list_panel = this->addGroup("MAPPING OBJECTS", ofJson({{"flex-direction", "column"}, {"width", 200}}));
	mapping_forms = this->add<ofx2DFormMapping>();
	mapping_forms->setConfig(ofJson({{"flex-direction", "column"}, {"flex", 1}}));

	//MAIN OPTIONS PANEL
	save.addListener(ctrl, &ofx2DMappingController::saveMappingDefault);
	main_panel->add(save);

	import.addListener(this, &ofx2DMappingView::importSvg);
	main_panel->add(import);

	direct_edit.addListener(this, &ofx2DMappingView::setEditMode);
	main_panel->add(direct_edit);

	//CALIBRATION OPTIONS

	ofxGuiGroup* calibration_options = main_panel->addGroup("CALIBRATION OPTIONS", ofJson({{"flex-direction", "column"}}));

	calibration_options->add(ctrl->getCalibrating());
	calibration_options->add(ctrl->getCalBorder());
	calibration_options->add(ctrl->getCalGrey());

	//OBJECT LIST PANEL

	ofxGuiGroup* add_buttons_panel = list_panel->addGroup("ADD MAPPING OBJECTS", ofJson({{"flex-direction", "column"}}));

	vector<ofPtr<ofx2DMappingObject>> options = ctrl->getOptions();
	for(unsigned int i = 0; i < options.size(); i++) {
		ofColor c = options.at(i)->color;
		ofxGuiElement* element = add_buttons_panel->add(options.at(i)->pleaseCopyMe.set("add " + options.at(i)->name, false));
		if(c.getSaturation() > 0){
			element->setTextColor(c);
		}
		options.at(i)->pleaseCopyMe.addListener(this, &ofx2DMappingView::addedObject);
	}


	//LIST MANIPULATION OPTIONS

	ofxGuiGroup* list_options = list_panel->addGroup("OBJECT MANIPULATION", ofJson({{"flex-direction", "column"}}));

	select_all.addListener(this, &ofx2DMappingView::selectAllObjects);
	list_options->add(select_all);

	deselect_all.addListener(this, &ofx2DMappingView::deselectAllObjects);
	list_options->add(deselect_all);

	delete_all.addListener(this, &ofx2DMappingView::removeAllObjects);
	list_options->add(delete_all);

	//OBJECT LIST

//	ofxGuiGroup::Config object_list_config = group_config;
//	object_list_config.spacing = 1;
	object_list = list_panel->add<ofxSortableList>("MAPPING OBJECT LIST", ofJson({{"flex-direction", "column"}}));
	ofAddListener(object_list->elementRemoved, this, &ofx2DMappingView::removeForm);
	ofAddListener(object_list->elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);

	//MAPPING RECT PANEL

	mapping_forms->setup("MAPPING FORMS", ctrl->getProjector(), object_list, w, h);
	mapping_forms->setMappingBackground(ctrl->getOutput());


	setName("Mapping");
	setShowHeader(false);

	setShape(x,y,w,h);

	updateObjectList();


}

void ofx2DMappingView::addedObject(bool & clickstart){
	if(clickstart){
		ctrl->update();
		mapping_forms->updateForms();
		updateObjectList();
	}
}

void ofx2DMappingView::updateObjectList() {

	object_list->clear();

	ofPtr<ofx2DMappingProjector> p = ctrl->getProjector();

	for(int i = p->shapeCount()-1; i >= 0; i--) {

		ofPtr<ofx2DMappingObject> mq = p->getMappingObject(i);
		if(mq) {
			//insert toggles at beginning of list
			mq->editable.setName(mq->name);
			ofColor c = mq->color;
			ofxGuiElement* toggle = object_list->add(mq->editable);
			if(c.getSaturation() > 0){
				toggle->setTextColor(c);
			}
		}
	}
}

void ofx2DMappingView::importSvg() {
	ctrl->importSvg();
	mapping_forms->updateForms();
	object_list->clear();
	updateObjectList();
}

void ofx2DMappingView::removeForm(RemovedElementData &data) {

	ofPtr<ofx2DMappingProjector> p = ctrl->getProjector();
	int index = p->shapeCount()-1-data.index;
	if(p->removeShape(index)) {
		p->updateOutlines();
		mapping_forms->updateForms();
	}

}

void ofx2DMappingView::reorderForm(MovingElementData &data) {

	ofPtr<ofx2DMappingProjector> p = ctrl->getProjector();

	int index1 = p->shapeCount()-1-data.old_index;
	int index2 = p->shapeCount()-1-data.new_index;

	bool swapped = p->swapShapes(index1,index2);
	if(swapped) {
		mapping_forms->updateForms();
	}

}

void ofx2DMappingView::setMappingBackground(ofFbo_ptr fbo) {
	mapping_forms->setMappingBackground(fbo);
}

void ofx2DMappingView::showSource(bool show) {
	mapping_forms->getShowSource().set(show);
}

void ofx2DMappingView::setEditMode(bool &) {
	mapping_forms->setEditMode(direct_edit);
}

ofx2DFormMapping *ofx2DMappingView::getFormMapping() {
	return mapping_forms;
}

void ofx2DMappingView::selectAllObjects() {
	for(unsigned int i = 0; i < ctrl->getProjector()->shapeCount(); i++) {
		ctrl->getProjector()->getMappingObject(i)->editable = true;
	}
}

void ofx2DMappingView::deselectAllObjects() {
	for(unsigned int i = 0; i < ctrl->getProjector()->shapeCount(); i++) {
		ctrl->getProjector()->getMappingObject(i)->editable = false;
	}
}

void ofx2DMappingView::removeAllObjects() {
	ctrl->getProjector()->removeAllShapes();
	ctrl->getProjector()->updateOutlines();
	mapping_forms->updateForms();
	updateObjectList();
}
