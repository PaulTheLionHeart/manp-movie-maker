# ManpMovieMaker

ManpMovieMaker is a companion application for ManpWIN that creates high-quality fractal animations from PNG image sequences.

The program was originally developed to solve a practical problem faced by many fractal enthusiasts: creating smooth zoom movies without having to render thousands of expensive intermediate frames.

By resizing and cropping images between rendered frames, ManpMovieMaker can generate intermediate animation frames that significantly reduce rendering times while still producing smooth zoom animations. The resulting image sequences can then be assembled into high-quality MP4 movies using FFmpeg.

Although originally designed for deep zoom animations, the program can also be used to assemble other forms of fractal animation, including parameter animations and Julia set sequences.

---

## Current Features

* Create animation frame sequences from ManpWIN PNG images.
* Generate interpolated frames between rendered images.
* Resize and crop images to maintain smooth zoom transitions.
* Generate FFmpeg command scripts.
* Add audio tracks to completed animations.
* Create high-quality MP4 movies using FFmpeg.

---

## Typical Workflow

1. Generate a sequence of PNG frames using ManpWIN.
2. Load the frame sequence into ManpMovieMaker.
3. Specify the number of interpolated frames required.
4. Generate the complete frame sequence.
5. Create FFmpeg commands.
6. Assemble the final MP4 movie.
7. Optionally add an audio soundtrack.

---

## Project Status

ManpMovieMaker is currently undergoing a modernisation program.

Planned improvements include:

* Modern C++ development practices.
* CMake build support.
* Improved FFmpeg integration.
* Simplified workflow.
* Improved documentation.
* Audio fade-in and fade-out support.
* Final-frame hold support.
* Improved movie production tools.

The goal is to preserve the proven functionality of the existing application while modernising the codebase and improving usability.

---

## Documentation

Project planning and modernisation documents are located in the `docs` directory.

Primary document:

- ManpMovieMaker Modernisation.docx**

---

## FFmpeg

ManpMovieMaker uses FFmpeg to create MP4 movies.

FFmpeg is not distributed with ManpMovieMaker and must be installed separately. Future documentation will include detailed installation and configuration instructions.

---

## Related Projects

### ManpWIN

ManpMovieMaker was developed as a companion application for ManpWIN, a Windows fractal generation and exploration system supporting:

* Deep zoom rendering
* Perturbation theory
* Bilinear Approximation (BLA)
* Double-double, quad-double and arbitrary precision arithmetic
* Formula parsing
* Fractal animation generation

Repository:

https://github.com/PaulTheLionHeart/manpwin

---

## Acknowledgements

ManpMovieMaker was created by **Paul the LionHeart**.

The ongoing modernisation, documentation, architectural planning, testing strategies, and development discussions have been greatly assisted through collaboration with **ChatGPT**, acting as a workshop assistant, reviewer, teacher, sounding board, and enthusiastic dragon spotter.

### Credits

| Contributor        | Role               |
| ------------------ | ------------------ |
| Paul the LionHeart | Author             |
| ChatGPT            | Workshop Assistant |

---

*"Fun, beauty and passion."*
