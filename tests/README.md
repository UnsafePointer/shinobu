# Tests

The `test-locations.txt` files have the absolute location of the test ROMs, you can test multiple ROMs by executing:

```Bash
$ ./tests/run.sh tests/blargg/test-locations.txt
```

## Results

### Blargg's tests

| Test            |                  |
|-----------------|------------------|
|cpu_instrs.gb    |:heavy_check_mark:|
|instr_timing.gb  |:heavy_check_mark:|
|mem_timing.gb    |:heavy_check_mark:|
|mem_timing-2.gb  |:heavy_check_mark:|
|interrupt_time.gb|:heavy_check_mark:|

### Mooneye GB's interrupt tests

| Test                     |                  |
|--------------------------|------------------|
|ei_sequence.gb            |:x:               |
|if_ie_registers.gb        |:x:               |
|ei_timing.gb              |:heavy_check_mark:|
|halt_ime0_ei.gb           |:heavy_check_mark:|
|halt_ime0_nointr_timing.gb|:heavy_check_mark:|
|halt_ime1_timing.gb       |:heavy_check_mark:|
|intr_timing.gb            |:heavy_check_mark:|

### Mooneye GB's OAM DMA timing tests

| Test                     |                  |
|--------------------------|------------------|
|basic.gb                  |:heavy_check_mark:|
|reg_read.gb               |:heavy_check_mark:|
|sources-GS.gb             |:heavy_check_mark:|
|oam_dma_restart.gb        |:heavy_check_mark:|
|oam_dma_start.gb          |:heavy_check_mark:|
|oam_dma_timing.gb         |:heavy_check_mark:|

### Mooneye GB's timer tests

| Test                     |                  |
|--------------------------|------------------|
|div_write.gb              |:heavy_check_mark:|
|rapid_toggle.gb           |:heavy_check_mark:|
|tim00_div_trigger.gb      |:heavy_check_mark:|
|tim00.gb                  |:heavy_check_mark:|
|tim01_div_trigger.gb      |:heavy_check_mark:|
|tim01.gb                  |:heavy_check_mark:|
|tim10_div_trigger.gb      |:heavy_check_mark:|
|tim10.gb                  |:heavy_check_mark:|
|tim11_div_trigger.gb      |:heavy_check_mark:|
|tim11.gb                  |:heavy_check_mark:|
|tima_reload.gb            |:heavy_check_mark:|
|tima_write_reloading.gb   |:x:               |
|tma_write_reloading.gb    |:x:               |
