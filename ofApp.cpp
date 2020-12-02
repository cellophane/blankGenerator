#include "ofApp.h"
#include "json.hpp"

#include "pack2.h"
#include <map>
using namespace std;
using json = nlohmann::json;
//--------------------------------------------------------------
void ofApp::setup(){
	gui.setup();
	backgroundColor = ofColor(255, 255, 255);
	font.setColor(ofColor(0, 0, 255));
	loadedJson = false;
	packed = false;
}

//--------------------------------------------------------------
void ofApp::update(){
	
}
void ofApp::drawNonOverlapping() {
	ofFbo offscreen;
	
	for (pack2::bin_t b : E.bins()) {
		if (b->contents().size() > 0) {
			

			ofClear(255, 255, 255, 0);
			float width = (float)(b->sizX()) * 72. / 100.;
			float height = (float)(b->sizY()) * 72. / 100;
			offscreen.allocate(width, height);
			offscreen.begin();
			ofBeginSaveScreenAsPDF(b->userID() + "_" + to_string((b->sizX() + 25)) + "x" + to_string((b->sizY() + 25)) + ".pdf");
			ofClear(255, 255, 255, 0);
			ofSetColor(0, 255, 0);
			ofNoFill();
			ofDrawRectangle(0, 0, width + 18, height + 18);

			map<int, vector<pair<int, int>>> xmap;
			map<int, vector<pair<int, int>>> ymap;
			for (auto recs : b->contents()) {

				float X = (float)(recs->locX()) * 72. / 100. + 9;
				float Y = (float)(recs->locY()) * 72. / 100. + 9;
				float width = (float)(recs->sizX()) * 72. / 100.;
				float height = (float)(recs->sizY()) * 72. / 100;
				if (recs->isSpun()) {
					font.draw(recs->userID(), X + 1, Y + 1, .25, false, 90.);
				}
				else {
					font.draw(recs->userID(), X + 1, Y - 1 + height, .25, false);
				}
				int x = (recs->locX());
				int y = (recs->locY());
				int w = (recs->sizX());
				int h = (recs->sizY());
				pair<int,int> horizontal = make_pair(x, x + w);
				pair<int, int> vertical = make_pair(y, y + h);
				//xmap is a map of horizontal line segments by y value
				//ymap is similar
				if (xmap.count(y)) {
					xmap[y].push_back(horizontal);
				}
				else {
					xmap[y] = vector<pair<int, int>>();
					xmap[y].push_back(horizontal);
				}
				if (xmap.count(y+h)) {
					xmap[y+h].push_back(horizontal);
				}
				else {
					xmap[y+h] = vector<pair<int, int>>();
					xmap[y+h].push_back(horizontal);
				}

				if (ymap.count(x)) {
					ymap[x].push_back(vertical);
				}
				else {
					ymap[x] = vector<pair<int, int>>();
					ymap[x].push_back(vertical);
				}

				if (ymap.count(x+w)) {
					ymap[x+w].push_back(vertical);
				}
				else {
					ymap[x+w] = vector<pair<int, int>>();
					ymap[x+w].push_back(vertical);
				}
			}
			ofLog() << "Bin " << b->userID();
			ofLog()<< to_string(size(xmap));
			ofLog() << to_string(size(ymap));
			map<int, vector<pair<int, int>>>::iterator it;
			for (it = xmap.begin(); it != xmap.end(); it++) {
				if(size(it->second)>1){
				sort((it->second).begin(),(it->second.end()));
				}
			}
			for (it = ymap.begin(); it != ymap.end(); it++) {
				if (size(it->second) > 1) {
					sort((it->second).begin(), (it->second.end()));
				}
			}
			vector <pair<int, pair<int, int>>> hsegments;
			vector <pair<int, pair<int, int>>> vsegments;
			int x1, x2 = 0;
			for (it = xmap.begin(); it != xmap.end(); it++) {
				bool first = true;
				int y = it->first;
				for (auto seg : it->second) {
					if (first) {
						x1 = seg.first;
						x2 = seg.second;
						first = false;
					}
					else {
						if (seg.first <= x2) {
							x2 = seg.second;
						}
						else {
							hsegments.push_back(make_pair(y,make_pair(x1, x2)));
							x1 = seg.first;
							x2 = seg.second;

						}
					}

				}
				hsegments.push_back(make_pair(y, make_pair(x1, x2)));

			}
			int y1, y2 = 0;
			for (it = ymap.begin(); it != ymap.end(); it++) {
				bool first = true;
				int x = it->first;
				if (size(it->second) > 1) {
					for (auto seg : it->second) {
						if (first) {
							y1 = seg.first;
							y2 = seg.second;
							first = false;
						}
						else {
							if (seg.first <= y2) {
								y2 = seg.second;
							}
							else {
								vsegments.push_back(make_pair(x, make_pair(y1, y2)));
								y1 = seg.first;
								y2 = seg.second;

							}
						}

					}
					vsegments.push_back(make_pair(x, make_pair(y1, y2)));
				}
				else {
					x = it->first;
					y1 = (it->second[0]).first;
					y2 = (it->second[0]).second;
					vsegments.push_back(make_pair(x, make_pair(y1, y2)));
				}
			}
			ofSetColor(255, 0, 0);
			for (auto seg : hsegments) {
				float y = seg.first * 72. / 100.+9;
				float x1 = seg.second.first * 72. / 100.+9;
				float x2 = seg.second.second * 72. / 100.+9;
				ofLog() << "Drawing horizontal: " << to_string(y) << "," << to_string(x1) << "," << to_string(x2);
				ofDrawLine(x1, y, x2, y);
			}
			for (auto seg : vsegments) {
				float x = seg.first * 72. / 100.+9;
				float y1 = seg.second.first * 72. / 100.+9;
				float y2 = seg.second.second * 72. / 100.+9;
				ofLog() << "Drawing vertical: " << to_string(x) << "," << to_string(y1) << "," << to_string(y2);
				ofDrawLine(x, y1, x, y2);
			}
			ofEndSaveScreenAsPDF();
			offscreen.clear();
			offscreen.end();

		}
	}
}
float ofApp::drawPacked(int index) {
	float totalArea = 0;
	float usedArea = 0;
	pack2::bin_t b = E.bins()[index];
		if (b->contents().size() > 0) {
			ofClear(255, 255, 255, 0);
			float width = (float)(b->sizX()) * 72. / 100.;
			float height = (float)(b->sizY()) * 72. / 100;
			ofClear(255, 255, 255, 0);
			ofSetColor(0, 255, 0);
			ofNoFill();
			ofDrawRectangle(0, 0, (width+18)/5, (height+18)/5);
			//ofLog() << to_string(b->userID);
			totalArea = width * height;
			
			for (auto recs : b->contents()) {
				float x = (float)(recs->locX()) * 72. / 100.+6;
				float y = (float)(recs->locY()) * 72. / 100.+6;
				float width = (float)(recs->sizX()) * 72. / 100.;
				float height = (float)(recs->sizY()) * 72. / 100;
				usedArea += width * height;
				if (recs->isSpun()) {
					font.draw(recs->userID(),(x + 1)/5, (y + 1)/5,.5, false, 90.);
				}
				else {
					font.draw(recs->userID(), (x + 1)/5, (y -1)/5+height/5, .5, false);
				}
				ofSetColor(255, 0, 0);
				ofDrawRectangle(x/5, y/5, width/5, height/5);
				
			}
			
		}
		if(totalArea>0){
		return usedArea / totalArea;
		}
		else {
			return 0;
		}
	}
	
	
	
	


