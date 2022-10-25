.. py:currentmodule:: moderngl

An introduction to OpenGL
=========================

The simplified story
--------------------

`OpenGL`_ (Open Graphics Library) has a long history reaching
all the way back to 1992 when it was created by `Silicon Graphics`_.
It was partly based in their proprietary `IRIS GL`_ 
(Integrated Raster Imaging System Graphics Library) library.

Today OpenGL is managed by the `Khronos Group`_, an open 
industry consortium of over 150 leading hardware and software
companies creating advanced, royalty-free, acceleration
standards for 3D graphics, Augmented and Virtual Reality,
vision and machine learning

The purpose of `OpenGL`_ is to provide a standard way to interact
with the graphics processing unit to achieve hardware accelerated rendering
across several platforms. How this is done under the hood is up to the
vendors (AMD, Nvidia, Intel, ARM .. etc) as long as the the specifications are
followed.

`OpenGL`_ have gone though many versions and it can be confusing when looking
up resources. Today we separate "Old OpenGL" and "Modern OpenGL".
From 2008 to 2010 version 3.x of OpenGL evolved until version
3.3 and 4.0 was released simultaneously.

In 2010 version 3.3, 4.0 and 4.1 was released to modernize the api
(simplified explanation) creating something that would be able
to utilize Direct3D 11-class hardware. **OpenGL 3.3 is the first
"Modern OpenGL" version** (simplified explanation). Everything
from this version is forward compatible all the way to the latest
4.x version. An optional deprecation mechanism was introduced to
disable outdated features. Running OpenGL in **core mode** would
remove all old features while running in **compatibility mode**
would still allow mixing the old and new api.

.. Note:: OpenGL 2.x, 3.0, 3.1 and 3.2 can of course access some
          modern OpenGL features directly, but for simplicity we are
          are focused on version 3.3 as it created the final
          standard we are using today. Older OpenGL was also
          a pretty wild world with countless vendor specific
          extensions. Modern OpenGL cleaned this up quite a bit.

In OpenGL we often talk about the **Fixed Pipeline** and the
**Programmable Pipeline**.

OpenGL code using the **Fixed Pipeline** (Old OpenGL) would use functions like
``glVertex``, ``glColor``, ``glMaterial`` ``glMatrixMode``,
``glLoadIdentity``, ``glBegin``, ``glEnd``, ``glVertexPointer``,
``glColorPointer``, ``glPushMatrix`` and ``glPopMatrix``.
The api had strong opinions and limitations on what you
could do hiding what really went on under the hood.

OpenGL code using the **Programmable Pipeline** (Modern OpenGL) would use
functions like ``glCreateProgram``, ``UseProgram``. ``glCreateShader``,
``VertexAttrib*``, ``glBindBuffer*``, ``glUniform*``.
This API mainly works with buffers of data and smaller programs
called "shaders" running on the GPU to process this data
using the **OpenGL Shading Language (GLSL)**. This gives
enormous flexibility but requires that we understand the
OpenGL pipeline (actually not that complicated).

Beyond OpenGL
-------------

OpenGL has a lot of "baggage" after 25 years and hardware have
drastically changed since its inception. Plans for "OpenGL 5"
was started as the **Next Generation OpenGL Initiative (glNext)**.
This Turned into the `Vulkan`_ API and was a grounds-up redesign
to unify OpenGL and OpenGL ES into one common API that will not be
backwards compatible with existing OpenGL versions.

This doesn't mean OpenGL is not worth learning today. In fact
learning 3.3+ shaders and understanding the rendering pipeline
will greatly help you understand `Vulkan`_. In most cases you can
pretty much copy paste the shaders over to `Vulkan`_.

Where do ModernGL fit into all this?
------------------------------------

The ModernGL library exposes the **Programmable Pipeline**
using OpenGL 3.3 core or higher. However, we don't expose OpenGL
functions directly. Instead we expose features though various
objects like :py:class:`Buffer` and :py:class:`Program`
in a much more "pythonic" way. It's in other words a higher level
wrapper making OpenGL much easier to reason with. We try to hide
most of the complicated details to make the user more productive.
There are a lot of pitfalls with OpenGL and we remove most of them.

Learning ModernGL is more about learning shaders and the OpenGL
pipeline.

.. _Vulkan: https://www.khronos.org/vulkan/
.. _IRIS GL: https://wikipedia.org/wiki/IRIS_GL
.. _OpenGL: https://en.wikipedia.org/wiki/OpenGL
.. _Silicon Graphics: https://wikipedia.org/wiki/Silicon_Graphics
.. _Khronos Group: https://www.khronos.org
