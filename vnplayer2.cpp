/*
 * Copyright (C) [2023] Milan Kazarka
 * Email: milan.kazarka.office@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
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
#define _VERTICAL_MODE_ENABLED

SDL_Texture* bgImageTexture = NULL;
SDL_Texture* testImageTexture = NULL;
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
char *currentCharacter = NULL;
char *currentMusicTrack = "";
const int BUTTON_HEIGHT = 60;
const int MIN_WINDOW_WIDTH = 800;
const int MIN_WINDOW_HEIGHT = 480;
int numChoicesDisplayed = 0; // this just counts the number of choices displayed. We calculate if we hit a button manually.
char *choices[4][255];
int nchoices = 0;
bool menu = true; // is the menu displayed or not
int selectedSlot = -1;
const int MAX_SAVEFILE_LINE_LENGTH = 256;
bool playPressed = false;
bool gameEnd = false; // did we reach the ending? this is the alert showing that we reached the end
bool bcontinue = false; // do we show "start" or "continue"

char* constructFilePath(const char* directory, const char* filename, char* buffer, size_t bufferSize);
char* completePath(const char* filename); // not thread safe, just uses workingDirectory & filenameBuffer
int loadScene(CScene *scene);
int loadScope(CScope *scope);
void onTap( int x, int y );
void onNextScene( );
void saveScreenshot( );
void loadSaveFile(const char *filePath);
int copyFile(const char *sourcePath, const char *destinationPath);
void renderTextWithColor(const char* text, int x, int y, int color);
void renderTextWithRGB(const char* text, int x, int y, int r, int g, int b);

typedef struct relativePosition {
    float x, y, w, h;
} relativePosition;

typedef struct overlay {
    SDL_Texture *texture;
    relativePosition relative; // relative positioning
} overlay;

overlay overlays[6];

typedef struct uiButton {
    SDL_Rect rect;
    char text[64];
    int tag = 0; // identifier of the button
    bool hidden;
    int bgColor; // 0-255
    int fgColor; // 0-255
    int bgAlpha = 64;
} uiButton;

typedef struct saveSlot {
    SDL_Rect rect;
    uiButton button; // only used for the rect, not drawn
} saveSlot;

bool didTapButton( uiButton *button, int x, int y ) {
    if (!button)
        return false;
    if (button->hidden)
        return false;
    if (x>button->rect.x && x<button->rect.x+button->rect.w && y>button->rect.y && y<button->rect.y+button->rect.h)
        return true;
    return false;
}

uiButton loadButton;
uiButton saveButton;
uiButton playButton; // start/continue
uiButton restartButton;
uiButton menuButton;
uiButton gameendButton;
saveSlot slots[6];

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

int fileExists(const char *path) {
    return access(path, F_OK) != -1;
}

// read the savefile - in a format:
//      1. position
//      2. attribute:value
//      3. attribute:value
//      ... 
//
void loadSaveFile(const char *filePath) {

    printf("loadSaveFile\n");

    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_SAVEFILE_LINE_LENGTH];
    int firstLine = 1;  // Flag to check if it's the first line

    CScene *cs = NULL;
    CCurrentContext *cc = NULL;

    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove trailing newline characters
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
        }

        if (firstLine) {
            // First line, handle it as a parameter
            printf("First Line (Parameter): %s\n", line);
            firstLine = 0;

            cs = script->getSceneAtId(line,false);
            if (cs) {
                cc = getGlobalContext();
                if (cc) {
                    cc->flush();
                }
            }
        } else {
            if (cs && cc) {
                // Subsequent lines, treat as attribute:value pairs
                char *token = strtok(line, ":");
                if (token != NULL) {
                    char *attribute = token;
                    char *value = strtok(NULL, ":");
                    if (value != NULL) {
                        printf("Attribute: %s, Value: %s\n", attribute, value);
                        CAttr *attr = new CAttr(attribute,value);
                        cc->addAttribute(attr);
                    }
                }
            }
        }
    }

    fclose(file);

    if (cs) {
        loadScene(cs);
    }
}

void onLoad( ) {
    printf("onLoad\n");
    if (selectedSlot==-1) {
        printf("no save slot selected\n");
        return;
    }
    char tmp[256];
    sprintf((char*)tmp,"savefile%d.txt",selectedSlot);
    loadSaveFile(completePath((char*)tmp));
}

void onSave( ) {
    printf("onSave\n");
    if (selectedSlot==-1) {
        printf("no save slot selected\n");
        return;
    }
    // save the current screenshot for the savefile position
    char tmp[256];
    sprintf((char*)tmp,"screenshot%d.bmp",selectedSlot);
    char source[256], destination[256];
    strcpy((char*)source,completePath("screenshot.bmp"));
    strcpy((char*)destination,completePath((char*)tmp));
    copyFile(source,destination);

    // save the position & the current attributes
    char *strid = script->getCurrentMultilevelSceneId();
    if (strid) {

        sprintf((char*)tmp,"savefile%d.txt",selectedSlot);
        FILE *file = fopen(completePath((char*)tmp), "w");
        if (file == NULL) {
            perror("Error opening file");
            return;
        }

        fprintf(file,strid);
        fprintf(file,"\n");

        printf("save at position(%s)\n",strid);
        CCurrentContext *cc = getGlobalContext();
        if (cc) {
            CAttr *attr = cc->mattributes;
            while(attr) {
                if (attr->mkey && attr->mvalue) {
                    printf("save attribute key(%s) value(%s)\n",attr->mkey,attr->mvalue);
                    fprintf(file,attr->mkey);
                    fprintf(file,":");
                    fprintf(file,attr->mvalue);
                    fprintf(file,"\n");
                }
                attr = attr->mnext;
            }
        }

        fclose(file);
    }
}

// Function to copy a file from sourcePath to destinationPath
int copyFile(const char *sourcePath, const char *destinationPath) {

    printf("copyFile(%s,%s)\n",sourcePath,destinationPath);

    FILE *sourceFile = fopen(sourcePath, "rb");
    if (sourceFile == NULL) {
        perror("Error opening source file");
        return 1;
    }

    FILE *destinationFile = fopen(destinationPath, "wb");
    if (destinationFile == NULL) {
        fclose(sourceFile);
        perror("Error opening destination file");
        return 1;
    }

    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
        if (fwrite(buffer, 1, bytesRead, destinationFile) != bytesRead) {
            perror("Error writing to destination file");
            fclose(sourceFile);
            fclose(destinationFile);
            return 1;
        }
    }

    fclose(sourceFile);
    fclose(destinationFile);

    return 0; // Success
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

// this is good if we want to print text in monochrome
void renderTextWithColor(const char* text, int x, int y, int color) {
    renderTextWithRGB(text,x,y,color,color,color);
}

void renderTextWithRGB(const char* text, int x, int y, int r, int g, int b) {
    SDL_Color colorDef;
    colorDef.r = static_cast<Uint8>(r); // Ensure r, g, and b are within the range 0-255
    colorDef.g = static_cast<Uint8>(g);
    colorDef.b = static_cast<Uint8>(b);

    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, colorDef);
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
    SDL_FreeSurface(textSurface);
}

void renderUIButton(uiButton *buttonDef) {
    if (buttonDef->hidden)
        return;
    // Draw a semi-transparent black rectangle
    SDL_SetRenderDrawColor(renderer, buttonDef->bgColor, buttonDef->bgColor, buttonDef->bgColor, buttonDef->bgAlpha);
    SDL_RenderFillRect(renderer, &buttonDef->rect);

    SDL_Color colorDef;
    colorDef.r = static_cast<Uint8>(buttonDef->fgColor); // Ensure fgColor is within the range 0-255
    colorDef.g = static_cast<Uint8>(buttonDef->fgColor);
    colorDef.b = static_cast<Uint8>(buttonDef->fgColor);
    colorDef.a = 255; // Set the alpha channel to 255
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, (char*)buttonDef->text, colorDef);

    renderTextWithColor((char*)buttonDef->text,buttonDef->rect.x+((buttonDef->rect.w-textSurface->w)/2),buttonDef->rect.y+((buttonDef->rect.h-textSurface->h)/2),buttonDef->fgColor);
    SDL_FreeSurface(textSurface);
}

void renderBMPImage(const char* imagePath, SDL_Rect rect) {
    // Load a BMP image

    if (!fileExists(imagePath))
        return;

    SDL_Surface* image = SDL_LoadBMP(imagePath);
    if (image == NULL) {
        printf("Failed to load image! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_Texture* imageTexture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    SDL_QueryTexture(imageTexture, NULL, NULL, &srcRect.w, &srcRect.h);

    if ((float)((float)srcRect.w/(float)srcRect.h)>(float)((float)rect.w/(float)rect.h)) {
        // source is wider, sides will be cut
        float cutout = (float)srcRect.h*(float)((float)rect.w/(float)rect.h);
        srcRect.x = (srcRect.w-cutout)/2;
        srcRect.w = cutout;
        printf("adjusting src(%dx%d+%d+%d) dest(%dx%d) cutout(%f) dest_val(%d)\n",
            srcRect.w,srcRect.h,srcRect.x,srcRect.y,rect.w,rect.h,cutout,rect.w/rect.h);
    } else {
        // destination is wider
        float cutout = (float)srcRect.w*(float)((float)rect.h/(float)rect.w);
        srcRect.y = (srcRect.h-cutout)/2;
        srcRect.h = cutout;
    }

    // Render the image
    SDL_RenderCopy(renderer, imageTexture, &srcRect, &rect);
    SDL_DestroyTexture(imageTexture);
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
void renderOverlay(overlay *ol) {

    if (!ol)
        return;

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    SDL_QueryTexture(ol->texture, NULL, NULL, &srcRect.w, &srcRect.h);

    SDL_Rect destRect;
    destRect.x = (int)(ol->relative.x * windowWidth);
    destRect.y = (int)(ol->relative.y * windowHeight);
    destRect.w = (int)(ol->relative.w * windowWidth);
    destRect.h = (int)(ol->relative.h * windowHeight);

    // Calculate aspect-fit scaling based on the relative position and size
    float imageAspectRatio = (float)srcRect.w / srcRect.h;
    float destAspectRatio = (float)destRect.w / destRect.h;
    
    // we override this so that we can display the VN in vertical mode
#ifndef _VERTICAL_MODE_ENABLED
    
    if (imageAspectRatio > destAspectRatio) {
        // Image is wider than the destination rect
        printf("image is wider than destination\n");
        destRect.h = (int)(destRect.w / imageAspectRatio);
    } else {
#endif
        // Image is taller than the destination rect
        destRect.w = (int)(destRect.h * imageAspectRatio);
#ifndef _VERTICAL_MODE_ENABLED
    }
#endif

    destRect.x = destRect.x + (int)((ol->relative.w * windowWidth - destRect.w) / 2);
    destRect.y = destRect.y + (int)((ol->relative.h * windowHeight - destRect.h) / 2);

    renderOverlay(ol->texture,destRect);
}

void redraw() {

    SDL_Rect srcRect, destRect;
    int windowWidth, windowHeight;
    srcRect.x = 0;
    srcRect.y = 0;
    SDL_QueryTexture(bgImageTexture, NULL, NULL, &srcRect.w, &srcRect.h);

    // Get the current window size
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        
    if (menu) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_Rect menuRect;
        menuRect.x = 0;
        menuRect.y = 0;
        menuRect.w = windowWidth;
        menuRect.h = windowHeight;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &menuRect);

        renderTextWithColor("vnplayer",20,20,255);

        saveButton.rect.x = 20;
        saveButton.rect.y = windowHeight-80;
        renderUIButton(&saveButton);
        loadButton.rect.x = saveButton.rect.x+saveButton.rect.w+20;
        loadButton.rect.y = windowHeight-80;
        renderUIButton(&loadButton);
        playButton.rect.x = windowWidth-playButton.rect.w-20;
        playButton.rect.y = windowHeight-80;
        if (bcontinue) {
            strcpy((char*)playButton.text,"continue");
        }
        renderUIButton(&playButton);
        restartButton.rect.x = playButton.rect.x-restartButton.rect.w-20;
        restartButton.rect.y = windowHeight-80;
        renderUIButton(&restartButton);

        SDL_Rect slotsRect;
        slotsRect.x = 20;
        slotsRect.y = 80;
        slotsRect.w = windowWidth-40;
        slotsRect.h = windowHeight-80-saveButton.rect.h-40;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 32);
        SDL_RenderFillRect(renderer, &slotsRect);

        // now set the save slot grid items & draw them
        // 1 row:
        slots[0].rect.x = slotsRect.x;
        slots[0].rect.y = slotsRect.y;
        slots[1].rect.x = slotsRect.x+slotsRect.w/3;
        slots[1].rect.y = slotsRect.y;
        slots[2].rect.x = slotsRect.x+(slotsRect.w/3)*2;
        slots[2].rect.y = slotsRect.y;
        // 2 row:
        slots[3].rect.x = slotsRect.x;
        slots[3].rect.y = slotsRect.y+(slotsRect.h/2);
        slots[4].rect.x = slotsRect.x+slotsRect.w/3;
        slots[4].rect.y = slotsRect.y+(slotsRect.h/2);
        slots[5].rect.x = slotsRect.x+(slotsRect.w/3)*2;
        slots[5].rect.y = slotsRect.y+(slotsRect.h/2);
        for(int n = 0; n < 6; n++) {
            slots[n].rect.w = (slotsRect.w/3);
            slots[n].rect.h = (slotsRect.h/2);

            slots[n].rect.x+=1;
            slots[n].rect.y+=1;
            slots[n].rect.w-=2;
            slots[n].rect.h-=2;
        }
        char modPath[256];
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 30);
        for(int n = 0; n < 6; n++) {
            slots[n].button.rect = slots[n].rect;
            slots[n].button.hidden = false;
            SDL_RenderFillRect(renderer, &slots[n].rect);
            sprintf((char*)modPath,"screenshot%d.bmp",n);
            renderBMPImage(completePath((char*)modPath),slots[n].rect);
        }

        if (selectedSlot>-1) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
            SDL_RenderDrawRect(renderer, &slots[selectedSlot].rect);
        }

    } else {

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
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_RenderCopy(renderer, bgImageTexture, &srcRect, &destRect);

        int n;
        for(n = 0; n < 6; n++) {
            // #todo - do a function for rendering layers like we have with renderOverlay()
            SDL_QueryTexture(laImageTexture[n], NULL, NULL, &srcRect.w, &srcRect.h);
            imageAspectRatio = (float)srcRect.w / srcRect.h;
            windowAspectRatio = (float)windowWidth / windowHeight;

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
            SDL_RenderCopy(renderer, laImageTexture[n], &srcRect, &destRect);
        }

        for(n = 0; n < 6; n++) {
            if (overlays[n].texture) {
                renderOverlay(&overlays[n]);//.texture,overlays[n].relative);
            }
        }

        // Create a black gradient with varying alpha from the bottom to 25% of the screen height
        for (int y = windowHeight; y >= windowHeight * 0.65; y--) {
            float alpha = (y-(windowHeight * 0.65))/(windowHeight * 0.35);
            //printf("alpha(%f) y(%d)\n",alpha,y);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha*255);
            SDL_RenderDrawLine(renderer, 0, y, windowWidth, y);
        }

        // Create a black gradient with varying alpha from the top to 10% of the screen height
        for (int y = 0; y <= windowHeight * 0.1; y++) {
            float alpha = y/(windowHeight * 0.1);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255-(alpha*255));
            SDL_RenderDrawLine(renderer, 0, y, windowWidth, y);
        }

        int textX = TEXT_OFFSET; // offset from the sides
        int textY = windowHeight - (windowHeight * 25 / 100); // 25% from the bottom
        int lineHeight = TTF_FontLineSkip(font);

        if (currentCharacter) {
            printf("print character name(%s)\n",currentCharacter);
            if (strlen(currentCharacter)) {
                char tmp[256];
                sprintf((char*)tmp,"%s:",currentCharacter);
                renderTextWithRGB((char*)tmp,textX,textY,255,165,0);
            }
        }
        textY+=lineHeight; 

        char* textCopy = strdup(currentText);
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

        numChoicesDisplayed = 0;
        //printf("query choice buttons\n");
        for(int n = 0; n < nchoices; n++) {
            //printf("    %s\n",(char*)choices[n]);
            renderChoiceButton((char*)choices[n]);
        }

        renderUIButton(&menuButton);

        if (gameEnd) {
            gameendButton.rect.x = (windowWidth/2)-(gameendButton.rect.w/2);
            gameendButton.rect.y = (windowHeight/2)-(gameendButton.rect.h/2);
            SDL_Rect rect;
            rect.x = gameendButton.rect.x-20;
            rect.y = gameendButton.rect.y-20;
            rect.w = gameendButton.rect.w+40;
            rect.h = gameendButton.rect.h+40;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 20);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 60);
            SDL_RenderDrawRect(renderer, &gameendButton.rect);
            renderUIButton(&gameendButton);
        }

    } // menu yes/no

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
        if (page->mepage->Attribute("character")) {
            currentCharacter = (char*)page->mepage->Attribute("character");
            printf("set the current character(%s)\n",currentCharacter);
        } else {
            currentCharacter = NULL;
        }
    } else {
        currentText = "";
        currentCharacter = NULL;
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
        
        for(n = 0; n < 6; n++) {
            if (overlays[n].texture) {
                SDL_DestroyTexture(overlays[n].texture);
                overlays[n].texture = NULL;
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

            overlays[n].texture = IMG_LoadTexture(renderer, completePath(current->meoverlay->Attribute("filename")));
            SDL_SetTextureBlendMode(overlays[n].texture, SDL_BLENDMODE_BLEND);
            overlays[n].relative.x = current->posx;
            overlays[n].relative.y = current->posy;
            overlays[n].relative.w = current->w;
            overlays[n].relative.h = current->h;
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

int playCurrentEffectFromPage(CPage *page) {
    if (!page)
        return 0;

    if (page->mmusiceffect) {
        
        CMusicInfo *current = page->mmusiceffect->mmusicinfo;
        while(current) {

            if (current->memusic->Attribute("filename")) {
                Mix_Music* music = Mix_LoadMUS(completePath(current->memusic->Attribute("filename")));
                if (music) {
                    Mix_PlayMusic(music, 1); // only play once
                }
            }

            current = current->mnext;
        }
    }

    return 1;
}

int setCurrentPage(CPage *page) {
    printf("setCurrentPage\n");
    if (!page)
        return 0;

    setCurrentTextFromPage(page);
    setCurrentOverlaysFromPage(page);
    //playCurrentMusic(page);

    return 1;
}

int playSceneMusic(CScene *scene) {
    if (scene->mmusicinfo) {
        if (strlen(scene->mmusicinfo->memusic->Attribute("filename"))==0) {
            Mix_PauseMusic();
            return 0;
        }

        if (strcmp(scene->mmusicinfo->memusic->Attribute("filename"),currentMusicTrack)!=0) {
            printf("playCurrentMusic filename(%s)\n",scene->mmusicinfo->memusic->Attribute("filename"));
            
            Mix_Music* music = Mix_LoadMUS(completePath(scene->mmusicinfo->memusic->Attribute("filename")));
            if (music) {
                Mix_PlayMusic(music, -1);
            }
            currentMusicTrack = (char*)scene->mmusicinfo->memusic->Attribute("filename");
        }
    } else {
        Mix_PauseMusic();
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

int loadLayersForScene(CScene *scene) {
    printf("loadLayersForScene\n");
    if (!scene)
        return 0;
    
    for(int n = 0; n<6; n++) {
        if (laImageTexture[n]) {
            SDL_DestroyTexture(laImageTexture[n]);
            laImageTexture[n] = NULL;
        }
    }

    int index = 0;
    CLayer *current = scene->mlayer;
    while(current) {
        if (current->melayer->Attribute("filename")) {
            laImageTexture[index] = IMG_LoadTexture(renderer, completePath(current->melayer->Attribute("filename")));
            index++;
        }
        current = current->mnext;
    }

    return 1;
}

int loadSceneContent(CScene *scene) {
    if (!scene)
        return 0;

    script->onScene(scene);

    if (bgImageTexture) {
        SDL_DestroyTexture(bgImageTexture);
        bgImageTexture = NULL;
    }

    for(int n = 0; n < 6; n++) {
        if (overlays[n].texture) {
            SDL_DestroyTexture(overlays[n].texture);
            overlays[n].texture = NULL;
        }
    }

    TiXmlElement *eimage = scene->getImageElement();
    if (eimage) {
        loadBackgroundElement(eimage);
    }

    loadLayersForScene(scene);

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

    return 1;
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
        if (script->mcurrentScene->mparent!=script->mscope) {
            // ending
            gameEnd = true;
        } else {
            printf("also end of game\n");
            // #todo - probably good ending
            gameEnd = true; // #todo make a different view for this
        }
        redraw();
        return;
    }
    loadScene(scene);
}

void onMenuWindow( int x, int y ) {
    printf("onMenuWindow\n");

    for(int n = 0; n < 6; n++) {
        if (didTapButton(&slots[n].button,x,y)) {
            printf("tapped slot(%d)\n",n);
            selectedSlot = n;
            return;
        }
    }

    if (didTapButton(&saveButton,x,y)) {
        printf("saveButton tapped\n");
        
        onSave();

    } else if (didTapButton(&loadButton,x,y)) {
        printf("loadButton tapped\n");
        if (selectedSlot>-1) {
            menu = false;
            bcontinue = true;
            onLoad();
        }
    } else if (didTapButton(&playButton,x,y)) {
        printf("playButton tapped\n");
        
        menu = false;
        bcontinue = true;
        Mix_ResumeMusic();
        redraw();
    } else if (didTapButton(&restartButton,x,y)) {
        printf("restartButton tapped\n");
        
        CScene *scene = script->rewind();
        scene->rewind();
        menu = false;
        loadScene(scene);
    }

}

// on the menu button in game
void onMenu( ) {
    nchoices = 0;
    selectedSlot = -1;
    saveScreenshot(); // we save the screenshot everytime we go into the menu in case we want to do a save
    Mix_PauseMusic();
    menu = true;
    redraw();
}

void onTap( int x, int y ) {
    printf("onTap\n");

    if (gameEnd) {
        if (didTapButton(&gameendButton,x,y)) {
            gameEnd = false;

            CScene *scene = script->rewind();
            scene->rewind();
            loadScene(scene);

            return;
        }
        return; // we lock you in at the ending - you can't go to the menu unless you press the restart button
    }

    if (didTapButton(&menuButton,x,y)) {
        onMenu();
        return;
    }

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

void saveScreenshot( ) {
    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

    // Capture a screenshot
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    SDL_Surface* screenshot = SDL_CreateRGBSurface(0, windowWidth, windowHeight, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);
    SDL_SaveBMP(screenshot, completePath("screenshot.bmp")); // Save the screenshot as a BMP file - in the resources

    // Cleanup and exit
    SDL_FreeSurface(screenshot);
}

int main(int argc, char* argv[]) {

    if (argc > 0) {
        char *path = realpath(argv[0], NULL);
        if (path != NULL) {
            char *tmp = dirname(path);
            workingDirectory = (char*)malloc(strlen(tmp)+1);
            strcpy(workingDirectory,tmp);
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

    for(int n = 0; n < 6; n++) {
        overlays[n].texture = NULL;
    }

    //
    saveButton.tag = 0;
    strcpy(saveButton.text,"save");
    saveButton.rect.w = 120;
    saveButton.rect.h = 60;
    saveButton.bgColor = 255;
    saveButton.fgColor = 255;
    saveButton.hidden = false;
    loadButton.tag = 1;
    strcpy(loadButton.text,"load");
    loadButton.rect.w = 120;
    loadButton.rect.h = 60;
    loadButton.bgColor = 255;
    loadButton.fgColor = 255;
    loadButton.hidden = false;
    playButton.tag = 2;
    strcpy(playButton.text,"start");
    playButton.rect.w = 120;
    playButton.rect.h = 60;
    playButton.bgColor = 255;
    playButton.fgColor = 255;
    playButton.hidden = false;
    restartButton.tag = 3;
    strcpy(restartButton.text,"restart");
    restartButton.rect.w = 120;
    restartButton.rect.h = 60;
    restartButton.bgColor = 255;
    restartButton.fgColor = 255;
    restartButton.hidden = false;
    //
    menuButton.tag = 99;
    strcpy(menuButton.text,"menu");
    menuButton.rect.x = 10;
    menuButton.rect.y = 10;
    menuButton.rect.w = 90;
    menuButton.rect.h = 30;
    menuButton.bgColor = 0;
    menuButton.bgAlpha = 0;
    menuButton.fgColor = 200;
    menuButton.hidden = false;
    //
    gameendButton.tag = 100;
    strcpy(gameendButton.text,"restart");
    gameendButton.rect.x = 0;
    gameendButton.rect.y = 0;
    gameendButton.rect.w = 120;
    gameendButton.rect.h = 40;
    gameendButton.bgColor = 0;
    gameendButton.fgColor = 255;
    gameendButton.hidden = false;

    loadScript("script.xml");
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

    //SDL_SetTextureBlendMode(olImageTexture[0], SDL_BLENDMODE_BLEND);
    
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
                    if (menu) {
                        onMenuWindow(event.button.x,event.button.y);
                    } else {
                        onTap(event.button.x,event.button.y);
                    }
                }
                redraw();
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

        //redraw();

    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

