void bootmain(void) {
    while (1) {
        __asm__ volatile ("hlt" :::);
    }
}
