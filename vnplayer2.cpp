#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "tinyxml.h"
#include <unistd.h>
#include <libgen.h>
#include "engine/CCommon.h"
#include "engine/CScript.h"
#include "engine/CCurrentContext.h"

#define _VERBOSE_RENDER_NO

SDL_Texture* bgImageTexture = NULL;
SDL_Texture* testImageTexture = NULL;
SDL_Texture *olImageTexture[6] = {NULL,NULL,NULL,NULL,NULL,NULL}; // 6 overlays
SDL_Texture *laImageTexture[6] = {NULL,NULL,NULL,NULL,NULL,NULL}; // 6 layers
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
char *workingDirectory = NULL;
char filenameBuffer[1024];
CScript *script = NULL;
SDL_Color textColor = {255, 255, 255}; // White color
TTF_Font* font = NULL;
const char* FONT_FILE = "Open_Sans/static/OpenSans_SemiCondensed-Bold.ttf";
const int FONT_SIZE = 18;
const int TEXT_OFFSET = 60;
char *currentText = ""; 
char *currentMusicTrack = "";
const int BUTTON_HEIGHT = 60;
const int MIN_WINDOW_WIDTH = 800;
const int MIN_WINDOW_HEIGHT = 480;
int numChoicesDisplayed = 0; // this just counts the number of choices displayed. We calculate if we hit a button manually.
char *choices[4][255];
int nchoices = 0;

char* constructFilePath(const char* directory, const char* filename, char* buffer, size_t bufferSize);
char* completePath(const char* filename); // not thread safe, just uses workingDirectory & filenameBuffer
int loadScene(CScene *scene);
int loadScope(CScope *scope);
void onTap( int x, int y );
void onNextScene( );

typedef struct relativePosition {
    float x;
    float y;
    float w;
    float h;
} relativePosition;

char *constructFilePath(const char* directory, const char* filename, char* buffer, size_t bufferSize) {
    if (directory == NULL || filename == NULL || buffer == NULL || bufferSize == 0) {
        return NULL;
    }

    if (snprintf(buffer, bufferSize, "%s/resources/%s", directory, filename) < 0) {
        perror("snprintf");
        return NULL;
    }

    return buffer;
}

// not thread safe - constructs the complete file to resources for a file
//
char *completePath(const char* filename) {
    return constructFilePath(workingDirectory,filename,(char*)filenameBuffer,1024);
}

void loadScript(char *scriptFilename) {
    if (!scriptFilename)
        return;
    script = new CScript(completePath(scriptFilename));
}

void renderText(const char* text, int x, int y) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// with choice buttons the position is calculated automatically
//
void renderChoiceButton(const char* buttonText) {
    //printf("renderChoiceButton\n");
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    int y = ((windowHeight/2)-(BUTTON_HEIGHT*2))+(numChoicesDisplayed*(BUTTON_HEIGHT+20));
    SDL_Rect buttonRect = {TEXT_OFFSET, y, windowWidth-(TEXT_OFFSET*2), BUTTON_HEIGHT};

    // Draw a semi-transparent black rectangle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Alpha value of 128 (0.5 opacity)
    SDL_RenderFillRect(renderer, &buttonRect);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, buttonText, (SDL_Color){255, 255, 255, 255});
    renderText(buttonText,TEXT_OFFSET + (windowWidth-(TEXT_OFFSET*2) - textSurface->w) / 2,y + (BUTTON_HEIGHT - textSurface->h) / 2);
    numChoicesDisplayed++;
}

int onQuestionButton(int index) {
    
    nchoices = 0;
    script->mcurrentScene->rewind(); // refactor - remove such speciffics from the UI
    
    char *scopeid = NULL;
    if (script->mcurrentScene->mchoices) {
        TiXmlElement *echoice = script->mcurrentScene->choiseElementAtIndex(index);
        if (echoice)
        {
            if (echoice->Attribute("action"))
                scopeid = (char*)echoice->Attribute("action");
        }
    } else {
        printf("questionboxOnIndex error, data integrity\n");
        return -1;
    }
    
    if (!scopeid) {

        printf("questionboxOnIndex warning, couldn't figure out scope id of choice - continuing\n");
        onTap(0,0);
        return -1;
    }
        
    CScope *scope = script->getScopeAtId(scopeid);
    if (scope) {
        if (scope!=script->mcurrentScope) {
            //[self loadScope:scope];
            loadScope(scope);
        } else {
            onNextScene();
        }
    } else {
        onNextScene();
    }

    return 0;
}

