#include "ofx2DFormMapping.h"

ofx2DFormMapping::ofx2DFormMapping(): ofxBaseGui() {

	shapes.clear();
	resizing = false;
	zoom_factor = 0;
	zoom_speed = 0.1;
	dragging_dst = false;
	mapping_margin = 10;
	show_source.set("show source", true);
	direct_edit.set("direct edit", false);
	parent_projector = nullptr;
	source_empty_bg.setFillColor(ofColor(0,0,0,100));
	source_empty_bg.setFilled(true);

	ofAddListener(this->resize, this, &ofx2DFormMapping::onResized);
	ofAddListener(this->move, this, &ofx2DFormMapping::onMoved);

}

void ofx2DFormMapping::setup(string title, ofPtr<ofx2DMappingProjector> parent_projector, ofxSortableList *parent_list, float w, float h) {
//	setSize(w,h);
//    spacing = Config().spacing;
//    spacingNextElement = Config().spacingNextElement;
//    spacingFirstElement = Config().spacingFirstElement;
//    header = Config().header;
//    minimized = Config().minimized;
//    bShowHeader = Config().showHeader;
//    b.width = defaultWidth;
//    layout = Config().layout;
	clear();
//    filename = Config().filename;
//    guiActive = false;
	setName(title);

	registerMouseEvents(-2);
	setNeedsRedraw();

	this->parent_projector = parent_projector;
	this->parent_list = parent_list;

	rebuild();
}

void ofx2DFormMapping::setMappingRects() {

	if(!resizing){

		resizing = true;

		int margin = 10;
		float amount_dst = 2./3.;

		if(parent_projector){
			if(direct_edit) {
				mapping_rect_dst = mapping_rect_output;
				mapping_rect_src.x = margin;
				mapping_rect_src.y = margin;
				float output_ratio_inv = parent_projector->outputHeight()/parent_projector->outputWidth();
				mapping_rect_src.width = this->getWidth()-2*margin;
				mapping_rect_src.height = mapping_rect_src.width*output_ratio_inv;

				setHeight(mapping_rect_src.getHeight() + 2*margin);

	//            ofxGuiGroup::setSize(mapping_rect_src.width+2*margin, header+mapping_rect_src.height+2*margin);
			}
			else {

				float output_ratio_inv = parent_projector->outputHeight()/parent_projector->outputWidth();

				mapping_rect_dst.x = margin;
				mapping_rect_dst.y = margin;

				mapping_rect_dst.width = getWidth() - margin*2;
				mapping_rect_dst.height = mapping_rect_dst.width*output_ratio_inv;

				mapping_rect_src.setSize(mapping_rect_dst.width,mapping_rect_dst.height);
				mapping_rect_src.setPosition(mapping_rect_dst.getPosition()+ofPoint(0,mapping_rect_dst.height+margin));

	//            ofxGuiGroup::setSize(mapping_rect_dst.width+2*margin, mapping_rect_dst.height+header+mapping_rect_src.height+3*margin);

				mapping_front.clear();
				if(mapping_rect_dst.width > 0 && mapping_rect_dst.height > 0){
					mapping_front.allocate(mapping_rect_dst.width+2*mapping_margin, mapping_rect_dst.height+2*mapping_margin, GL_RGBA);
				}

				setHeight(mapping_rect_src.getBottom() - mapping_rect_dst.getTop() + 3*margin);
			}
		}

		resizing = false;
	}
}

