# TelegramContest2

My solution for **C++ Telegram Contest 2022**.

The task was to create a cross-platform C++ beautification module for faces and 
create a demo application for Android that uses the module to convert video steam from the front camera in real time. 

My app loads image from camera. 
Using arCore it gets face mesh. Image texture and mesh are loaded to vertex shader. Vertex shader projects mesh on the texture. 
Vertex shader gets texture colors in projected vertices positions. In fragment shader colors from vertex shader are interpolated. As a result, 
skin looks smother. To ignore eyes and hair we use alpha-texture. This solution took 3-rd place in the contest.

Wrote this code on March 2022 (during the contest) and publishing it now (December 2022). 

Feel free to use any code you find useful.