//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(backgroundColor);
	gui.begin();
	{
		if (loadedJson) {
			static int currentCustomIndex = 0;
			if (ofxImGui::VectorListBox("blanks",&currentCustomIndex,blankIDs)) {
				ofLog() << " VectorListBox FILE PATH: " << files[currentCustomIndex].getAbsolutePath();
			}
		}
		
		if (ImGui::Button("Load Customs")) {
			loadJson();
		}
		if(size(blankIDs)>0){
			ImGui::SameLine();
			
		if (ImGui::Button("Clear Customs")) {
			fileNames=std::vector<std::string>() ;
			files = std::vector<ofFile>();
			blankIDs = std::vector<std::string>();
		}
		}
		static int currentBoardIndex = 0;
		if (!boards.empty()) {
			if (ofxImGui::VectorListBox("boards", &currentBoardIndex, boardStrings)) {
				ofLog() << " Blank index " << to_string(currentBoardIndex);
			}
		}

		if (ImGui::Button("Load Boards")) {
			loadBoards();
		}

		if (addBoard) {
			makeBoard(false);
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Board")) {
			addBoard = true;
			makeBoard(true);
		}
		
		if (size(boards) > 0) {
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				if (currentBoardIndex < boards.size()) {
					boards.erase(boards.begin() + currentBoardIndex);
					boardStrings.erase(boardStrings.begin() + currentBoardIndex);
				}
			}
		}
		if (size(boards) > 0 && size(blankIDs) > 0) {
			if (ImGui::Button("Pack")) {
				packRects();
				packed = true;
			}

			if (packed) {
				static int currentPackedIndex = 0;
				if (!boards.empty()) {
					if (ofxImGui::VectorListBox("packed boards", &currentPackedIndex, packedStrings)) {
						ofLog() << " Blank index " << to_string(currentBoardIndex);
					}
				}
				float usedArea = drawPacked(currentPackedIndex);
				ImGui::Text("%.3f",usedArea);
				if (ImGui::Button("export")) {
					drawNonOverlapping();
					
				}
			}
		}
		if (ImGui::Button("exit")) {

			ofExit();
		}

		
	}
	gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