void ofx2DFormMapping::updateForms() {

	if(parent_projector){

		shapes.clear();

		for (unsigned int i = 0; i < parent_projector->shapeCount(); i++) {

			ofPtr<ofx2DMappingObject> obj = parent_projector->getMappingObject(i);

			if(ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(obj)) {

				mappableShape ms;
				ms.color = shape->color;
				ms.polyline.clear();
				ms.dst.clear();
				ms.src.clear();
				ofPolyline line = shape->polyline;

				for(unsigned int j = 0; j < line.size(); j++) {
					ms.polyline.push_back(draggableVertex());
					ms.polyline[j].x = line[j].x*mapping_rect_dst.width+mapping_rect_dst.x;
					ms.polyline[j].y = line[j].y*mapping_rect_dst.height+mapping_rect_dst.y;
					ms.polyline[j].bOver 			= false;
					ms.polyline[j].bBeingDragged 	= false;
					ms.polyline[j].radius = 6;
				}

				for(int j = 0; j < 4; j++) {
					ms.dst.push_back(ofPoint());
					ms.dst[j].x = shape->dst[j].x*mapping_rect_dst.width+mapping_rect_dst.x;
					ms.dst[j].y = shape->dst[j].y*mapping_rect_dst.height+mapping_rect_dst.y;
				}

				if(ofPtr<ofx2DMappingContentShape> cshape = std::dynamic_pointer_cast<ofx2DMappingContentShape>(shape)) {
					for(int j = 0; j < 4; j++) {
						ms.src.push_back(draggableVertex());
						ms.src[j].x = cshape->src[j].x*mapping_rect_src.width+mapping_rect_src.x;
						ms.src[j].y = cshape->src[j].y*mapping_rect_src.height+mapping_rect_src.y;
						ms.src[j].bOver 			= false;
						ms.src[j].bBeingDragged 	= false;
						ms.src[j].radius = 6;
					}
				}

				shapes.push_back(ms);

			}
			else {
				if(ofPtr<ofx2DMappingPoint> point = std::dynamic_pointer_cast<ofx2DMappingPoint>(obj)) {

					mappableShape ms;
					ms.color = point->color;
					ms.src.clear();
					ms.dst.clear();
					ms.polyline.clear();
					draggableVertex v;
					v.x = point->pos.x*mapping_rect_dst.width+mapping_rect_dst.x;
					v.y = point->pos.y*mapping_rect_dst.height+mapping_rect_dst.y;
					v.bOver = false;
					v.bBeingDragged = false;
					v.radius = 13;
					ms.polyline.push_back(v);

					shapes.push_back(ms);

				}
			}

		}

		setNeedsRedraw();


	}

}

void ofx2DFormMapping::updateSourceBackground() {
	//find first content shape from top to use as background for source mapping

	source_bg = 0;
	vector<ofPtr<ofx2DMappingContentShape>> objs = parent_projector->getShapesByClass<ofx2DMappingContentShape>();
	if(objs.size() > 0) {
		for(int i = objs.size()-1; i >= 0; i--) {
			if(objs.at(i)->editable) {
				source_bg = objs.at(i)->getTexture();
				break;
			}
		}
	}
}

void ofx2DFormMapping::rebuild() {
	setMappingRects();
	updateForms();
}

void ofx2DFormMapping::generateDraw(){

	ofxBaseGui::generateDraw();

	updateSourceBackground();
	source_empty_bg.clear();
	source_empty_bg.rectangle(mapping_rect_src);

	if(!direct_edit) {

		//ZOOM TRANSLATION

		translation_dst = zoom_point-zoom_point_scaled+zoom_point_offset;
		if(translation_dst.x > 0) translation_dst.x = 0;
		if(translation_dst.y > 0) translation_dst.y = 0;
		if(translation_dst.x < -addZoom(mapping_rect_dst.getWidth())+mapping_rect_dst.getWidth())
			translation_dst.x = -addZoom(mapping_rect_dst.getWidth())+mapping_rect_dst.getWidth();
		if(translation_dst.y < -addZoom(mapping_rect_dst.getHeight())+mapping_rect_dst.getHeight())
			translation_dst.y = -addZoom(mapping_rect_dst.getHeight())+mapping_rect_dst.getHeight();
	}

	for (unsigned int i = 0; i < shapes.size(); i++) {

		if(shapes[i].polyline.size() > 0){

			shapes[i].dst_bg.setFillColor(ofColor(shapes[i].color,40));

			if(direct_edit) {
				shapes[i].dst_border.setStrokeColor(ofColor(255));
				shapes[i].dst_drag_unset.setStrokeColor(ofColor(255));
				shapes[i].dst_drag_set.setFillColor(ofColor(255));
			}else {
				shapes[i].dst_border.setStrokeColor(ofColor(shapes[i].color,255));
				shapes[i].dst_drag_unset.setStrokeColor(ofColor(shapes[i].color,255));
				shapes[i].dst_drag_set.setFillColor(ofColor(shapes[i].color,255));
			}

			shapes[i].dst_border.clear();
			shapes[i].dst_bg.clear();
			shapes[i].dst_drag_unset.clear();
			shapes[i].dst_drag_set.clear();

			for(unsigned int j = 0; j < shapes[i].polyline.size(); j++) {
				ofPoint p(addZoom(shapes[i].polyline[j]-mapping_rect_dst.getPosition())+mapping_rect_dst.getPosition());
				if(j == 0){
					shapes[i].dst_border.moveTo(p);
					shapes[i].dst_bg.moveTo(p);
				}else{
					shapes[i].dst_border.lineTo(p);
					shapes[i].dst_bg.lineTo(p);
				}
				if(shapes[i].polyline[j].bOver){
					shapes[i].dst_drag_set.moveTo(p);
					shapes[i].dst_drag_set.circle(p,6);
					shapes[i].dst_drag_set.newSubPath();
				}else {
					shapes[i].dst_drag_unset.moveTo(p);
					shapes[i].dst_drag_unset.circle(p,6);
					shapes[i].dst_drag_unset.newSubPath();
				}

			}

			shapes[i].dst_border.close();
			shapes[i].dst_bg.close();


			shapes[i].src_border.setStrokeColor(ofColor(shapes[i].color,255));

			shapes[i].src_border.clear();
			shapes[i].src_drag_unset.clear();
			shapes[i].src_drag_set.clear();

			for(unsigned int j = 0; j < shapes[i].src.size(); j++) {
				ofPoint p(shapes[i].src[j]);
				if(j == 0){
					shapes[i].src_border.moveTo(p);
				}else{
					shapes[i].src_border.lineTo(p);
				}
				if(j%2==0){
					if (shapes[i].src[j].bOver){
						shapes[i].src_drag_set.moveTo(p);
						shapes[i].src_drag_set.circle(p,6);
						shapes[i].src_drag_set.newSubPath();
					}else{
						shapes[i].src_drag_unset.moveTo(p);
						shapes[i].src_drag_unset.circle(p,6);
						shapes[i].src_drag_unset.newSubPath();
					}
				}
			}

			shapes[i].src_border.close();

		}

	}

}

