# Animation Theory

## Introduction

Creating a smooth animation normally requires a large number of individual frames.

For complex fractal images, rendering every frame can be extremely time-consuming.

ManpMovieMaker reduces rendering time by generating intermediate frames between rendered images.

This approach allows high-quality movies to be created from a much smaller number of rendered source images.

---

## Why Are Intermediate Frames Needed?

Movies are created by displaying a sequence of still images.

Common frame rates include:

* 24 frames per second
* 25 frames per second
* 30 frames per second
* 60 frames per second

A 10-minute movie at 25 frames per second requires:

```text
25 × 60 × 10 = 15,000 frames
```

Rendering every frame directly is often impractical.

---

## Fractal Zoom Animations

In a zoom animation, each image is rendered at a different magnification.

As magnification increases, rendering time often increases dramatically.

Deep zoom animations may require:

* Thousands of rendered images
* Many hours or days of computation
* High-precision arithmetic

For large projects, rendering every movie frame directly may not be practical.

---

## Intermediate Frame Generation

ManpMovieMaker generates additional frames between rendered images.

The program analyses the zoom ratio between adjacent images and creates intermediate frames by resizing and cropping.

The resulting image sequence appears smooth while requiring significantly fewer rendered source images.

---

## Example

Suppose a movie requires 15,000 frames.

One approach would be to render all 15,000 frames directly.

Alternatively, the movie could be created from approximately 3,000 to 5,000 rendered images, with several intermediate frames generated between each rendered image.

In practice, many animations achieve excellent results using 3 to 4 inserted frames between rendered images. Larger values may further reduce rendering times but can lead to a gradual reduction in image quality.

The optimum balance depends on the type of animation and the quality requirements of the final movie.

---

## Quality Versus Rendering Time

ManpMovieMaker provides a trade-off between rendering time and image quality.

Increasing the number of inserted frames:

* Reduces the number of images that must be rendered.
* Reduces total rendering time.
* Increases image processing.
* May reduce visual quality if taken too far.

For best results, a sufficient number of high-quality source images should still be rendered.

ManpMovieMaker is designed to reduce rendering effort, not eliminate it.

---

## Limitations

Intermediate frame generation works best when changes between rendered images are relatively small.

Large jumps in magnification may produce visible stepping or loss of smoothness.

Selecting an appropriate number of rendered images remains important.

---

## Summary

ManpMovieMaker trades image processing for rendering time.

For many fractal animations this can reduce computation by a substantial amount while maintaining excellent visual quality.

By combining a modest number of rendered source images with carefully generated intermediate frames, high-quality movies can be created in a fraction of the time required to render every frame directly.
