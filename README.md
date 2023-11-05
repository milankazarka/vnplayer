# vnplayer
SDL Based Cross-platform visual novel game engine

# build

To build vnplayer you have multiple options:

1.) use build.sh. This assumes that you know which dependencies you have to install:
SDL2, SDL2_image, SDL2_mixer, and SDL2_ttf


./build.sh


2.) use the Automake system for a more guided process:

autoreconf --install

./configure

make

Both should produce a ./vnplayer2 binary which if you run from the same directory should load the demo visual novel script from resources.

# use

In the visual novel, all interactions, music, backgrounds, overlays (characters), and choices are defined using an XML script. All files, including the script, images, music, and font have to be placed in the **./resources** folder. By default, the engine reads the **./resources/script.xml** file.

# game script

All the examples can be found in the **./resources/script.xml** - this together with the images and music in the **./resources** folder is the demo of the visual novel Tears of Enceladus. Feel free to modify it and use it as a starting point for your own visual novel.

First, we define the meta-information that goes along the visual novel. This resides in <script> which is the entry object to your visual novel.

```xml
<?xml version="1.0" encoding="utf-8"?>
<script storycode="tears01" version="1">
    <meta>
        <title>Tears of Enceladus Demo</title>
        <author>Milan Kazarka</author>
        <revision>1.0</revision>
        <revision_date>2023-01-01 5:05:29+0200</revision_date>

        <info>Tears of Enceladus - Visual novel by Milan Kazarka</info>
        <website>http://www.vysoko.com/</website>
        <copyright>© 2023 Milan Kazarka</copyright>

    </meta>
```

We use <scope> as a logical grouping of <scenes>. A <scope> is used as an entry point after an in-game decision and on entering a <scope> the player enters the first <scene> in the <scenes> that reside in the scope.

Each <scene> includes a definition of attributes that are being set, the background that's being used, music, layers (there can be a maximum of 6 layers on top of a background), and a list of pages that are where you define the dialogue between the characters. The character assets and how they are placed on the screen are defined per page.

Here is an example of a simple scene without character overlays per page, but just a layer on top of a background and a few lines of dialogue. In this example we do not yet introduce the character, so we keep (character="") undefined. For this scene, we chose to play (GA_100_Dm_DarkShadow_FRK.mp3) as the music track and we set the background to (Enceladus.png).

```xml
<scene id="sce02">
    <musiclist>
        <music filename="GA_100_Dm_DarkShadow_FRK.mp3" forcestart="yes" />
    </musiclist>
    <widgets>
        <image filename="Enceladus.png"/>
        <layers>
            <layer filename="Ashley in pod.png" size="1.0" alignment="center" id="l01" />
        </layers>
        <pages>
            <page character="" text="Suddenly, I feel a tremendous heat coursing through my body."/>
            <page character="" text="It's awakening all of my senses."/>
        </pages>
    </widgets>
</scene>
```

Here is a more complicated scene at the end of which we have some choices (<choices default="NO">). You can only define choices at the end of a <scene> and each choice jumps to a specific <scope>. In this example, we also define <overlays> for every page with relative positioning of the overlay defined by (posx="0.25" posy="0.1" width="0.5" height="0.90").

```xml
<scene id="sce_meet01_after_Father_interject02_withHumans01">
    <musiclist>
        <music filename="Modern-Filmscore-Vol.2-Track-1.mp3" forcestart="yes"/>
    </musiclist>
    <widgets>
        <image filename="City Hall Silhouettes.png"/>
            <pages>
                <page character="me" text="Humans?">
                    <overlays>
                        <overlay filename="Neutral_Closed.png" posx="0.25" posy="0.1" width="0.5" height="0.90"/>
                    </overlays>
                </page>
                <page character="me" text="There is a flickering, a movement, shadows on the walls.">
                    <overlays>
                        <overlay filename="Neutral_Closed.png" posx="0.25" posy="0.1" width="0.5" height="0.90"/>
                    </overlays>
                </page>
                ...
            </pages>
        </widgets>

        <choices default="NO">
            <choice message="What are those figures, those living beings?" action="sco_meet01.sco_humans01"/>
            <choice message="I don't care about those other beings." action="sco_meet01.sco_dontcare01"/>
        </choices>
```

You can define <scopes> within a <scene> effectively grouping the logic of your game into a decision tree. On the other hand, you can also choose to jump to any <scope> anywhere in the script. In our example, we added a list of <scopes> after the choices - so for example, if we choose ("What are those figures, those living beings?") we jump into the first scene of scope (sco_meet01.sco_humans01).

```xml
<choices default="NO">
    <choice message="What are those figures, those living beings?" action="sco_meet01.sco_humans01"/>
    <choice message="I don't care about those other beings." action="sco_meet01.sco_dontcare01"/>
</choices>
                
<scopes>
    <scope id="sco_meet01.sco_humans01">
        <textarea posx="5%" posy="82%" width="90%" height="18%"/>
        <scenes>
            <scene id="sce_humans01">
```

At the end of each scene, we can also define where to jump to based on attributes. Here is an example of a scene we jumped where we set an attribute - this attribute we then use in another scene to decide what scope to jump to.

```xml
<scope id="sco_meet01.sco_dontcare01">
    <textarea posx="5%" posy="82%" width="90%" height="18%"/>
    <scenes>
        <scene id="sce_dontcare01">
            <conditions>
                <attr key="ashleyannoyed" value="1" />
            </conditions>
            ...
```

The attribute that we set in the previous example is used as a condition to jump to (sco_listentomeinstead01:sce_listentomeinstead01). Using **<choices>**, **<conditions>**, attributes and **<onfinish>** are the primary ways we define the game logic. This way we can for example set the 'mood' of a character and end up in a different scene down the line. Using <conditions> is a way to make a change to the flow of your game without defining two whole new paths, but reusing entire scopes and only presenting a different path once we need to.

```xml
</widgets>
<onfinish>
    <routes>
        <route goto="sco_listentomeinstead01:sce_listentomeinstead01">
            <conditions>
                <attr key="ashleyannoyed" value="0" />
            </conditions>
        </route>
        <route goto="sco_listentomeinstead01:sce_listentomeinstead01">
            <conditions>
                <attr key="ashleyannoyed" value="1" />
            </conditions>
        </route>
        ...
```



