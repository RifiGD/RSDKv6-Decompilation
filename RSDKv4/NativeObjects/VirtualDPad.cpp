#include "RetroEngine.hpp"

void VirtualDPad_Create(void *objPtr)
{
    RSDK_THIS(VirtualDPad);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    float screenXCenter = SCREEN_CENTERX;
    float screenYCenter = SCREEN_CENTERY;
#if !RETRO_USE_V6
    self->moveX         = saveGame->vDPadX_Move - screenXCenter;
#else
    if (Engine.gameType != GAME_SONICCD){
        self->moveX         = saveGame->vDPadX_Move - screenXCenter;
    }
    else {
        self->moveX         = saveGame->vDPad_CD - screenXCenter;
    }
#endif
#if !RETRO_USE_V6
    self->moveY         = -(saveGame->vDPadY_Move - screenYCenter);
#else
    if (Engine.gameType != GAME_SONICCD){
        self->moveY         = -(saveGame->vDPadY_Move - screenYCenter);
    }
    else {
        self->moveY         = -(saveGame->vDPad_CD - screenYCenter);
    }
#endif
#if !RETRO_USE_V6
    self->jumpX         = saveGame->vDPadX_Jump + screenXCenter;
#else
    if (Engine.gameType != GAME_SONICCD){
        self->jumpX         = saveGame->vDPadX_Jump + screenXCenter;
    }
    else {
        self->jumpX         = saveGame->vDPad_CD + screenXCenter;
    }
#endif
    self->pauseY        = 104.0f;
#if !RETRO_USE_V6
    self->jumpY         = -(saveGame->vDPadY_Jump - screenYCenter);
#else
    if (Engine.gameType != GAME_SONICCD){
        self->jumpY         = -(saveGame->vDPadY_Jump - screenYCenter);
    }
    else {
        self->jumpY         = -(saveGame->vDPad_CD - screenYCenter);
    }
#endif
    self->pauseX        = screenXCenter - 76.0f;
    self->pauseX_S      = screenXCenter - 52.0f;
    self->moveFinger    = -1;
    self->jumpFinger    = -1;
#if !RETRO_USE_V6
    float dpadSize            = saveGame->vDPadSize * (1 / 256.0f);
#else
    float dpadSize            = saveGame->vDPadSize * (1 / 256.0f);
    if (Engine.gameType == GAME_SONICCD){
        dpadSize = saveGame->vDPad_CD * (1 / 256.0f);
    }
#endif
    self->moveSize            = dpadSize;
    self->jumpSize            = dpadSize;
    self->pressedSize         = dpadSize * 0.85;
    self->useTouchControls    = GetGlobalVariableID("options.touchControls");
    self->usePhysicalControls = GetGlobalVariableID("options.physicalControls");
    self->vsMode              = GetGlobalVariableID("options.vsMode");
    self->textureID           = LoadTexture("Data/Game/Menu/VirtualDPad.png", TEXFMT_RGBA8888);
}
void VirtualDPad_Main(void *objPtr)
{
    RSDK_THIS(VirtualDPad);
    SaveGame *saveGame = (SaveGame *)saveRAM;

    if (globalVariables[self->useTouchControls] && (!globalVariables[self->usePhysicalControls] || self->editMode)) {
        if (self->alpha < saveGame->vDPadOpacity) {
            self->alpha += 4;
            if (self->pauseAlpha < 0xFF) {
                self->pauseAlpha = (self->alpha << 8) / saveGame->vDPadOpacity;
            }
        }
    }
    else {
        if (self->alpha > 0) {
            self->alpha -= 4;
            self->pauseAlpha = (self->alpha << 8) / saveGame->vDPadOpacity;
        }
    }

    if (self->alpha > 0) {
        SetRenderBlendMode(RENDER_BLEND_ALPHA);
        RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 128.0, 128.0, 256.0, 256.0, 0.0, 0.0, self->alpha,
                    self->textureID);

        if (self->alpha != saveGame->vDPadOpacity) {
            self->offsetX = 0.0;
            self->offsetY = 0.0;
        }
        else if (keyDown.up) {
            RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 128.0, 128.0, 256.0, 120.0, 256.0, 256.0, self->alpha,
                        self->textureID);

            self->offsetX = 0.0;
            self->offsetY = 20.0;
        }
        else if (keyDown.down) {
            RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 128.0, -8.0, 256.0, 120.0, 256.0, 392.0, self->alpha,
                        self->textureID);

            self->offsetX = 0.0;
            self->offsetY = -20.0;
        }
        else if (keyDown.left) {
            RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, 128.0, 128.0, 120.0, 256.0, 256.0, 256.0, self->alpha,
                        self->textureID);

            self->offsetX = 20.0;
            self->offsetY = 0.0;
        }
        else if (keyDown.right) {
            RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, -8.0, 128.0, 120.0, 256.0, 392.0, 256.0, self->alpha,
                        self->textureID);

            self->offsetX = -20.0;
            self->offsetY = 0.0;
        }
        else {
            self->offsetX = 0.0;
            self->offsetY = 0.0;
        }

        self->pivotX += (self->offsetX - self->pivotX) * 0.25f;
        self->pivotY += (self->offsetY - self->pivotY) * 0.25f;
        RenderImage(self->moveX, self->moveY, 160.0, self->moveSize, self->moveSize, self->pivotX + 84.0, self->pivotY + 84.0, 168.0, 168.0, 16.0,
                    328.0, self->alpha, self->textureID);
        RenderImage(self->jumpX, self->jumpY, 160.0, self->pressedSize, self->pressedSize, 128.0, 128.0, 256.0, 256.0, 256.0, 0.0, self->alpha,
                    self->textureID);

        float size = 0.0f;
        if (self->alpha == saveGame->vDPadOpacity && (keyDown.C || keyDown.A || keyDown.B))
            size = self->pressedSize;
        else
            size = self->jumpSize;
        RenderImage(self->jumpX, self->jumpY, 160.0, size, size, 84.0, 83.0, 168.0, 168.0, 16.0, 328.0, self->alpha, self->textureID);

        if (Engine.gameMode == ENGINE_MAINGAME) {
            if (!globalVariables[self->vsMode]) {
                if (activeStageList == STAGELIST_SPECIAL)
                    RenderImage(self->pauseX_S, self->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, self->pauseAlpha,
                                self->textureID);
                else
                    RenderImage(self->pauseX, self->pauseY, 160.0, 0.25, 0.25, 32.0, 32.0, 64.0, 64.0, 160.0, 258.0, self->pauseAlpha,
                                self->textureID);
            }
        }
    }
}