void ofx2DFormMapping::render() {

	ofxBaseGui::render();

	ofEnableAlphaBlending();

	//draw dst

	if(!direct_edit) {
		mapping_front.begin();
		ofClear(0,0,0,0);
		ofTranslate(mapping_margin, mapping_margin);

		ofPushMatrix();
		ofTranslate(-mapping_rect_dst.getPosition());

		//ZOOM TRANSLATION
		ofTranslate(translation_dst);

		ofSetColor(255,160);
		mapping_bg->draw(
					mapping_rect_dst.x,
					mapping_rect_dst.y,
					addZoom(mapping_rect_dst.width),
					addZoom(mapping_rect_dst.height));

	}

	ofSetColor(255);

	for (unsigned int i = 0; i < shapes.size(); i++) {

//        //draw dst rectangles

//        ofNoFill();
//        ofSetColor(80,130,150);

//        ofBeginShape();
//        for(unsigned int j = 0; j < shapes[i].dst.size(); j++) {
//            ofVertex(shapes[i].dst[j]);
//        }
//        ofEndShape(true);

		//draw shape lines and fillings

		shapes[i].dst_bg.draw();
		shapes[i].dst_border.draw();

		if(parent_projector->getMappingObject(i)->editable) {
			shapes[i].dst_drag_unset.draw();
			shapes[i].dst_drag_set.draw();
		}

	}

	if(!direct_edit) {
		ofPopMatrix();
		mapping_front.end();
		mapping_front.draw(mapping_rect_dst.getPosition()-ofPoint(mapping_margin, mapping_margin), mapping_front.getWidth(), mapping_front.getHeight());
	}

	//draw src

	if(show_source) {
		source_empty_bg.draw();
		if(source_bg) {
			ofSetColor(255,60);
			source_bg->draw(
						mapping_rect_src.x,
						mapping_rect_src.y,
						mapping_rect_src.width,
						mapping_rect_src.height);
		}
		for (unsigned int i = 0; i < shapes.size(); i++) {

			shapes[i].src_border.draw();

			if(parent_projector->getMappingObject(i)->editable) {
				shapes[i].src_drag_unset.draw();
				shapes[i].src_drag_set.draw();
			}

		}
	}

}

