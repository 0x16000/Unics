#ifndef _HDMI_H_
#define _HDMI_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* HDMI Register Base Addresses (platform specific - adjust as needed) */
#define HDMI_BASE_ADDR          0xFE902000  /* BCM2835/2836 HDMI base */
#define HDMI_CORE_BASE          (HDMI_BASE_ADDR + 0x000)
#define HDMI_PHY_BASE           (HDMI_BASE_ADDR + 0x300)

/* HDMI Core Registers */
#define HDMI_CORE_CTRL          0x000
#define HDMI_CORE_STATUS        0x004
#define HDMI_CORE_INTR_MASK     0x008
#define HDMI_CORE_INTR_STATUS   0x00C
#define HDMI_CORE_VIDEO_CFG     0x010
#define HDMI_CORE_AUDIO_CFG     0x014
#define HDMI_CORE_HOTPLUG       0x018
#define HDMI_CORE_CEC           0x01C

/* HDMI Video Configuration */
#define HDMI_VIDEO_TIMING       0x020
#define HDMI_VIDEO_HACTIVE      0x024
#define HDMI_VIDEO_VACTIVE      0x028
#define HDMI_VIDEO_HSYNC        0x02C
#define HDMI_VIDEO_VSYNC        0x030
#define HDMI_VIDEO_HBLANK       0x034
#define HDMI_VIDEO_VBLANK       0x038

/* HDMI Audio Configuration */
#define HDMI_AUDIO_SAMPLE_RATE  0x040
#define HDMI_AUDIO_CHANNELS     0x044
#define HDMI_AUDIO_FORMAT       0x048

/* HDMI Control Bits */
#define HDMI_CTRL_ENABLE        (1 << 0)
#define HDMI_CTRL_RESET         (1 << 1)
#define HDMI_CTRL_AUDIO_EN      (1 << 2)
#define HDMI_CTRL_VIDEO_EN      (1 << 3)
#define HDMI_CTRL_HDCP_EN       (1 << 4)

/* HDMI Status Bits */
#define HDMI_STATUS_CONNECTED   (1 << 0)
#define HDMI_STATUS_STABLE      (1 << 1)
#define HDMI_STATUS_AUDIO_READY (1 << 2)
#define HDMI_STATUS_VIDEO_READY (1 << 3)

/* Video Resolution Presets */
typedef enum {
    HDMI_RES_640x480_60Hz,
    HDMI_RES_800x600_60Hz,
    HDMI_RES_1024x768_60Hz,
    HDMI_RES_1280x720_60Hz,
    HDMI_RES_1920x1080_60Hz,
    HDMI_RES_3840x2160_30Hz,
    HDMI_RES_CUSTOM
} hdmi_resolution_t;

/* Color Depth */
typedef enum {
    HDMI_COLOR_DEPTH_8BIT = 8,
    HDMI_COLOR_DEPTH_10BIT = 10,
    HDMI_COLOR_DEPTH_12BIT = 12,
    HDMI_COLOR_DEPTH_16BIT = 16
} hdmi_color_depth_t;

/* Audio Sample Rates */
typedef enum {
    HDMI_AUDIO_32KHZ = 32000,
    HDMI_AUDIO_44KHZ = 44100,
    HDMI_AUDIO_48KHZ = 48000,
    HDMI_AUDIO_96KHZ = 96000,
    HDMI_AUDIO_192KHZ = 192000
} hdmi_audio_rate_t;

/* Video Timing Structure */
typedef struct {
    uint32_t pixel_clock;       /* Pixel clock in Hz */
    uint16_t h_active;          /* Horizontal active pixels */
    uint16_t h_blanking;        /* Horizontal blanking pixels */
    uint16_t h_sync_offset;     /* Horizontal sync offset */
    uint16_t h_sync_width;      /* Horizontal sync width */
    uint16_t v_active;          /* Vertical active lines */
    uint16_t v_blanking;        /* Vertical blanking lines */
    uint16_t v_sync_offset;     /* Vertical sync offset */
    uint16_t v_sync_width;      /* Vertical sync width */
    bool h_sync_polarity;       /* Horizontal sync polarity (true = positive) */
    bool v_sync_polarity;       /* Vertical sync polarity (true = positive) */
    bool interlaced;            /* Interlaced mode */
} hdmi_video_timing_t;

/* Audio Configuration Structure */
typedef struct {
    hdmi_audio_rate_t sample_rate;
    uint8_t channels;           /* Number of audio channels (2, 6, 8) */
    uint8_t bit_depth;          /* Audio bit depth (16, 20, 24) */
    bool compressed;            /* Compressed audio format */
} hdmi_audio_config_t;

/* HDMI Device Configuration */
typedef struct {
    hdmi_resolution_t resolution;
    hdmi_color_depth_t color_depth;
    hdmi_video_timing_t video_timing;
    hdmi_audio_config_t audio_config;
    bool audio_enabled;
    bool hdcp_enabled;
    uint32_t base_addr;
} hdmi_config_t;

/* HDMI Device State */
typedef struct {
    hdmi_config_t config;
    bool initialized;
    bool connected;
    bool stable;
    uint32_t interrupt_mask;
} hdmi_device_t;

/* Function Prototypes */

/* Core HDMI Functions */
int hdmi_init(hdmi_device_t *hdmi, uint32_t base_addr);
void hdmi_deinit(hdmi_device_t *hdmi);
int hdmi_reset(hdmi_device_t *hdmi);
bool hdmi_is_connected(hdmi_device_t *hdmi);
bool hdmi_is_stable(hdmi_device_t *hdmi);

/* Video Configuration */
int hdmi_set_resolution(hdmi_device_t *hdmi, hdmi_resolution_t resolution);
int hdmi_set_custom_timing(hdmi_device_t *hdmi, const hdmi_video_timing_t *timing);
int hdmi_set_color_depth(hdmi_device_t *hdmi, hdmi_color_depth_t depth);
int hdmi_enable_video(hdmi_device_t *hdmi, bool enable);

/* Audio Configuration */
int hdmi_set_audio_config(hdmi_device_t *hdmi, const hdmi_audio_config_t *config);
int hdmi_enable_audio(hdmi_device_t *hdmi, bool enable);

/* Control Functions */
int hdmi_enable(hdmi_device_t *hdmi, bool enable);
int hdmi_enable_hdcp(hdmi_device_t *hdmi, bool enable);
uint32_t hdmi_get_status(hdmi_device_t *hdmi);

/* Interrupt Handling */
int hdmi_set_interrupt_mask(hdmi_device_t *hdmi, uint32_t mask);
uint32_t hdmi_get_interrupt_status(hdmi_device_t *hdmi);
void hdmi_clear_interrupts(hdmi_device_t *hdmi, uint32_t mask);
void hdmi_interrupt_handler(hdmi_device_t *hdmi);

/* Utility Functions */
const char* hdmi_resolution_to_string(hdmi_resolution_t resolution);
hdmi_video_timing_t hdmi_get_standard_timing(hdmi_resolution_t resolution);
int hdmi_validate_timing(const hdmi_video_timing_t *timing);

/* Register Access Helpers */
uint32_t hdmi_read_reg(uint32_t base_addr, uint32_t offset);
void hdmi_write_reg(uint32_t base_addr, uint32_t offset, uint32_t value);
void hdmi_modify_reg(uint32_t base_addr, uint32_t offset, uint32_t mask, uint32_t value);

/* Debug Functions */
void hdmi_dump_registers(hdmi_device_t *hdmi);
void hdmi_print_config(const hdmi_device_t *hdmi);

#endif /* _HDMI_H_ */

