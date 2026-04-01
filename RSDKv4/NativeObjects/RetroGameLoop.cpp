#include "RetroEngine.hpp"

#if !RETRO_USE_V6
void InitPauseMenu()
{
    PauseSound();
    ClearNativeObjects();
    CREATE_ENTITY(MenuBG);
    CREATE_ENTITY(PauseMenu);
}
#else
void InitPauseMenu()
{   
#if RETRO_USE_ORIGINAL_CODE
    Engine.gameMode = ENGINE_INITJAVAPAUSE;
#else
    // well uhh what's the point of having a Java pause menu if you can't even use it in the first place
    PauseSound();
    ClearNativeObjects();
    CREATE_ENTITY(MenuBG);
    CREATE_ENTITY(PauseMenu);
#endif
}
#endif

#if RETRO_USE_V6
void showPauseScreenJava()
{
    //it's Java sided, i'm not even gonna bother with this
    PrintLog("Showing the Java Pause Menu");
}

void eventPauseMenuVisible(bool paused, int state)
{
    if (paused){
        //pretty much InitPauseMenu but without creating the entities
        mixFiltersOnJekyll = false;
        PauseSound();
        ClearNativeObjects();
        return;
    }
    if (state == JAVAPAUSEMENU_STATE_EXIT){ // == 3
        Engine.gameMode = ENGINE_RESETGAME;
        if ((GetGlobalVariableByName("options.gameMode"))< 2){
            Engine.gameMode = ENGINE_ENDGAME;
        }
        SetGlobalVariableByName("timeAttack.result", 1000000);
    }
    else{
        if (state != JAVAPAUSEMENU_STATE_RESTART){ //!= 1
            if (state == JAVAPAUSEMENU_STATE_RESUME){
                mixFiltersOnJekyll = true;
                RenderRetroBuffer(0x40,0x43200000);
                ClearNativeObjects();
            // this means that even if the build was compiled for MOBILE,
            // the VirtualDPad object will never return | sad ://
                CreateNativeObject(RetroGameLoop_Create, RetroGameLoop_Main);
                ResumeSound();
                Engine.gameMode = ENGINE_MAINGAME;
                return;
            }
        return;
        }
        stageMode = STAGEMODE_LOAD;
        Engine.gameMode = ENGINE_EXITPAUSE;
        if ((GetGlobalVariableByName("options.gameMode")) < 2){
            SetGlobalVariableByName("player.lives", (GetGlobalVariableByName("player.lives") - 1)); //the decompiler output tempGlobalVar + -1, which is the exact thing
        }
        if (activeStageList == STAGELIST_REGULAR){
            SetGlobalVariableByName("lampPostID", 0);
            SetGlobalVariableByName("starPostID", 0);
        }
    ResetCurrentStageFolder();
    }
    CreateNativeObject(FadeScreen_Create, FadeScreen_Main);
    return;
}

bool restartBtnUnAvailable()
{
    // i usually dont put global variables in c++ variables like this
    // but uhh... you'll see later why i did this
    int lives = GetGlobalVariableByName("player.lives");
    int scriptGameMode = GetGlobalVariableByName("options.gameMode");
    int attractMode = GetGlobalVariableByName("options.attractMode");
    int vsMode = GetGlobalVariableByName("options.vsMode");

    // WHO THE FUCK WROTE THIS LOGIC AGHHHHH
    if (Engine.gameType != GAME_SONICCD || ( (activeStageList & ~STAGELIST_REGULAR) != STAGELIST_BONUS && (activeStageList != STAGELIST_REGULAR || stageListPosition < 0x51) )) 
    {
        int value; // temporary

        if (lives >= 2)
            value = lives - 1;
        else
            value = scriptGameMode - 2;

        if (value >= 0 && vsMode != 1 && attractMode != 1) {
            return activeStageList == STAGELIST_PRESENTATION
                || scriptGameMode == 2;
        }
    }

    return true;
}
#endif

