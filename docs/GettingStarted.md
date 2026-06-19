# Getting Started

## Introduction

ManpMovieMaker (MMM) is a companion application for ManpWIN that creates high-quality fractal animations from image sequences.

Rather than rendering every frame of a movie, MMM can generate intermediate frames between rendered images, dramatically reducing rendering time while still producing smooth animations.

MMM can be used for:

* Deep zoom movies
* Parameter animations
* Julia set animations
* Other image sequence animations

The final output is a sequence of JPG frames and an FFmpeg script that can be used to create a high-quality MP4 movie.

---

## Prerequisites

Before using ManpMovieMaker you will need:

1. A sequence of PNG images.
2. ManpMovieMaker.
3. FFmpeg.

See:

* CreatingAnimationsInManpWIN.md
* FFmpegInstallation.md

for detailed instructions.

---

## Typical Workflow

Creating a movie consists of two stages:

### Stage 1 - Prepare JPG Frames

Use the "Prepare JPG Frames" dialog to:

* Select a folder containing PNG images.
* Specify the number of intermediate frames.
* Specify movie frame rate.
* Generate a complete JPG frame sequence.

MMM creates additional frames between rendered images by resizing and cropping images to produce smooth transitions.

---

### Stage 2 - Create FFmpeg Script

Use the "Create FFmpeg Script" dialog to:

* Select the JPG frame sequence.
* Specify movie parameters.
* Optionally add an audio track.
* Optionally add audio fade-in and fade-out.
* Generate an FFmpeg command script.

The generated script is then executed using FFmpeg to create the final MP4 movie.

---

## Directory Layout

A typical project might use:

PNG/
    Frame00001.png
    Frame00002.png
    ...

JPG/
    Frame00001.jpg
    Frame00002.jpg
    ...
    MyMovie.mp4

The PNG folder contains the original rendered images.

The JPG folder contains the generated movie frames and the final MP4 movie.

---

## Your First Movie

1. Create a PNG image sequence.
2. Open ManpMovieMaker.
3. Prepare JPG Frames.
4. Create an FFmpeg Script.
5. Run the script.
6. Enjoy your movie.

---

## Keyboard Shortcuts

ManpMovieMaker provides quick access to its two main functions:

| Key | Function             |
| --- | -------------------- |
| S   | Prepare JPG Frames   |
| F   | Create FFmpeg Script |

The typical workflow is:

1. Press **S** to prepare the JPG frame sequence.
2. Press **F** to create the FFmpeg script.
3. Run the generated FFmpeg script to create the final MP4 movie.

---

## Next Steps

For more detailed information see:

* CreatingAnimationsInManpWIN.md
* PrepareFrames.md
* CreateFFmpegScript.md
* FFmpegInstallation.md
* AnimationTheory.md
