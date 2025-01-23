tinyCLUNX33 Zephyr Example
##########################

This repository acts as an example project, and contain several sample
applications that can be used as a starting point for writing new firmware
for the tinyCLUNX33 SoM.

In order to get started, first follow Zephyr's
`Getting Started <https://docs.zephyrproject.org/latest/develop/getting_started/index.html>`_
instructions.

Then, setup the project to use
`tinyVision.ai Zephyr SDK <https://github.com/tinyvision-ai-inc/tinyvision_zephyr_sdk>`_:

.. code-block:: console

   # Reset the workspace directory you created from Zephyr's Getting Started Guide
   cd ~/zephyrproject
   rm -rf .west

   # Clone the example repository recursively
   west init -m https://github.com/tinyvision-ai-inc/tinyclunx33_zephyr_example
   west update

You are now ready to build any of the provided examples, in the ``app_...``
directories of this repository, each with a ``README`` file providing further
build instructions.

An end-to-end example of how to build the ``app_imx219`` is provided via the
``getting_started_on_linux.sh``, expected to build a working ``zephyr.elf``
binary.