void RetroGameLoop_Create(void *objPtr) { mixFiltersOnJekyll = Engine.useHighResAssets; }
void RetroGameLoop_Main(void *objPtr)
{
    switch (Engine.gameMode) {
        case ENGINE_DEVMENU:
#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
#endif

            ProcessStageSelect();
            TransferRetroBuffer();
            RenderRetroBuffer(64, 160.0);
            break;

        case ENGINE_MAINGAME:
#if RETRO_HARDWARE_RENDER
            gfxIndexSize        = 0;
            gfxVertexSize       = 0;
            gfxIndexSizeOpaque  = 0;
            gfxVertexSizeOpaque = 0;
            vertexSize3D        = 0;
            indexSize3D         = 0;
            render3DEnabled     = false;
#endif
            ProcessStage();
            TransferRetroBuffer();
            RenderRetroBuffer(64, 160.0);
            break;

        case ENGINE_INITDEVMENU:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            InitDevMenu();
            ResetCurrentStageFolder();
            break;

        case ENGINE_WAIT: break;

        case ENGINE_SCRIPTERROR:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            InitErrorMessage();
            ResetCurrentStageFolder();
            break;

        case ENGINE_INITPAUSE:
            mixFiltersOnJekyll = false;
            InitPauseMenu();
            break;

        case ENGINE_EXITPAUSE:
            Engine.gameMode = ENGINE_MAINGAME;
            ResumeSound();
            TransferRetroBuffer();
            break;

        case ENGINE_ENDGAME:
            ClearScreen(1);
            TransferRetroBuffer();
#if !RETRO_USE_ORIGINAL_CODE
            if (skipStartMenu) {
                activeStageList   = 0;
                stageListPosition = 0;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
            }
            else {
                RestoreNativeObjects();
                Engine.LoadGameConfig("Data/Game/GameConfig.bin");
                activeStageList   = 0;
                stageListPosition = 0;
            }
#else
            RestoreNativeObjects();
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            activeStageList   = 0;
            stageListPosition = 0;
#endif
            break;

        case ENGINE_RESETGAME: // Also called when 2P VS disconnects
            ClearScreen(1);
            TransferRetroBuffer();
#if !RETRO_USE_ORIGINAL_CODE
            if (skipStartMenu) {
                activeStageList   = 0;
                stageListPosition = 0;
                stageMode         = STAGEMODE_LOAD;
                Engine.gameMode   = ENGINE_MAINGAME;
            }
            else
                RestoreNativeObjects();
#else
            RestoreNativeObjects();
#endif
            break;
        
#if RETRO_USE_V6
        case ENGINE_INITJAVAPAUSE: //new gameMode state in RSDKv6
        eventPauseMenuVisible(true, 0);
        showPauseScreenJava();
        Engine.gameMode = 5; 
        return;
#endif

#if !RETRO_USE_ORIGINAL_CODE && RETRO_USE_NETWORKING
        case ENGINE_CONNECT2PVS: {
            CREATE_ENTITY(MultiplayerScreen)->bg = CREATE_ENTITY(MenuBG);
            NativeEntity_FadeScreen *fade        = CREATE_ENTITY(FadeScreen);
            fade->state                          = FADESCREEN_STATE_FADEIN;
            fade->delay                          = 1.5;
            fade->fadeSpeed                      = 1.0;
            Engine.gameMode                      = ENGINE_WAIT2PVS;
            break;
        }
        case ENGINE_WAIT2PVS:
            // wait for vs response
            if (dcError)
                CREATE_ENTITY(MultiplayerHandler);
            break;

#endif
#if RETRO_USE_MOD_LOADER
        case ENGINE_INITMODMENU:
            Engine.LoadGameConfig("Data/Game/GameConfig.bin");
            InitDevMenu();

            ResetCurrentStageFolder();

            SetupTextMenu(&gameMenu[0], 0);
            AddTextMenuEntry(&gameMenu[0], "MOD LIST");
            SetupTextMenu(&gameMenu[1], 0);
            InitMods(); // reload mods

            char buffer[0x100];
            for (int m = 0; m < modList.size(); ++m) {
                StrCopy(buffer, modList[m].name.c_str());
                StrAdd(buffer, ": ");
                StrAdd(buffer, modList[m].active ? "  Active" : "Inactive");
                AddTextMenuEntry(&gameMenu[1], buffer);
            }

            gameMenu[1].alignment      = MENU_ALIGN_RIGHT;
            gameMenu[1].selectionCount = 3;
            gameMenu[1].selection1     = 0;
            if (gameMenu[1].rowCount > 18)
                gameMenu[1].visibleRowCount = 18;
            else
                gameMenu[1].visibleRowCount = 0;

            gameMenu[0].alignment        = MENU_ALIGN_CENTER;
            gameMenu[0].selectionCount   = 1;
            gameMenu[1].timer            = 0;
            gameMenu[1].visibleRowOffset = 0;
            stageMode                    = DEVMENU_MODMENU;
            break;
#endif
        default:
            PrintLog("GameMode '%d' Called", Engine.gameMode);
            activeStageList   = 0;
            stageListPosition = 0;
            stageMode         = STAGEMODE_LOAD;
            Engine.gameMode   = ENGINE_MAINGAME;
            break;
    }
}
