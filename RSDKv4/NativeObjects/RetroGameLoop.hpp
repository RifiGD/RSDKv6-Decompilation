#ifndef NATIVE_RETROGAMELOOP_H
#define NATIVE_RETROGAMELOOP_H

struct NativeEntity_RetroGameLoop : NativeEntityBase {
    // Nothin lol
};

void RetroGameLoop_Create(void *objPtr);
void RetroGameLoop_Main(void *objPtr);

#if RETRO_USE_V6
enum JavaPauseMenuStates{
    JAVAPAUSEMENU_STATE_RESUME    = 0,
    JAVAPAUSEMENU_STATE_RESTART   = 1,
    JAVAPAUSEMENU_STATE_EXIT      = 3,
};

void eventPauseMenuVisible(bool paused, int state); // not sure about the name "state", best i could come up with based on the output
void showPauseScreenJava(); // as much as im gonna get without decompiling the classes files in the apk

// this function is actually never used by the engine, it's called by JNI
// but it kind of relates to the other 2 functions above so imma declare it here
bool restartBtnUnAvailable();

#endif

#endif // !NATIVE_RETROGAMELOOP_H