bool ofx2DFormMapping::mouseMoved(ofMouseEventArgs& args) {

	ofPoint mouse(args.x,args.y);

	mouse = screenToLocal(mouse);

	for (unsigned int i = 0; i < shapes.size(); i++){
		for (unsigned int j = 0; j < shapes[i].polyline.size(); j++){
			ofPoint zoomed_p = addZoomRelativeOfDstRect(shapes[i].polyline[j]);
			float diffx = mouse.x - zoomed_p.x;
			float diffy = mouse.y - zoomed_p.y;
			float dist = sqrt(diffx*diffx + diffy*diffy);
			if (dist < shapes[i].polyline[j].radius){
				if(!shapes[i].polyline[j].bOver){
					shapes[i].polyline[j].bOver = true;
					setNeedsRedraw();
				}
			} else {
				if(shapes[i].polyline[j].bOver){
					shapes[i].polyline[j].bOver = false;
					setNeedsRedraw();
				}
			}
		}
		for (unsigned int j = 0; j < shapes[i].src.size(); j++){
			float diffx = mouse.x - shapes[i].src[j].x;
			float diffy = mouse.y - shapes[i].src[j].y;
			float dist = sqrt(diffx*diffx + diffy*diffy);
			if (dist < shapes[i].src[j].radius){
				if(!shapes[i].src[j].bOver){
					shapes[i].src[j].bOver = true;
					setNeedsRedraw();
				}
			} else {
				if(shapes[i].src[j].bOver){
					shapes[i].src[j].bOver = false;
					setNeedsRedraw();
				}
			}
		}
	}

	return ofxBaseGui::mouseMoved(args);
}


bool ofx2DFormMapping::mouseDragged(ofMouseEventArgs &args) {

	ofPoint mouse(args.x,args.y);

	mouse = screenToLocal(mouse);

	for (unsigned int i = 0; i < shapes.size(); i++) {

		ofPtr<ofx2DMappingObject> obj = parent_projector->getMappingObject(i);

		for (unsigned int j = 0; j < shapes[i].polyline.size(); j++) {

			if (shapes[i].polyline[j].bBeingDragged == true){

				ofPoint zoomed_mouse = removeZoomRelativeOfDstRect(mouse);
				ofPoint zoomed_mapping_rect_dst_lefttop = removeZoomRelativeOfDstRect(mapping_rect_dst.getPosition());
				ofPoint zoomed_mapping_rect_dst_rightbottom = removeZoomRelativeOfDstRect(mapping_rect_dst.getPosition()+ofPoint(mapping_rect_dst.getWidth(), mapping_rect_dst.getHeight()));

				if(mouse.x < mapping_rect_dst.x+mapping_rect_dst.width) {
					if(mouse.x > mapping_rect_dst.x)
						shapes[i].polyline[j].x = zoomed_mouse.x;
					else
						shapes[i].polyline[j].x = zoomed_mapping_rect_dst_lefttop.x;
				}
				else {
					shapes[i].polyline[j].x = zoomed_mapping_rect_dst_rightbottom.x;
				}
				if(mouse.y < mapping_rect_dst.y+mapping_rect_dst.height) {
					if(mouse.y > mapping_rect_dst.y)
						shapes[i].polyline[j].y = zoomed_mouse.y;
					else
						shapes[i].polyline[j].y = zoomed_mapping_rect_dst_lefttop.y;
				}
				else {
					shapes[i].polyline[j].y = zoomed_mapping_rect_dst_rightbottom.y;
				}

				if(shapes[i].polyline.size() == 4) {

					if(ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(obj)) {
						shape->dst[j].x = (shapes[i].polyline[j].x-mapping_rect_dst.x)/mapping_rect_dst.width;
						shape->dst[j].y = (shapes[i].polyline[j].y-mapping_rect_dst.y)/mapping_rect_dst.height;

						shape->polyline[j].x = shape->dst[j].x;
						shape->polyline[j].y = shape->dst[j].y;
					}

				}
				else {

					if(ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(obj)) {
						shape->polyline[j].x = (shapes[i].polyline[j].x-mapping_rect_dst.x)/mapping_rect_dst.width;
						shape->polyline[j].y = (shapes[i].polyline[j].y-mapping_rect_dst.y)/mapping_rect_dst.height;

						ofRectangle bounding = shape->polyline.getBoundingBox();

						shape->dst[0].x = bounding.x;
						shape->dst[0].y = bounding.y;
						shape->dst[1].x = bounding.x+bounding.width;
						shape->dst[1].y = bounding.y;
						shape->dst[2].x = bounding.x+bounding.width;
						shape->dst[2].y = bounding.y+bounding.height;
						shape->dst[3].x = bounding.x;
						shape->dst[3].y = bounding.y+bounding.height;
					}
					if(ofPtr<ofx2DMappingPoint> point = std::dynamic_pointer_cast<ofx2DMappingPoint>(obj)) {
						//j should be 0
						point->pos.x = (shapes[i].polyline[j].x-mapping_rect_dst.x)/mapping_rect_dst.width;
						point->pos.y = (shapes[i].polyline[j].y-mapping_rect_dst.y)/mapping_rect_dst.height;

					}

				}

				obj->newpos = true;
				ofColor c = parent_list->getControl(shapes.size()-1-i)->getFillColor();
				parent_list->getControl(shapes.size()-1-i)->setBackgroundColor(c);
				setNeedsRedraw();
				break;
			}
		}

		if(ofPtr<ofx2DMappingContentShape> cshape = std::dynamic_pointer_cast<ofx2DMappingContentShape>(obj)) {

			for (unsigned int j = 0; j < shapes[i].src.size(); j++){
				if (shapes[i].src[j].bBeingDragged == true){
					if(mouse.x < mapping_rect_src.x+mapping_rect_src.width) {
						if(mouse.x > mapping_rect_src.x)
							shapes[i].src[j].x = mouse.x;
						else
							shapes[i].src[j].x = mapping_rect_src.x;
					}
					else {
						shapes[i].src[j].x = mapping_rect_src.x+mapping_rect_src.width;
					}
					if(mouse.y < mapping_rect_src.y+mapping_rect_src.height) {
						if(mouse.y > mapping_rect_src.y)
							shapes[i].src[j].y = mouse.y;
						else
							shapes[i].src[j].y = mapping_rect_src.y;
					}
					else {
						shapes[i].src[j].y = mapping_rect_src.y+mapping_rect_src.height;
					}
					cshape->src[j].x = (shapes[i].src[j].x-mapping_rect_src.x)/mapping_rect_src.width;
					cshape->src[j].y = (shapes[i].src[j].y-mapping_rect_src.y)/mapping_rect_src.height;

					switch(j) {
					case 0: {
						cshape->src[1].y = cshape->src[j].y;
						cshape->src[3].x = cshape->src[j].x;
						shapes[i].src[1].y = shapes[i].src[j].y;
						shapes[i].src[3].x = shapes[i].src[j].x;
						break;
					}
					case 1: {
						cshape->src[0].y = cshape->src[j].y;
						cshape->src[2].x = cshape->src[j].x;
						shapes[i].src[0].y = shapes[i].src[j].y;
						shapes[i].src[2].x = shapes[i].src[j].x;
						break;
					}
					case 2: {
						cshape->src[1].x = cshape->src[j].x;
						cshape->src[3].y = cshape->src[j].y;
						shapes[i].src[1].x = shapes[i].src[j].x;
						shapes[i].src[3].y = shapes[i].src[j].y;
						break;
					}
					case 3: {
						cshape->src[0].x = cshape->src[j].x;
						cshape->src[2].y = cshape->src[j].y;
						shapes[i].src[0].x = shapes[i].src[j].x;
						shapes[i].src[2].y = shapes[i].src[j].y;
						break;
					}
					default:
						break;
					}

					cshape->newpos = true;
					setNeedsRedraw();
				}
			}

		}

	}

	if(dragging_dst) {
		zoom_point_offset += mouse - last_mouse;
		last_mouse = mouse;
		setNeedsRedraw();
	}

	return ofxBaseGui::mouseDragged(args);
}