int getTextWidth(const char* text, TTF_Font* font) {
    int textWidth, textHeight;
    if (TTF_SizeText(font, text, &textWidth, &textHeight) != 0) {
        fprintf(stderr, "TTF_SizeText error: %s\n", TTF_GetError());
        return -1; // Error
    }
    return textWidth;
}

void renderOverlay(SDL_Texture* olImageTexture, SDL_Rect destRect) {
    #ifdef _VERBOSE_RENDER
        printf("renderOverlay p(%p)\n",olImageTexture);
    #endif
    if (!olImageTexture)
        return;
    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    SDL_QueryTexture(olImageTexture, NULL, NULL, &srcRect.w, &srcRect.h);
    #ifdef _VERBOSE_RENDER
        printf("    srcRect(%dx%d+%d+%d) destRect(%dx%d+%d+%d)\n",
            srcRect.w,srcRect.h,srcRect.x,srcRect.y,
            destRect.w,destRect.h,destRect.x,destRect.y);
    #endif
    SDL_RenderCopy(renderer, olImageTexture, &srcRect, &destRect);
    #ifdef _VERBOSE_RENDER
        printf("renderOverlay finish\n");
    #endif
}

// render overlay by defining a relative position/sizing to the window
//
void renderOverlayRelative(SDL_Texture* olImageTexture, relativePosition position) {
    if (!olImageTexture)
        return;

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    SDL_QueryTexture(olImageTexture, NULL, NULL, &srcRect.w, &srcRect.h);

    SDL_Rect destRect;
    destRect.x = (int)(position.x * windowWidth);
    destRect.y = (int)(position.y * windowHeight);
    destRect.w = (int)(position.w * windowWidth);
    destRect.h = (int)(position.h * windowHeight);

    // Calculate aspect-fit scaling based on the relative position and size
    float imageAspectRatio = (float)srcRect.w / srcRect.h;
    float destAspectRatio = (float)destRect.w / destRect.h;

    if (imageAspectRatio > destAspectRatio) {
        // Image is wider than the destination rect
        destRect.h = (int)(destRect.w / imageAspectRatio);
    } else {
        // Image is taller than the destination rect
        destRect.w = (int)(destRect.h * imageAspectRatio);
    }

    destRect.x = destRect.x + (int)((position.w * windowWidth - destRect.w) / 2);
    destRect.y = destRect.y + (int)((position.h * windowHeight - destRect.h) / 2);

    renderOverlay(olImageTexture,destRect);
}

