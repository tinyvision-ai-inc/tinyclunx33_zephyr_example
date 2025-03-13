# Copyright (c) 2024 tinyVision.ai Inc.
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_VIDEO_TINYVISION_EXAMPLE example.c)
#zephyr_library_sources_ifdef(CONFIG_VIDEO_AR0144 ar0144.c)
