# teensyquencer
![Version 1.9](https://img.shields.io/badge/warning-work_in_progress-brightgreen.svg?label=warning&colorA=555555&colorB=ff4a88)

Experimental multi-channel sequencer for teensy
* each channel has a number of patterns. each pattern has its own length
* setting next pattern on a channel will take effect once current pattern on that channel is complete.
* Im using an akai apc40 controller to select patterns for channels - please fork and adapt to your own controller...
* comming soon - star this repo if you want to see these features
  * ~~midi import~~ and export
  * ~~playing samples at different pitches~~
  * parameter, midi, audio recording and playback
 
![Audio signal path](AudioPath.png)

## dependencies
* [Audio library](https://github.com/PaulStoffregen/Audio)
* [Avariable playback rate for teensy audio library](https://github.com/newdigate/teensy-variable-playback)

[![teensy-quencer on youtube](https://img.youtube.com/vi/eEeuXfxMdCI/0.jpg)](https://www.youtube.com/watch?v=eEeuXfxMdCI)

## code
* define multisequencer:
``` c
tempo tempo(120.0f);
multisequencer multisequencer(tempo);
midisequenceadapter adapter(multisequencer); // to read midi files into the a sequencer pattern
```

* add sequencer channel:
``` c
  sequencer *sequencer = multisequencer.newSequencer();
```

* add event callbacks:
``` c
  sequencer->onevent = [] (sequencerevent *event) {
      switch(event->channel) {
        case 0: triggerAudioEvent(event, playSdRaw1, "KICK.WAV"); break;
        case 1: triggerAudioEvent(event, playSdRaw2, "SNARE.WAV"); break;      
        default: break;
      }
  };
```
* add pattern:
``` c

  //                        !               !               !               !      
  const char* pattern4x4 = "X...X...X...X...X...X...X...X...X...X...X...X...X...X...X...X..."; 
  const char* patternSnr = "....x.......x.......x.......x.......x.......x.......x.......x..."; 

  int pattern = sequencer->addPattern(4); // empty pattern: 4 bars;
  pattern = sequencer->addPattern(4); // kick solo 4 x 4: 4 bars
  readPattern(0, pattern, pattern4x4, sequencer); //kick

  pattern = sequencer->addPattern(4); // kick solo 4 x 4 + snare: 4 bars;
  readPattern(0, pattern, pattern4x4, sequencer); //kick
  readPattern(1, pattern, patternSnr, sequencer); //snare
  
  // load 'snare.mid' smf into next pattern
  adapter.loadMidi("snare.mid");
  adapter.loadMidifileToNextChannelPattern(0, 0, 8);  // multisequencer channel number=0, midi track number, 8 bars long
  adapter.close();
  
  adapter.loadMidi("dredbass.mid");
  adapter.loadMidifileToNextChannelPattern(0, 0, 128, 24); // multisequencer channel number=0, midi track number, 128 bars long, transpose 24 notes
  adapter.close();

```

## compiling for teensy
You will need to add c++ std library to your compiler flags in boards.txt (Teensyduino/Java/hardware/teensy/avr/boards.txt)
* remove -nostdlib from teensy40.build.flags.common
* add -lstdc++ to teensy40.build.flags.cpp
```
teensy40.build.flags.common=-g -Wall -ffunction-sections -fdata-sections
teensy40.build.flags.cpp=-std=gnu++14 -fno-exceptions -fpermissive -fno-rtti -fno-threadsafe-statics -felide-constructors -Wno-error=narrowing -lstdc++
```

