#include <iostream>
#include "SD/SD.h"
#include <dirent.h>
#include "../src/sequencer.h"
#include "../src/midireader.h"
#include "../src/midisequenceadapter.h"
#include <cstdio>
#include <pwd.h>
#include <unistd.h>
#include <math.h>

using namespace std;

void millis_test();
void delay_test();
void first_test();
void second_test();
void midireader_test();
void midisequenceadapter_test();
extern unsigned char midi_monophonic_mid[];
extern unsigned int midi_monophonic_len;

streampos readFileData(const string& filename, char *&buffer);

const char *getHomeDirectory();

int main(int argc, char **argv){
    std::cout << "starting app...\n";
    initialize_mock_arduino();
    midisequenceadapter_test();
    //midireader_test();
    //first_test();

    std::cout << "complete...\n";
}


void midisequenceadapter_test() {
    char *buffer = (char *)&midi_monophonic_mid[0];
    SD.setSDCardFileData(buffer, midi_monophonic_len);

    tempo tempo;
    multisequencer multisequencer(tempo);
    sequencer *sequencer = multisequencer.newSequencer();

    midisequenceadapter adapter(multisequencer);
    adapter.loadMidi("blah.mid");
    adapter.loadMidifileToNextChannelPattern(0, 0, 8);

    sequencer->writenoteslist(0);
    sequencer->writescore(0);
}


void first_test() {
    tempo tempo(120.0f);
    songposition *position = new songposition();
    sequencer sequencer(tempo, position);
    sequencer.onevent = [&] (sequencerevent *event) {
        Serial.printf("%d:%d:%4f\n", position->bar, position->beat, position->sixtyFourth / 64.0f);
        Serial.printf("----- %d:%d:%4f\n", event->channel, event->isNoteStartEvent, event->rate);
    };

    sequencer.addPattern(4);

    for (int i=0; i < 16; i++) {
      loopelement *kick = new loopelement();
      kick->rate = 1.0f;
      kick->start_tick = (i * 480);
      kick->stop_tick = (i * 480) + 479;
      kick->channel = 2;
      kick->loopType = looptypex_none;
      sequencer.addelement(0, kick);
  }

  for (int i=0; i < 16; i++) {
      loopelement *kick = new loopelement();
      kick->rate = 1.0f;
      kick->start_tick = ((i+2) * 480);
      kick->stop_tick = ((i+2) * 480) + 479;
      kick->channel = 3;
      kick->loopType = looptypex_none;
      sequencer.addelement(0, kick);
  }

  for (int i=0; i < 32; i++) {
      loopelement *snare = new loopelement();
      snare->rate = 1.0f;
      snare->start_tick = ((i+2) * 240);
      snare->stop_tick = ((i+2) * 240) + 239;
      snare->channel = 1;
      snare->loopType = looptypex_none;
      sequencer.addelement(0, snare);
  }

    sequencer.writescore(0);
    sequencer.writenoteslist(0);
  //return;
    sequencer.start(micros());
   // sequencer.tick(0);
   // sequencer.tick(250);
  //  sequencer.tick(500);
  //  sequencer.tick(750);
  //  sequencer.tick(1000);
  //  sequencer.tick(1250);
  //  sequencer.tick(1500);
/*
    while (true) {
       sequencer.tick(micros());
       //Serial.printf("%d:%d:%4f\n", position.bar, position.beat, position.sixtyFourth / 64.0f);
       delay(5);
    }
    for (int i=0; i<8; i++) {
       sequencer.tick(micros());
       delay(20);
    }
*/
 }

