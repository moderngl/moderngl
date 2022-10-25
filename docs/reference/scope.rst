Scope
=====

.. py:currentmodule:: moderngl

.. autoclass:: moderngl.Scope

Create
------

.. automethod:: Context.scope
   :noindex:

Methods
-------

.. automethod:: Scope.__enter__
.. automethod:: Scope.__exit__
.. automethod:: Scope.release

Attributes
----------

.. autoattribute:: Scope.extra
.. autoattribute:: Scope.mglo
.. autoattribute:: Scope.ctx

Examples
--------

.. rubric:: Simple scope example

.. code-block:: python

    scope1 = ctx.scope(fbo1, moderngl.BLEND)
    scope2 = ctx.scope(fbo2, moderngl.DEPTH_TEST | moderngl.CULL_FACE)

    with scope1:
        # do some rendering

    with scope2:
        # do some rendering

.. rubric:: Scope for querying

.. code-block:: python

    query = ctx.query(samples=True)
    scope = ctx.scope(ctx.screen, moderngl.DEPTH_TEST | moderngl.RASTERIZER_DISCARD)

    with scope, query:
        # do some rendering

    print(query.samples)

.. rubric:: Understanding what scope objects do

.. code-block:: python

    scope = ctx.scope(
        framebuffer=framebuffer1,
        enable_only=moderngl.BLEND,
        textures=[
            (texture1, 4),
            (texture2, 3),
        ],
        uniform_buffers=[
            (buffer1, 6),
            (buffer2, 5),
        ],
        storage_buffers=[
            (buffer3, 8),
        ],
    )

    # Let's assume we have some state before entering the scope
    some_random_framebuffer.use()
    some_random_texture.use(3)
    some_random_buffer.bind_to_uniform_block(5)
    some_random_buffer.bind_to_storage_buffer(8)
    ctx.enable_only(moderngl.DEPTH_TEST)

    with scope:
        # on __enter__
        #     framebuffer1.use()
        #     ctx.enable_only(moderngl.BLEND)
        #     texture1.use(4)
        #     texture2.use(3)
        #     buffer1.bind_to_uniform_block(6)
        #     buffer2.bind_to_uniform_block(5)
        #     buffer3.bind_to_storage_buffer(8)

        # do some rendering

        # on __exit__
        #     some_random_framebuffer.use()
        #     ctx.enable_only(moderngl.DEPTH_TEST)

    # Originally we had the following, let's see what was changed
    some_random_framebuffer.use()                 # This was restored hurray!
    some_random_texture.use(3)                    # Have to restore it manually.
    some_random_buffer.bind_to_uniform_block(5)   # Have to restore it manually.
    some_random_buffer.bind_to_storage_buffer(8)  # Have to restore it manually.
    ctx.enable_only(moderngl.DEPTH_TEST)          # This was restored too.

    # Scope objects only do as much as necessary.
    # Restoring the framebuffer and enable flags are lowcost operations and
    # without them you could get a hard time debugging the application.

.. toctree::
    :maxdepth: 2
