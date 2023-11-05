# vnplayer
SDL Based Cross-platform visual novel game engine

# build

To build vnplayer you have multiple options:

1.) use build.sh. This assumes that you know which dependencies you have to install:
SDL2, SDL2_image, SDL2_mixer, and SDL2_ttf


./build.sh


2.) use CMake for a more guided process:


mkdir ./build

cd build

cmake ../

make

cd ../

# use

# game script

```xml
<?xml version="1.0" encoding="utf-8"?>
<script storycode="tears01" version="1">
    <meta>
        <title>Tears of Enceladus</title>
        <author>Milan Kazarka</author>
        <revision>1.0</revision>
        <revision_date>2023-01-01 5:05:29+0200</revision_date>

        <info>Tears of Enceladus - Visual novel by Milan Kazarka</info>
        <website>http://www.vysoko.com/</website>
        <copyright>© 2023 Milan Kazarka</copyright>

        <!-- menu settings -->
        <menu-background>menuBackground04.png</menu-background>
        <menu-label-style>background: none; border: none; color: black;</menu-label-style>
        <menu-button-style>background: #A0404040; border: 1px #A0404040; color: white; min-height: 40px; border-radius: 8px;</menu-button-style>
    </meta>
```

