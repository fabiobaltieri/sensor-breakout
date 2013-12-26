void flash_start(void);
int flash_can_write(uint16_t size);
void flash_write(const uint8_t *buf, uint16_t size);
void flash_end_write(void);
uint16_t flash_read(uint8_t *buf, uint16_t size);
