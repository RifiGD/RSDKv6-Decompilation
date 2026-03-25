#ifndef NATIVE_CWSPLASH_H
#define NATIVE_CWSPLASH_H

#if !RETRO_USE_V6
enum CWSplashStates { CWSPLASH_STATE_ENTER, CWSPLASH_STATE_EXIT, CWSPLASH_STATE_SPAWNTITLE };
#else
enum CWSplashStates { CWSPLASH_STATE_ENTER, CWSPLASH_STATE_EXIT, CWSPLASH_STATE_SPAWNTITLE, CWSPLASH_STATE_MENUCONTROL_ACTION = 8 };
#endif
struct NativeEntity_CWSplash : NativeEntityBase {
    CWSplashStates state;
    float rectAlpha;
    byte textureID;
};

void CWSplash_Create(void *objPtr);
void CWSplash_Main(void *objPtr);

#if RETRO_USE_V6
// actual function in v6 btw, very needed with the way it skips title screen
void loadTextureAll();
#endif

#endif // !NATIVE_CWSPLASH_H