void ofApp::loadJson() {
	ofFileDialogResult result = ofSystemLoadDialog("select input folder", true);
	if (result.bSuccess) {
		loadedJson = true;
		string inputPath = result.getPath();
		ofDirectory dataDirectory(inputPath);
		files = dataDirectory.getFiles();
		for (size_t i = 0; i < files.size(); i++)
		{
			if(files[i].getExtension()=="json"){
				fileNames.push_back(files[i].getFileName());
				string fname = files[i].getFileName();
				json j;
				std::ifstream i(files[i].getAbsolutePath());
				i >> j;
				i.close();
				int pos = fname.find(".");
				string idStr = fname.substr(0, pos);
				int id = stoi(idStr);
				float w = j["width"]+.3;
				float h = float(j["height"])+.125/2+.32;
				blankData.push_back(make_tuple(id, w, h));
				blankIDs.push_back(idStr+" " + to_string(w).substr(0,to_string(w).find(".")+2)+"x"+to_string(h).substr(0, to_string(h).find(".") + 2));
			}
		}
	}
}

void ofApp::loadBoards() {
	ofFileDialogResult result = ofSystemLoadDialog("select board file", false);
	if (result.bSuccess) {
		string inputFile = result.getPath();
		std::ifstream i(inputFile);
		string board;
		while (std::getline(i,board)) {
			int pos = board.find(",");
			float w = std::stof(board.substr(0, pos));
			float h = std::stof(board.substr(pos+1));
			boards.push_back(make_pair(w-.125*2, h-.125*2));
			string boardString = to_string(w).substr(0,to_string(w).find(".")+2) + "," + to_string(h).substr(0, to_string(h).find(".") + 2);
			boardStrings.push_back(boardString);
			ofLog() << " Loaded board: " << boardString;
		}
		i.close();
	}
}

void ofApp::makeBoard(bool first = false) {

	static float w;
	ImGui::InputFloat("width", &w, 4.0f, 32.0f, "%.2f");
	static float h;
	ImGui::InputFloat("height", &h, 4.0f, 32.0f, "%.2f");
	if (ImGui::Button("submit")) {
		boards.push_back(make_pair(w-.125*2, h-.125*2));
		string boardString = to_string(w).substr(0, to_string(w).find(".") + 2) + "," + to_string(h).substr(0, to_string(h).find(".") + 2);
		boardStrings.push_back(boardString);
		ofLog() << " Made board: " << boardString;
		addBoard = false;
	}
}
void ofApp::packRects() {
	//multiply all values by 100 and round
	int count = 0;
	//E.Algorithm().fMultipleFit = true;
	E.Algorithm().fTryEveryItemFirst = true;
	E.Algorithm().fThruCuts = false;
	for (auto board : boards) {
		pack2::bin_t b = pack2::bin_t(new pack2::cBin("Bin " + to_string(count), (int)(board.first * 100), (int)(board.second * 100)));
		
		E.add(b);
		count += 1;
	}

	SortBinsIntoIncreasingSize(E);
	for (auto blank : blankData) {
		int id = get<0>(blank);
		int w = (int)(get<1>(blank) * 100);
		int h = (int)(get<2>(blank) * 100);
		E.addItem(to_string(id), w, h);

	}
	for (auto i : E.items()) {
		i->spinEnable();
	}
	Pack(E);
	ofLog() << CSV(E);
	for(int ind = 0; ind<E.bins().size();++ind){
		packedIndices = std::vector<int>();
		pack2::bin_t b = E.bins()[ind];

		if (b->contents().size() > 0) {
			packedIndices.push_back(ind);
			string packedString = to_string(ind) + ": " + to_string(b->sizX()) + "," + to_string(b->sizY());
			packedStrings.push_back(packedString);
			//ofLog() << to_string(b->userID);
			for (auto recs : b->contents()) {
				ofLog() << (recs->userID());
				ofLog() << to_string(recs->locX()) << "," << to_string(recs->locY());
			}
		}
	}
}

