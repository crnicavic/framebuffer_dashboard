# framebuffer_dashboard

A small C library for making interactive dashboards on framebuffer devices with tslib for touch support. This was built for making a control panel on a waveshare SPI display.

The library features an entire dynamic rendering system and optional callbacks for maximum flexibility.

## Non goals
This library was built for a very specific purpose, and unless something radically changes, features one might wish for will not be implemented:
- GPU accelaration
- Framework-esque architecture
- Wide hardware support

## Getting started

Bare bones examples are inside of the `tests/` directory, and should be sufficiently self-explanatory.

## TODOs

The library does almost everything i need it to, but there are a few things that might need to be worked out:
 - [ ] Touch debouncing - Reading the touch event device too fast results in ts_read reading nothing, which is detected as there being no touch
 - [x] Heap allocation - currently all of the elements are on the stack, and that forces me to use some hacky ways to extend lifetimes of variables