void redraw() {

    SDL_Rect srcRect, destRect;
    int windowWidth, windowHeight;
    srcRect.x = 0;
    srcRect.y = 0;
    SDL_QueryTexture(bgImageTexture, NULL, NULL, &srcRect.w, &srcRect.h);

    // Get the current window size
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Calculate aspect-fill scaling based on the current window size
    float imageAspectRatio = (float)srcRect.w / srcRect.h;
    float windowAspectRatio = (float)windowWidth / windowHeight;

    if (imageAspectRatio > windowAspectRatio) {
        // Image is wider than the window

        destRect.h = windowHeight;
        destRect.w = windowHeight*imageAspectRatio;
        destRect.x = -(destRect.w-windowWidth)/2;
        destRect.y = 0;

    } else {
        // Image is taller than the window

        destRect.h = windowWidth/imageAspectRatio;
        destRect.w = windowWidth;
        destRect.y = -(destRect.h-windowHeight)/2;
        destRect.x = 0;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bgImageTexture, &srcRect, &destRect);
        
    //renderOverlay(olImageTexture[0],destRect);
    relativePosition relative = {0.0,0.1,1.0,0.9};
    int n;
    for(n = 0; n < 6; n++) {
        if (olImageTexture[n])
            renderOverlayRelative(olImageTexture[n],relative);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    // Create a black gradient with varying alpha from the bottom to 25% of the screen height
    for (int y = windowHeight; y >= windowHeight * 0.65; y--) {
        float alpha = (y-(windowHeight * 0.65))/(windowHeight * 0.35);
        //printf("alpha(%f) y(%d)\n",alpha,y);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha*255);
        SDL_RenderDrawLine(renderer, 0, y, windowWidth, y);
    }

    int textX = TEXT_OFFSET; // offset from the sides
    int textY = windowHeight - (windowHeight * 25 / 100); // 25% from the bottom
    int lineHeight = TTF_FontLineSkip(font);

    char* textCopy = strdup(currentText); //"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\nUt enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "); // Make a copy of the text to avoid modifying the original
    char* token = strtok(textCopy, " ");
        
    while (token != NULL) {
        
        char* newlinePos = strchr(token, '\n');

        if (newlinePos != NULL) {
            // Calculate the length of the first part of the string
            size_t firstPartLength = newlinePos - token;

            // Create two new strings to hold the parts
            char firstPart[firstPartLength + 1]; // +1 for the null-terminator
            char secondPart[strlen(newlinePos) + 1];

            // Copy the first part of the string
            strncpy(firstPart, token, firstPartLength);
            firstPart[firstPartLength] = '\0'; // Null-terminate the first part

            // Copy the second part of the string (including the newline character)
            strcpy(secondPart, newlinePos+1);

            // Print the two parts
            //printf("First Part: %s\n", firstPart);
            //printf("Second Part: %s\n", secondPart);

            int tokenWidth = getTextWidth(firstPart, font);
            tokenWidth += getTextWidth(" ", font);
            if (textX + tokenWidth + TEXT_OFFSET > windowWidth) {
                textX = TEXT_OFFSET;
                textY += lineHeight;
            }
            renderText(firstPart, textX, textY);
            textX += tokenWidth;
            //
            textY += lineHeight;
            textX = TEXT_OFFSET;
            
            tokenWidth = getTextWidth(secondPart, font);
            tokenWidth += getTextWidth(" ", font);
            
            renderText(secondPart, textX, textY);
            textX += tokenWidth;

        } else {
            // nope

            int tokenWidth = getTextWidth(token, font);
            tokenWidth += getTextWidth(" ", font);
            if (textX + tokenWidth + TEXT_OFFSET > windowWidth) {
                textX = TEXT_OFFSET;
                textY += lineHeight;
            }
            renderText(token, textX, textY);
            textX += tokenWidth;
        }

        token = strtok(NULL, " ");
    }

    free(textCopy);

    //numChoicesDisplayed = 0;
    //renderChoiceButton("choice 1");
    //renderChoiceButton("choice 2");

    numChoicesDisplayed = 0;
    //printf("query choice buttons\n");
    for(int n = 0; n < nchoices; n++) {
        //printf("    %s\n",(char*)choices[n]);
        renderChoiceButton((char*)choices[n]);
    }

    SDL_RenderPresent(renderer);
}

static int resizingEventWatcher(void* data, SDL_Event* event) {
  if (event->type == SDL_WINDOWEVENT &&
      event->window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
    if (win == (SDL_Window*)data) {
      printf("resizing.....\n");
      redraw();
    }
  }
  return 0;
}

int loadBackgroundElement(TiXmlElement *eimage) {
    if (!eimage)
        return -1;
    bgImageTexture = IMG_LoadTexture(renderer, completePath(eimage->Attribute("filename")));
    return 0;
}

int setCurrentTextFromPage(CPage *page) {
    if (!page)
        return -1;
    
    if (page->mchardata) {
        currentText = (char*)page->mchardata;
    } else {
        currentText = "";
    }

    printf("setCurrentTextFromPage currentText(%s)\n",currentText);

    return 0;
}

int setCurrentOverlaysFromPage(CPage *page) {
    if (!page)
        return -1;

    COverlay *omaster = NULL;
    
    // if there isn't any <overlays> definition in page we try loading the scenes <overlays>
    if (!page->moverlay)
    {
        if (script->mcurrentScene->moverlay)
            omaster = script->mcurrentScene->moverlay;
    }
    else
        omaster = page->moverlay;
        
    if (omaster)
    {
        int n = 0;
        
        /** for(n = 0; n < [self.moverlays count]; n++ )
        {
            COverlayView *ov = [self.moverlays objectAtIndex:n];
            if (ov)
                ov.doclear = YES;
        } */
        for(n = 0; n < 6; n++) {
            if (olImageTexture[n]) {
                SDL_DestroyTexture(olImageTexture[n]);
                olImageTexture[n] = NULL;
            }
        }
        
        COverlay *current = omaster;
        while(current) {
            //current->loaded = NO;
            current = current->mnext;
        }
        
        n = 0;
        current = omaster;
        while(current) {

            olImageTexture[0] = IMG_LoadTexture(renderer, completePath(current->meoverlay->Attribute("filename")));
            n++;

            current = current->mnext;
        }
        
    } else {
        //[self clearOverlays];
    }

    return 0;
}
/**
int playCurrentSceneMusic(CScene *scene) {
    if (!page)
        return -1;
    
    if (page->mmusiceffect)
    {
        CMusicInfo *current = page->mmusiceffect->mmusicinfo;
        while(current)
        {
            if (strcmp(current->memusic->Attribute("filename"),currentMusicTrack)!=0) {
                printf("playCurrentMusic filename(%s)\n",current->memusic->Attribute("filename"));
                Mix_Music* music = Mix_LoadMUS(completePath(current->memusic->Attribute("filename")));
                if (music) {
                    Mix_PlayMusic(music, -1);
                }
                currentMusicTrack = (char*)current->memusic->Attribute("filename");
            }
            //[self playMusicEffectForMusicInfo:current];
            current = current->mnext;
        }
    }

    return 0;
}
*/
int setCurrentPage(CPage *page) {
    printf("setCurrentPage\n");
    if (!page)
        return -1;

    //[self setCurrentTextFromPage:page];
    //[self setCurrentOverlaysFromPage:page];
    setCurrentTextFromPage(page);
    setCurrentOverlaysFromPage(page);
    //playCurrentMusic(page);
    //[self runCurrentActionsFromPage:page];
    //[self playCurrentMusicEffectsFromPage:page];
    //[self playCurrentVideoEffectsFromPage:page];

    return 0;
}

int playSceneMusic(CScene *scene) {
    if (scene->mmusicinfo) {
        if (strcmp(scene->mmusicinfo->memusic->Attribute("filename"),currentMusicTrack)!=0) {
            printf("playCurrentMusic filename(%s)\n",scene->mmusicinfo->memusic->Attribute("filename"));
            
            Mix_Music* music = Mix_LoadMUS(completePath(scene->mmusicinfo->memusic->Attribute("filename")));
            if (music) {
                Mix_PlayMusic(music, -1);
            }
            currentMusicTrack = (char*)scene->mmusicinfo->memusic->Attribute("filename");
        }
    }
    return 0;
}

int onLastOfScene( ) {
    printf("onLastOfScene\n");

    CScene *scene = script->mcurrentScene;
    if (scene->mchoices && scene->mechoicedef) // mika - this lets us encapsulate <scopes> in <scene> generically
    {
        if (scene->melement->FirstChildElement("choices"))
        {
            TiXmlElement *echoicedef = scene->melement->FirstChildElement("choices")->FirstChildElement("choice");
            if (echoicedef)
            {
                nchoices = 0;
                numChoicesDisplayed = 0;
                if (echoicedef->Attribute("message")) {
                    //renderChoiceButton(echoicedef->Attribute("message"));
                    strcpy((char*)choices[nchoices],echoicedef->Attribute("message"));
                    nchoices++;
                } while(1) {
                    echoicedef = echoicedef->NextSiblingElement();
                    if (!echoicedef)
                        break;
                    if (echoicedef->Attribute("message")) {
                        strcpy((char*)choices[nchoices],echoicedef->Attribute("message"));
                        nchoices++;
                    }
                }
            }
        }
        
        return 1; // you can't have choices & <onfinish> actions in this version
    }

    // if there's an <onfinish goto="sco:sce"> we perform actions finishing the scene
    TiXmlElement *eonfinish = scene->melement->FirstChildElement("onfinish");
    if (eonfinish)
    {
        // with no options
        if (eonfinish->Attribute("goto"))
        {
            CScene *gotoscene = script->getSceneAtId((char*)eonfinish->Attribute("goto"));
            if (gotoscene)
            {
                script->setScene(gotoscene);
                //[self loadScene:gotoscene];
                loadScene(gotoscene);
                return 1;
            }
            else
            {
                printf("onLastOfScene warning, couldn't find goto scene\n");
            }
        }
        
        // with options, which work around the <context> attributes
        TiXmlElement *eroutes = eonfinish->FirstChildElement("routes");
        if (eroutes)
        {
            TiXmlElement *eroute = eroutes->FirstChildElement("route");
            if (eroute)
            {
                CContext *context = NULL;
                TiXmlElement *econtext = eroute->FirstChildElement("context"); // <conditions>
                if (!econtext)
                    econtext = eroute->FirstChildElement("conditions");
                if (econtext)
                {
                    context = new CContext(econtext);
                    if ((CCurrentContext*)getGlobalContext())
                    {
                        CCurrentContext *globalcontext = (CCurrentContext*)getGlobalContext();
                        globalcontext->list();
                        
                        if (globalcontext->includesContext(context))
                        {
                            CScene *gotoscene = script->getSceneAtId((char*)eroute->Attribute("goto"));
                            if (gotoscene)
                            {
                                script->setScene(gotoscene);
                                loadScene(gotoscene);
                                return 1;
                            }
                        } else {
                            printf("doesn't include context\n");
                        }
                    }
                }
                while(1)
                {
                    eroute = eroute->NextSiblingElement();
                    if (!eroute)
                        break;
                    
                    econtext = eroute->FirstChildElement("context"); // <conditions>
                    if (!econtext)
                        econtext = eroute->FirstChildElement("conditions");
                    if (econtext)
                    {
                        context = new CContext(econtext);
                        if (getGlobalContext())
                        {
                            CCurrentContext *globalcontext = (CCurrentContext*)getGlobalContext();
                            globalcontext->list();
                            
                            if (globalcontext->includesContext(context))
                            {
                                CScene *gotoscene = script->getSceneAtId((char*)eroute->Attribute("goto"));
                                if (gotoscene)
                                {
                                    script->setScene(gotoscene);
                                    //[self loadScene:gotoscene];
                                    loadScene(gotoscene);
                                    return 1;
                                }
                            } else {
                                printf("doesn't include context\n");
                            }
                        }
                    }
                }
            }
        }
        printf("end - couldn't match condition to global state\n");
        CCurrentContext *globalcontext = (CCurrentContext*)getGlobalContext();
        globalcontext->list();
    }

    return 0;
}

int loadSceneContent(CScene *scene) {
    if (!scene)
        return -1;

    script->onScene(scene);

    TiXmlElement *eimage = scene->getImageElement();
    if (eimage) {
        loadBackgroundElement(eimage);
    }

    CPage *pages = NULL;
    CPage *currentPage = NULL;
    if (script->mcurrentScene->mpages)
    {
        pages = script->mcurrentScene->mpages;
        currentPage = script->mcurrentScene->mcurrentPage;
    }
    else if (script->mcurrentScene->mtextarea)
    {
        if (script->mcurrentScene->mtextarea->mpages)
        {
            pages = script->mcurrentScene->mtextarea->mpages;
            currentPage = script->mcurrentScene->mtextarea->mcurrentPage;
        }
    }

    if (pages) {
        CPage *page = NULL;
        if ( currentPage ) { // we are jumping to page position
            page = currentPage;
        } else {
            if (script->mcurrentScene->mpages) {
                script->mcurrentScene->mcurrentPage = script->mcurrentScene->mpages;
                page = script->mcurrentScene->mcurrentPage;
            } else if (script->mcurrentScene->mtextarea) {
                script->mcurrentScene->mtextarea->mcurrentPage = script->mcurrentScene->mtextarea->mpages;
                page = script->mcurrentScene->mtextarea->mcurrentPage;
            }
        }
        //[self setCurrentPage:page];
        setCurrentPage(page);
    } else {
        //[self onLastOfScene];
        onLastOfScene();
    }

    playSceneMusic(scene);

    return 0;
}

int loadScope(CScope *scope) {
    if (!scope)
        return -1;

    CScene *cs = script->setScope(scope);
    if (cs) {
        loadScene(cs);
    } else {
        return 1;
    }

    return -1;
}

int loadScene(CScene *scene) { // when called from the interface

    printf("loadScene\n");
    if (!scene)
        return -1;
    
    script->mpresetNext = NULL;
    script->mpresetPrev = NULL;
    
    // is this a virtual scene? if so, then we fetch the scene with the content
    char *strcontent = (char*)scene->melement->Attribute("content");
    if (strcontent)
    {
        printf("loadScene content definition found\n");
        CScene *contentScene = script->getSceneAtId(strcontent);
        if (contentScene)
        {
            CScene *nextScene = script->next(-1);
            if (nextScene)
            {
                script->mpresetNext = nextScene;
                script->mpresetPrev = scene->mprev;
            }
            
            script->mcurrentScene = contentScene;
            script->mcurrentScope = (CScope*)contentScene->mparent;
            scene = contentScene;
        }
    }
    
    script->mcurrentScene = scene;
    script->mcurrentScope = (CScope*)scene->mparent;
    
    loadSceneContent(scene);
    redraw();

    return 0;
}

void onNextScene( ) {
    CScene *scene = script->next(-1);
    if (scene) {
        scene->rewind();
    } else {
    }
    loadScene(scene);
}

void onTap( int x, int y ) {
    printf("onTap\n");

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    // check if we've hit a choice button
    for(int n = 0; n < numChoicesDisplayed; n++) {
        int buttony = ((windowHeight/2)-(BUTTON_HEIGHT*2))+(n*(BUTTON_HEIGHT+20));
        if (x>TEXT_OFFSET && x<windowWidth-TEXT_OFFSET &&
            y>buttony && y<buttony+BUTTON_HEIGHT) {
            printf("tapped choice button(%d)\n",n);
            onQuestionButton(n);
            return;
        }
    }

    if (nchoices>0) {
        return;
    }

    // check if we just itterate the text page
    CPage *page = NULL;
    if (script->mcurrentScope->mtextarea) {
        //page = mscript->mcurrentScope->mtextarea->next();
        page = script->mcurrentScene->nextPage();
    }
    else if (script->mcurrentScene->mtextarea) {
        page = script->mcurrentScene->mtextarea->next();
    }
        
    // do we itterate the text page or the scene
    if ( page ) {
        printf("    next text page\n");
            
        setCurrentPage(page);
    } else {
        if (onLastOfScene()==0) {
            //if ( [self onLastOfScene] == 0 )
            //{
            //    [self clearLayers]; // clear layers - layers are scene dependent, so they should be cleared at end of scene
            //    [self onNextScene]; // move to next scene in the same scope
            //}
            onNextScene();
        }
    }
}

int main(int argc, char* argv[]) {

    if (argc > 0) {
        char *path = realpath(argv[0], NULL);
        if (path != NULL) {
            workingDirectory = dirname(path);
            printf("The directory of the currently running binary is: %s\n", workingDirectory);
            free(path);
        } else {
            perror("realpath");
            return 1;
        }
    } else {
        fprintf(stderr, "Unable to determine the binary path.\n");
        return 1;
    }

    loadScript("script_tears01-vnp.xml");
    CScene *scene = script->rewind();
    scene->rewind();

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        // Handle SDL initialization error
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    font = TTF_OpenFont(completePath(FONT_FILE), FONT_SIZE);
    if (font == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        // Handle SDL Mixer initialization error
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image initialization failed: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("VNPLAYER2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowMinimumSize(window, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);

    SDL_AddEventWatch(resizingEventWatcher, window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Enable linear filtering for smoother textures
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    /**
    bgImageTexture = IMG_LoadTexture(renderer, completePath("City Hall.png"));
    if (!bgImageTexture) {
        fprintf(stderr, "Image loading failed: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    olImageTexture[0] = IMG_LoadTexture(renderer, completePath("Neutral_Closed.png"));
    if (!olImageTexture[0]) {
        fprintf(stderr, "Image loading failed: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    */
    SDL_SetTextureBlendMode(olImageTexture[0], SDL_BLENDMODE_BLEND);
    /**
    Mix_Music* music = Mix_LoadMUS(completePath("GA_100_Dm_DarkShadow_FRK.mp3"));
    if (music == NULL) {
        // Handle audio loading error
        return 1;
    }
    //Mix_Music* music = Mix_QuickLoad_RAW(mp3Buffer, bufferSize); // to play raw data (load mp3 into mem)

    Mix_PlayMusic(music, -1);  // -1 loops the audio indefinitely
    */
    Mix_Volume(-1, MIX_MAX_VOLUME / 4);
    
    loadScene(scene);

    // Event loop
    SDL_Event event;
    int isRunning = 1;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = 0;
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    printf("Left mouse button clicked at (%d, %d)\n", event.button.x, event.button.y);
                    onTap(event.button.x,event.button.y);
                }
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    // Window has been resized
                    printf("SDL_WINDOWEVENT_RESIZED\n");
                } else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    printf("SDL_WINDOWEVENT_SIZE_CHANGED\n");
                    continue;
                } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
                    printf("SDL_WINDOWEVENT_MOVED\n");
                }
            }
        }

        redraw();

    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

