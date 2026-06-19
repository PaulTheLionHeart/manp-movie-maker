# ManpMovieMaker

ManpMovieMaker is a companion application for ManpWIN that bridges the gap between fractal rendering and movie production.

The program was originally developed to solve a practical problem faced by fractal enthusiasts and deep-zoom explorers: creating smooth, high-quality animations without having to render every movie frame directly from the fractal engine.

By generating intermediate animation frames between rendered images, ManpMovieMaker can dramatically reduce production times while still producing smooth visual motion. The generated frame sequences can then be assembled into professional-quality MP4 movies using FFmpeg.

Although originally designed for deep zoom animations, ManpMovieMaker can also be used for many other forms of image-based animation, including:

* Deep zoom sequences
* Julia set animations
* Parameter animations
* Morphing image sequences
* General image-based movies

Over time, ManpMovieMaker has evolved beyond a simple interpolation utility into a movie-production tool that assists with frame generation, FFmpeg scripting, audio integration, and final movie creation.

---

# Why Use ManpMovieMaker?

Producing high-quality fractal movies can be computationally expensive.

A single deep zoom animation may require thousands of rendered images, with each image potentially taking seconds, minutes, or even hours to generate depending on image size, zoom depth, rendering method, and arithmetic precision.

ManpMovieMaker helps reduce this workload by generating additional intermediate frames between rendered images, allowing smooth animations to be produced from a smaller set of source frames.

The result is a practical workflow that combines the rendering power of ManpWIN with the movie-production capabilities of FFmpeg.

---

# Features

Current capabilities include:

* Generate JPG animation frames from PNG image sequences.
* Create smooth zoom animations.
* Create smooth morphing animations between rendered images.
* Insert intermediate frames between source images.
* Control movie frame rates.
* Add start and end hold periods.
* Generate FFmpeg command scripts.
* Add music tracks to completed movies.
* Support audio fade-in and fade-out effects.
* Create high-quality MP4 movies using FFmpeg.

---

# Typical Workflow

1. Create a sequence of PNG images.
2. Generate animation JPG frames using ManpMovieMaker.
3. Create an FFmpeg movie-production script.
4. Produce a high-quality MP4 movie.

---

## Documentation

Detailed documentation is available in the docs directory.

### Getting Started

- [Getting Started](docs/GettingStarted.md)

### Tutorials

- [Creating Animations in ManpWIN](docs/CreatingAnimationsInManpWIN.md)

### User Guides

- [Prepare JPG Frames](docs/PrepareFrames.md)
- [Create FFmpeg Script](docs/CreateFFmpegScript.md)
- [FFmpeg Installation](docs/FFmpegInstallation.md)

### Background

- [Animation Theory](docs/AnimationTheory.md)
- [ManpMovieMaker Modernisation](docs/ManpMovieMakerModernisation.md)

---

# Project Status

ManpMovieMaker is currently undergoing active modernisation and enhancement.

Development is focused on preserving the proven functionality of the original application while improving usability, documentation, workflow integration, and long-term maintainability.

---

# Related Projects

## ManpWIN

ManpMovieMaker was developed as a companion application for ManpWIN, a Windows fractal generation and exploration system supporting deep zoom rendering, perturbation theory, Bilinear Approximation (BLA), arbitrary precision arithmetic, formula parsing, and fractal animation generation.

Repository:

https://github.com/PaulTheLionHeart/manpwin

---

# Credits

**Paul the LionHeart**
Author and Developer

**ChatGPT**
Workshop Assistant, Reviewer, Teacher, Sounding Board, and Enthusiastic Dragon Spotter

*"Fun, beauty and passion."*