bool ofx2DFormMapping::mousePressed(ofMouseEventArgs& args) {

	ofPoint mouse(args.x,args.y);

	mouse = screenToLocal(mouse);

	bool on_element = false;

	for (unsigned int i = 0; i < shapes.size(); i++){
		bool editable = parent_projector->getMappingObject(i)->editable;
		for (unsigned int j = 0; j < shapes[i].polyline.size(); j++){
			ofPoint zoomed_p = addZoomRelativeOfDstRect(shapes[i].polyline[j]);
			float diffx = mouse.x - zoomed_p.x;
			float diffy = mouse.y - zoomed_p.y;
			float dist = sqrt(diffx*diffx + diffy*diffy);
			if (dist < shapes[i].polyline[j].radius){
				if(editable) {
					shapes[i].polyline[j].bBeingDragged = true;
					on_element = true;
					break;
				}
			} else {
				shapes[i].polyline[j].bBeingDragged = false;
			}
		}
		for (unsigned int j = 0; j < shapes[i].src.size(); j++){
			float diffx = mouse.x - shapes[i].src[j].x;
			float diffy = mouse.y - shapes[i].src[j].y;
			float dist = sqrt(diffx*diffx + diffy*diffy);
			if (dist < shapes[i].src[j].radius){
				if(editable) {
					shapes[i].src[j].bBeingDragged = true;
					break;
				}
			} else {
				shapes[i].src[j].bBeingDragged = false;
			}
		}
	}

	if(!on_element && mapping_rect_dst.inside(mouse)) {
		dragging_dst = true;
		last_mouse = mouse;
	}

	return ofxBaseGui::mousePressed(args);
}