void second_test() {
    tempo tempo(120.0f);

    multisequencer multisequencer(tempo);

    sequencer *sequencer = multisequencer.newSequencer();
    
    sequencer->onevent = [&] (sequencerevent *event) {
        //Serial.printf("%d:%d:%4f\n", position.bar, position.beat, position.sixtyFourth / 64.0f);
        Serial.printf("----- %d:%d:%4f\n", event->channel, event->isNoteStartEvent, event->rate);
    };

    sequencer->addPattern(4);

    for (int i=0; i < 16; i++) {
      loopelement *kick = new loopelement();
      kick->rate = 1.0f;
      kick->start_tick = (i * 480);
      kick->stop_tick = (i * 480) + 479;
      kick->channel = 2;
      kick->loopType = looptypex_none;
      sequencer->addelement(0, kick);
  }

  for (int i=0; i < 16; i++) {
      loopelement *kick = new loopelement();
      kick->rate = 1.0f;
      kick->start_tick = ((i+2) *480);
      kick->stop_tick = ((i+2) * 480) + 479;
      kick->channel = 3;
      kick->loopType = looptypex_none;
      sequencer->addelement(0, kick);
  }

  for (int i=0; i < 32; i++) {
      loopelement *snare = new loopelement();
      snare->rate = 1.0f;
      snare->start_tick = ((i+2) * 240);
      snare->stop_tick = ((i+2) * 240) + 239;
      snare->channel = 1;
      snare->loopType = looptypex_none;
      sequencer->addelement(0, snare);
  }

    sequencer->writescore(0);
    sequencer->writenoteslist(0);

  //return;
    multisequencer.start(micros());
   // sequencer.tick(0);
   // sequencer.tick(250);
  //  sequencer.tick(500);
  //  sequencer.tick(750);
  //  sequencer.tick(1000);
  //  sequencer.tick(1250);
  //  sequencer.tick(1500);

    while (true) {
       multisequencer.tick(micros());
       //Serial.printf("%d:%d:%4f\n", position.bar, position.beat, position.sixtyFourth / 64.0f);
       delay(5);
    }
}

void search(const std::string& curr_directory, const std::string& extension, vector<string> &results){
	DIR* dir_point = opendir(curr_directory.c_str());
    if (!dir_point) {
        cout << "Directory not found!!! " << curr_directory << "\n";
        return;
    }
	dirent* entry = readdir(dir_point);
	while (entry){									// if !entry then end of directory
		if (entry->d_type == DT_DIR){				// if entry is a directory
			std::string fname = entry->d_name;
			if (fname != "." && fname != "..")
				search(entry->d_name, extension, results);	// search through it
		}
		else if (entry->d_type == DT_REG){		// if entry is a regular file
			std::string fname = entry->d_name;	// filename
      // if filename's last characters are extension
			if (fname.find(extension, (fname.length() - extension.length())) != std::string::npos)
				results.push_back(fname);		// add filename to results vector
		}
		entry = readdir(dir_point);
	}
	return;
}

void midireader_test() {
    std::vector<std::string> midiFiles;				// holds search results

    const char *homeDir = getHomeDirectory();

    string midiFileDirectory = string(homeDir) + "/Development/SD";
    search(midiFileDirectory,"mid", midiFiles);

    std::cout << midiFiles.size() << " files were found:" << std::endl;
    for (auto & midiFile : midiFiles) { // used unsigned to appease compiler warnings
        std::cout << midiFile << std::endl;

        char *buffer;
        std::string midiFileName = midiFileDirectory + "/" + midiFile;
        streampos bytesRead = readFileData(midiFileName, buffer);
        if (bytesRead > 0) {
            SD.setSDCardFileData(buffer, bytesRead);

            midireader reader;
            reader.open(midiFile.c_str());
            for (int i = 0; i < reader.getNumTracks(); i++) {
                midimessage message {};
                reader.setTrackNumber(i);
                while (reader.read(message)) {
                    //Serial.printf("status: %x, channel:%x\n", message.status, message.channel);
                }
                Serial.printf("track end: %d;\n", i);
            }
            Serial.printf("song end\n");
            delete buffer;
        }
    }
}

const char *getHomeDirectory() {
    const char *homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd)
           homeDir = pwd->pw_dir;
    }
    printf("Home directory is %s\n", homeDir);
    return homeDir;
}

