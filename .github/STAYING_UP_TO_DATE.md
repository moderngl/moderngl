# Staying up-to-date

All non-backward compatible changes will be logged here.

## How to use this file?

- Scroll down until you find the version you are using currently.
- Start scrolling up until you reach the top.
- Do all the proposed changes.
- Report all the issues you find.

## Versions

### I am using \_.\_.\_

> work in progress

`Context.default_framebuffer` is deprecated. Please change `default_framebuffer` to `screen`, they are the same.
The `Context.default_framebuffer` will be removed later.

```python
ctx = ModernGL.create_context()
# ctx.default_framebuffer.use()   # bad
ctx.screen.use()                  # good
```

`Context.screen` is the framebuffer 0. If you need Qt's default framebuffer, use `Context.detect_framebuffer`.

```python
ctx = ModernGL.create_context()
screen = Context.detect_framebuffer()
screen.use()
```

### I am using 4.2.0

This is the very end of this file.
Earlier versions are not yet added.