bool ofx2DFormMapping::mouseReleased(ofMouseEventArgs &args) {

	for (unsigned int i = 0; i < shapes.size(); i++){
		for (unsigned int j = 0; j < shapes[i].polyline.size(); j++){
			shapes[i].polyline[j].bBeingDragged = false;
			if(shapes.size()-1-i < parent_list->getNumControls() && parent_list->getNumControls() > 0 && shapes.size() == parent_list->getNumControls())
				parent_list->getControl(shapes.size()-1-i)->setBackgroundColor(ofColor(0,0,0,0));
		}
		for (unsigned int j = 0; j < shapes[i].src.size(); j++){
			shapes[i].src[j].bBeingDragged = false;
		}
	}

	dragging_dst = false;

	return ofxBaseGui::mouseReleased(args);
}

bool ofx2DFormMapping::mouseScrolled(ofMouseEventArgs &args) {

	if(!direct_edit && mapping_rect_dst.inside(ofGetMouseX(), ofGetMouseY())) {
		setZoomFactor(args.scrollY);
		setNeedsRedraw();
	}

	return ofxBaseGui::mouseScrolled(args);
}

void ofx2DFormMapping::setMappingBackground(ofFbo_ptr &fbo) {
	mapping_bg = fbo;
}

void ofx2DFormMapping::setEditMode(bool direct_edit) {
	this->direct_edit = direct_edit;
	if(direct_edit) {
		setZoomFactor(0);
	}
	rebuild();
}

void ofx2DFormMapping::setOutputForm(float x, float y, float w, float h) {
	if(x != mapping_rect_output.x
			|| y != mapping_rect_output.y
			|| w != mapping_rect_output.width
			|| h != mapping_rect_output.height) {
		mapping_rect_output = ofRectangle(x,y,w,h);
		rebuild();
	}
}

void ofx2DFormMapping::setOutputForm(ofRectangle rect) {
	mapping_rect_output = rect;
}

void ofx2DFormMapping::setZoomFactor(int factor) {

	int old_zoom_factor = zoom_factor;

	zoom_factor += factor;
	if(zoom_factor < 0)
		zoom_factor = 0;

	ofPoint zoom_point_old = zoom_point;

	ofPoint tmp_zoom_point;
	tmp_zoom_point.x = ofGetMouseX() - mapping_rect_dst.x - zoom_point_offset.x;
	tmp_zoom_point.y = ofGetMouseY() - mapping_rect_dst.y - zoom_point_offset.y;

	ofVec2f diff = tmp_zoom_point - zoom_point_old;

	if(old_zoom_factor == 0) {
		diff = ofPoint(0,0);
		zoom_point_offset = ofPoint(0,0);
		zoom_point_old = tmp_zoom_point;
	}

	zoom_point = zoom_point_old + removeZoom(diff);
	zoom_point_offset += tmp_zoom_point - zoom_point;
	zoom_point_scaled = addZoom(zoom_point);

}

ofPoint ofx2DFormMapping::addZoom(ofPoint p) {
	return p*(1+zoom_factor*zoom_speed);
}

ofPoint ofx2DFormMapping::addZoomRelativeOfDstRect(ofPoint p) {
	return addZoom(p-mapping_rect_dst.getPosition())+mapping_rect_dst.getPosition()+translation_dst;
}

ofPoint ofx2DFormMapping::removeZoomRelativeOfDstRect(ofPoint p) {
	return removeZoom(p-mapping_rect_dst.getPosition()-translation_dst)+mapping_rect_dst.getPosition();
}

float ofx2DFormMapping::addZoom(float p) {
	return p*(1+zoom_factor*zoom_speed);
}


ofPoint ofx2DFormMapping::removeZoom(ofPoint p) {
	return p/(1+zoom_factor*zoom_speed);
}

void ofx2DFormMapping::onResized(ResizeEventArgs&){
	rebuild();
}

void ofx2DFormMapping::onMoved(MoveEventArgs&){
	rebuild();
}

ofParameter<bool>& ofx2DFormMapping::getShowSource(){
	return show_source;
}

ofx2DFormMapping::~ofx2DFormMapping(){
	unregisterMouseEvents(-2);
	ofRemoveListener(this->resize, this, &ofx2DFormMapping::onResized);
	ofRemoveListener(this->move, this, &ofx2DFormMapping::onMoved);
}