streampos readFileData(const string& filename, char *&buffer) {
    fstream mockFile = fstream();
    mockFile.open(filename, ios_base::in | ios_base::binary | ios_base::ate);
    if (mockFile.is_open()) {
        streampos bytesRead = mockFile.tellg();
        buffer = new char[bytesRead];
        mockFile.seekg(0, ios_base::beg);
        mockFile.read(buffer, bytesRead);
        mockFile.close();
        return bytesRead;
    }
    return 0;
}

void millis_test() {
  unsigned long start = millis();
  cout << "millis() test start: " << start << endl;
  while( millis() - start < 10000 ) {
	cout << millis() << endl;
	delay(1);
  }
  unsigned long end = millis();
  cout << "End of test - duration: " << end - start << "ms" << endl;
}

void delay_test() {
  unsigned long start = millis();
  cout << "delay() test start: " << start << endl;
  while( millis() - start < 10000 ) {
	cout << millis() << endl;
	delay(250);
  }
  unsigned long end = millis();
  cout << "End of test - duration: " << end - start << "ms" << endl;
}

unsigned char midi_monophonic_mid[] = {
        0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01,
        0x01, 0xe0, 0x4d, 0x54, 0x72, 0x6b, 0x00, 0x00, 0x00, 0xe9, 0x00, 0xff,
        0x20, 0x01, 0x00, 0x00, 0xff, 0x03, 0x05, 0x73, 0x79, 0x6e, 0x74, 0x68,
        0x00, 0xff, 0x04, 0x14, 0x53, 0x74, 0x65, 0x69, 0x6e, 0x77, 0x61, 0x79,
        0x20, 0x47, 0x72, 0x61, 0x6e, 0x64, 0x20, 0x50, 0x69, 0x61, 0x6e, 0x6f,
        0x00, 0xff, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08, 0x00, 0xff, 0x59, 0x02,
        0x00, 0x00, 0x00, 0xff, 0x54, 0x05, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0x51, 0x03, 0x0a, 0x2c, 0x2b, 0x00, 0x90, 0x3c, 0x7f, 0x81, 0x1b,
        0x90, 0x3c, 0x00, 0x82, 0x45, 0x90, 0x3c, 0x7f, 0x81, 0x1b, 0x90, 0x3c,
        0x00, 0x82, 0x45, 0x90, 0x3c, 0x7f, 0x81, 0x1b, 0x90, 0x3c, 0x00, 0x82,
        0x45, 0x90, 0x3c, 0x7f, 0x81, 0x1b, 0x90, 0x3c, 0x00, 0x82, 0x45, 0x90,
        0x3c, 0x7f, 0x81, 0x1b, 0x90, 0x3c, 0x00, 0x82, 0x45, 0x90, 0x3c, 0x7f,
        0x81, 0x1b, 0x90, 0x3c, 0x00, 0x82, 0x45, 0x90, 0x3c, 0x7f, 0x81, 0x1b,
        0x90, 0x3c, 0x00, 0x82, 0x45, 0x90, 0x3d, 0x7f, 0x81, 0x1b, 0x90, 0x3d,
        0x00, 0x82, 0x45, 0x90, 0x38, 0x7f, 0x81, 0x1b, 0x90, 0x38, 0x00, 0x82,
        0x45, 0x90, 0x38, 0x7f, 0x81, 0x1b, 0x90, 0x38, 0x00, 0x82, 0x45, 0x90,
        0x38, 0x7f, 0x81, 0x1b, 0x90, 0x38, 0x00, 0x82, 0x45, 0x90, 0x38, 0x7f,
        0x81, 0x1b, 0x90, 0x38, 0x00, 0x82, 0x45, 0x90, 0x38, 0x7f, 0x81, 0x1b,
        0x90, 0x38, 0x00, 0x82, 0x45, 0x90, 0x38, 0x7f, 0x81, 0x1b, 0x90, 0x38,
        0x00, 0x82, 0x45, 0x90, 0x38, 0x7f, 0x81, 0x1b, 0x90, 0x38, 0x00, 0x82,
        0x45, 0x90, 0x37, 0x7f, 0x81, 0x1b, 0x90, 0x37, 0x00, 0x81, 0x98, 0x45,
        0xff, 0x2f, 0x00
};
unsigned int midi_monophonic_len = 255;