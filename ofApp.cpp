#include "ofApp.h"
#include "json.hpp"

#include "pack2.h"

using namespace std;
using json = nlohmann::json;
//--------------------------------------------------------------
void ofApp::setup(){
	gui.setup();
	backgroundColor = ofColor(114, 144, 154);
	loadedJson = false;
}

//--------------------------------------------------------------
void ofApp::update(){
	
}
void ofApp::drawPacked() {
	for (pack2::bin_t b : E.bins()) {
		ofFbo offscreen;
		offscreen.allocate(2304, 2304);
		offscreen.begin();
		if (b->contents().size() > 0) {
			ofClear(255, 255, 255, 0);
			ofBeginSaveScreenAsPDF(b->userID() + ".pdf");

			ofClear(255, 255, 255, 0);
			float width = (float)(b->sizX()) * 72. / 100.;
			float height = (float)(b->sizY()) * 72. / 100;
			ofSetColor(0, 255, 0);
			ofNoFill();
			ofDrawRectangle(0, 0, width, height);
			//ofLog() << to_string(b->userID);
			for (auto recs : b->contents()) {
				float x = (float)(recs->locX()) * 72. / 100.+6;
				float y = (float)(recs->locY()) * 72. / 100.+6;
				float width = (float)(recs->sizX()) * 72. / 100.;
				float height = (float)(recs->sizY()) * 72. / 100;
				if (recs->isSpun()) {
					font.draw(recs->userID(),x + 1, y + 1,.5, false, 90.);
				}
				else {
					font.draw(recs->userID(), x + 1, y -1+height, .5, false);
				}
				ofSetColor(255, 0, 0);
				ofDrawRectangle(x, y, width, height);
				ofLog() << to_string(recs->locX()) << "," << to_string(recs->locY());
			}
			ofEndSaveScreenAsPDF();
		}

		offscreen.clear();
		offscreen.end();
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
		ImGui::SameLine();
		if (ImGui::Button("Delete")) {
			if(currentBoardIndex<boards.size()){
			boards.erase(boards.begin()+currentBoardIndex);
			boardStrings.erase(boardStrings.begin()+currentBoardIndex);
			}
		}
		if (ImGui::Button("Pack")) {
			packRects();
		}
		if (ImGui::Button("draw")) {
			drawPacked();
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
				float w = j["width"];
				float h = j["height"];
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
			boards.push_back(make_pair(w, h));
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
		boards.push_back(make_pair(w, h));
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
	for (pack2::bin_t b : E.bins()) {
		if (b->contents().size() > 0) {
			//ofLog() << to_string(b->userID);
			for (auto recs : b->contents()) {
				ofLog() << (recs->userID());
				ofLog() << to_string(recs->locX()) << "," << to_string(recs->locY());
			}
		}
	}
}

