Include six tasks :

localPlay_init_task -->
                     1.1 Sdcard_task -->
                                      1.1.1 sd_player_task -->
                                                             1.1.1.1 pcm_flush_task 
                                      1.1.2 sd_file_search_task
                     1.2 Shell_task
                     
                     
                     
                     
                     
issue:

1: allco memory failed sometimes.       when use "_mem_alloc_system_zero()", allocate memory failed sometimes; when use "_mem_alloc_zero", no voice sometimes.
2: when usb pulg-in power on,   _lwsem_poll() run before _lwsem_create().


