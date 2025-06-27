#include "hdmi.h"
#include <stdio.h>
#include <stdlib.h>
#include <machine/pio.h>

/* Standard video timing presets */
static const hdmi_video_timing_t standard_timings[] = {
    /* 640x480@60Hz */
    {
        .pixel_clock = 25175000,
        .h_active = 640, .h_blanking = 160, .h_sync_offset = 16, .h_sync_width = 96,
        .v_active = 480, .v_blanking = 45, .v_sync_offset = 10, .v_sync_width = 2,
        .h_sync_polarity = false, .v_sync_polarity = false, .interlaced = false
    },
    /* 800x600@60Hz */
    {
        .pixel_clock = 40000000,
        .h_active = 800, .h_blanking = 256, .h_sync_offset = 40, .h_sync_width = 128,
        .v_active = 600, .v_blanking = 28, .v_sync_offset = 1, .v_sync_width = 4,
        .h_sync_polarity = true, .v_sync_polarity = true, .interlaced = false
    },
    /* 1024x768@60Hz */
    {
        .pixel_clock = 65000000,
        .h_active = 1024, .h_blanking = 320, .h_sync_offset = 24, .h_sync_width = 136,
        .v_active = 768, .v_blanking = 38, .v_sync_offset = 3, .v_sync_width = 6,
        .h_sync_polarity = false, .v_sync_polarity = false, .interlaced = false
    },
    /* 1280x720@60Hz (720p) */
    {
        .pixel_clock = 74250000,
        .h_active = 1280, .h_blanking = 370, .h_sync_offset = 110, .h_sync_width = 40,
        .v_active = 720, .v_blanking = 30, .v_sync_offset = 5, .v_sync_width = 5,
        .h_sync_polarity = true, .v_sync_polarity = true, .interlaced = false
    },
    /* 1920x1080@60Hz (1080p) */
    {
        .pixel_clock = 148500000,
        .h_active = 1920, .h_blanking = 280, .h_sync_offset = 88, .h_sync_width = 44,
        .v_active = 1080, .v_blanking = 45, .v_sync_offset = 4, .v_sync_width = 5,
        .h_sync_polarity = true, .v_sync_polarity = true, .interlaced = false
    },
    /* 3840x2160@30Hz (4K) */
    {
        .pixel_clock = 297000000,
        .h_active = 3840, .h_blanking = 560, .h_sync_offset = 176, .h_sync_width = 88,
        .v_active = 2160, .v_blanking = 90, .v_sync_offset = 8, .v_sync_width = 10,
        .h_sync_polarity = true, .v_sync_polarity = true, .interlaced = false
    }
};

/* Register access functions */
uint32_t hdmi_read_reg(uint32_t base_addr, uint32_t offset)
{
    volatile uint32_t *reg = (volatile uint32_t *)(base_addr + offset);
    return *reg;
}

void hdmi_write_reg(uint32_t base_addr, uint32_t offset, uint32_t value)
{
    volatile uint32_t *reg = (volatile uint32_t *)(base_addr + offset);
    *reg = value;
}

void hdmi_modify_reg(uint32_t base_addr, uint32_t offset, uint32_t mask, uint32_t value)
{
    uint32_t reg_val = hdmi_read_reg(base_addr, offset);
    reg_val = (reg_val & ~mask) | (value & mask);
    hdmi_write_reg(base_addr, offset, reg_val);
}

/* Core HDMI functions */
int hdmi_init(hdmi_device_t *hdmi, uint32_t base_addr)
{
    if (!hdmi) {
        return -1;
    }

    /* Initialize device structure */
    hdmi->config.base_addr = base_addr;
    hdmi->config.resolution = HDMI_RES_1920x1080_60Hz;
    hdmi->config.color_depth = HDMI_COLOR_DEPTH_8BIT;
    hdmi->config.audio_enabled = false;
    hdmi->config.hdcp_enabled = false;
    hdmi->initialized = false;
    hdmi->connected = false;
    hdmi->stable = false;
    hdmi->interrupt_mask = 0;

    /* Reset HDMI controller */
    if (hdmi_reset(hdmi) != 0) {
        return -1;
    }

    /* Set default video timing */
    hdmi->config.video_timing = standard_timings[HDMI_RES_1920x1080_60Hz];

    /* Set default audio configuration */
    hdmi->config.audio_config.sample_rate = HDMI_AUDIO_48KHZ;
    hdmi->config.audio_config.channels = 2;
    hdmi->config.audio_config.bit_depth = 16;
    hdmi->config.audio_config.compressed = false;

    /* Initialize interrupt mask */
    hdmi_set_interrupt_mask(hdmi, 0xFFFFFFFF);

    hdmi->initialized = true;
    return 0;
}

void hdmi_deinit(hdmi_device_t *hdmi)
{
    if (!hdmi || !hdmi->initialized) {
        return;
    }

    /* Disable HDMI output */
    hdmi_enable(hdmi, false);

    /* Reset controller */
    hdmi_reset(hdmi);

    /* Clear interrupt mask */
    hdmi_set_interrupt_mask(hdmi, 0);

    hdmi->initialized = false;
}

int hdmi_reset(hdmi_device_t *hdmi)
{
    if (!hdmi) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;

    /* Assert reset */
    hdmi_write_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_RESET);

    /* Wait for reset to complete (simple delay) */
    for (volatile int i = 0; i < 1000; i++);

    /* Clear reset */
    hdmi_write_reg(base, HDMI_CORE_CTRL, 0);

    /* Wait for controller to stabilize */
    for (volatile int i = 0; i < 1000; i++);

    return 0;
}

bool hdmi_is_connected(hdmi_device_t *hdmi)
{
    if (!hdmi || !hdmi->initialized) {
        return false;
    }

    uint32_t status = hdmi_read_reg(hdmi->config.base_addr, HDMI_CORE_STATUS);
    hdmi->connected = (status & HDMI_STATUS_CONNECTED) != 0;
    return hdmi->connected;
}

bool hdmi_is_stable(hdmi_device_t *hdmi)
{
    if (!hdmi || !hdmi->initialized) {
        return false;
    }

    uint32_t status = hdmi_read_reg(hdmi->config.base_addr, HDMI_CORE_STATUS);
    hdmi->stable = (status & HDMI_STATUS_STABLE) != 0;
    return hdmi->stable;
}

/* Video configuration functions */
int hdmi_set_resolution(hdmi_device_t *hdmi, hdmi_resolution_t resolution)
{
    if (!hdmi || !hdmi->initialized || resolution >= HDMI_RES_CUSTOM) {
        return -1;
    }

    hdmi->config.resolution = resolution;
    hdmi->config.video_timing = standard_timings[resolution];

    return hdmi_set_custom_timing(hdmi, &hdmi->config.video_timing);
}

int hdmi_set_custom_timing(hdmi_device_t *hdmi, const hdmi_video_timing_t *timing)
{
    if (!hdmi || !hdmi->initialized || !timing) {
        return -1;
    }

    if (hdmi_validate_timing(timing) != 0) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;

    /* Configure video timing registers */
    hdmi_write_reg(base, HDMI_VIDEO_HACTIVE, timing->h_active);
    hdmi_write_reg(base, HDMI_VIDEO_VACTIVE, timing->v_active);
    hdmi_write_reg(base, HDMI_VIDEO_HSYNC, 
                   (timing->h_sync_offset << 16) | timing->h_sync_width);
    hdmi_write_reg(base, HDMI_VIDEO_VSYNC, 
                   (timing->v_sync_offset << 16) | timing->v_sync_width);
    hdmi_write_reg(base, HDMI_VIDEO_HBLANK, timing->h_blanking);
    hdmi_write_reg(base, HDMI_VIDEO_VBLANK, timing->v_blanking);

    /* Set sync polarities and interlacing */
    uint32_t timing_reg = 0;
    if (timing->h_sync_polarity) timing_reg |= (1 << 0);
    if (timing->v_sync_polarity) timing_reg |= (1 << 1);
    if (timing->interlaced) timing_reg |= (1 << 2);
    hdmi_write_reg(base, HDMI_VIDEO_TIMING, timing_reg);

    hdmi->config.video_timing = *timing;
    return 0;
}

int hdmi_set_color_depth(hdmi_device_t *hdmi, hdmi_color_depth_t depth)
{
    if (!hdmi || !hdmi->initialized) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;
    uint32_t depth_bits = 0;

    switch (depth) {
        case HDMI_COLOR_DEPTH_8BIT:  depth_bits = 0; break;
        case HDMI_COLOR_DEPTH_10BIT: depth_bits = 1; break;
        case HDMI_COLOR_DEPTH_12BIT: depth_bits = 2; break;
        case HDMI_COLOR_DEPTH_16BIT: depth_bits = 3; break;
        default: return -1;
    }

    hdmi_modify_reg(base, HDMI_CORE_VIDEO_CFG, 0x3 << 4, depth_bits << 4);
    hdmi->config.color_depth = depth;
    return 0;
}

int hdmi_enable_video(hdmi_device_t *hdmi, bool enable)
{
    if (!hdmi || !hdmi->initialized) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;
    if (enable) {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_VIDEO_EN, HDMI_CTRL_VIDEO_EN);
    } else {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_VIDEO_EN, 0);
    }

    return 0;
}

/* Audio configuration functions */
int hdmi_set_audio_config(hdmi_device_t *hdmi, const hdmi_audio_config_t *config)
{
    if (!hdmi || !hdmi->initialized || !config) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;

    /* Set sample rate */
    hdmi_write_reg(base, HDMI_AUDIO_SAMPLE_RATE, config->sample_rate);

    /* Set channel configuration */
    hdmi_write_reg(base, HDMI_AUDIO_CHANNELS, config->channels);

    /* Set audio format */
    uint32_t format = config->bit_depth | (config->compressed ? (1 << 8) : 0);
    hdmi_write_reg(base, HDMI_AUDIO_FORMAT, format);

    hdmi->config.audio_config = *config;
    return 0;
}

int hdmi_enable_audio(hdmi_device_t *hdmi, bool enable)
{
    if (!hdmi || !hdmi->initialized) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;
    if (enable) {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_AUDIO_EN, HDMI_CTRL_AUDIO_EN);
    } else {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_AUDIO_EN, 0);
    }

    hdmi->config.audio_enabled = enable;
    return 0;
}

/* Control functions */
int hdmi_enable(hdmi_device_t *hdmi, bool enable)
{
    if (!hdmi || !hdmi->initialized) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;
    if (enable) {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_ENABLE, HDMI_CTRL_ENABLE);
    } else {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_ENABLE, 0);
    }

    return 0;
}

int hdmi_enable_hdcp(hdmi_device_t *hdmi, bool enable)
{
    if (!hdmi || !hdmi->initialized) {
        return -1;
    }

    uint32_t base = hdmi->config.base_addr;
    if (enable) {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_HDCP_EN, HDMI_CTRL_HDCP_EN);
    } else {
        hdmi_modify_reg(base, HDMI_CORE_CTRL, HDMI_CTRL_HDCP_EN, 0);
    }

    hdmi->config.hdcp_enabled = enable;
    return 0;
}

uint32_t hdmi_get_status(hdmi_device_t *hdmi)
{
    if (!hdmi || !hdmi->initialized) {
        return 0;
    }

    return hdmi_read_reg(hdmi->config.base_addr, HDMI_CORE_STATUS);
}

/* Interrupt handling */
int hdmi_set_interrupt_mask(hdmi_device_t *hdmi, uint32_t mask)
{
    if (!hdmi || !hdmi->initialized) {
        return -1;
    }

    hdmi_write_reg(hdmi->config.base_addr, HDMI_CORE_INTR_MASK, mask);
    hdmi->interrupt_mask = mask;
    return 0;
}

uint32_t hdmi_get_interrupt_status(hdmi_device_t *hdmi)
{
    if (!hdmi || !hdmi->initialized) {
        return 0;
    }

    return hdmi_read_reg(hdmi->config.base_addr, HDMI_CORE_INTR_STATUS);
}

void hdmi_clear_interrupts(hdmi_device_t *hdmi, uint32_t mask)
{
    if (!hdmi || !hdmi->initialized) {
        return;
    }

    hdmi_write_reg(hdmi->config.base_addr, HDMI_CORE_INTR_STATUS, mask);
}

void hdmi_interrupt_handler(hdmi_device_t *hdmi)
{
    if (!hdmi || !hdmi->initialized) {
        return;
    }

    uint32_t status = hdmi_get_interrupt_status(hdmi);
    
    /* Handle hotplug events */
    if (status & (1 << 0)) {
        hdmi->connected = hdmi_is_connected(hdmi);
        printf("HDMI: Hotplug event - %s\n", 
               hdmi->connected ? "connected" : "disconnected");
    }

    /* Handle stability changes */
    if (status & (1 << 1)) {
        hdmi->stable = hdmi_is_stable(hdmi);
        printf("HDMI: Stability event - %s\n", 
               hdmi->stable ? "stable" : "unstable");
    }

    /* Clear handled interrupts */
    hdmi_clear_interrupts(hdmi, status);
}

/* Utility functions */
const char* hdmi_resolution_to_string(hdmi_resolution_t resolution)
{
    switch (resolution) {
        case HDMI_RES_640x480_60Hz:   return "640x480@60Hz";
        case HDMI_RES_800x600_60Hz:   return "800x600@60Hz";
        case HDMI_RES_1024x768_60Hz:  return "1024x768@60Hz";
        case HDMI_RES_1280x720_60Hz:  return "1280x720@60Hz (720p)";
        case HDMI_RES_1920x1080_60Hz: return "1920x1080@60Hz (1080p)";
        case HDMI_RES_3840x2160_30Hz: return "3840x2160@30Hz (4K)";
        case HDMI_RES_CUSTOM:         return "Custom";
        default:                      return "Unknown";
    }
}

hdmi_video_timing_t hdmi_get_standard_timing(hdmi_resolution_t resolution)
{
    if (resolution < HDMI_RES_CUSTOM) {
        return standard_timings[resolution];
    }
    
    /* Return default timing for invalid resolution */
    return standard_timings[HDMI_RES_1920x1080_60Hz];
}

int hdmi_validate_timing(const hdmi_video_timing_t *timing)
{
    if (!timing) {
        return -1;
    }

    /* Basic validation checks */
    if (timing->h_active == 0 || timing->v_active == 0) {
        return -1;
    }

    if (timing->h_sync_width == 0 || timing->v_sync_width == 0) {
        return -1;
    }

    if (timing->pixel_clock < 1000000) { /* Minimum 1MHz */
        return -1;
    }

    return 0;
}

/* Debug functions */
void hdmi_dump_registers(hdmi_device_t *hdmi)
{
    if (!hdmi || !hdmi->initialized) {
        printf("HDMI: Device not initialized\n");
        return;
    }

    uint32_t base = hdmi->config.base_addr;
    
    printf("HDMI Register Dump:\n");
    printf("  CTRL:        0x%08X\n", hdmi_read_reg(base, HDMI_CORE_CTRL));
    printf("  STATUS:      0x%08X\n", hdmi_read_reg(base, HDMI_CORE_STATUS));
    printf("  INTR_MASK:   0x%08X\n", hdmi_read_reg(base, HDMI_CORE_INTR_MASK));
    printf("  INTR_STATUS: 0x%08X\n", hdmi_read_reg(base, HDMI_CORE_INTR_STATUS));
    printf("  VIDEO_CFG:   0x%08X\n", hdmi_read_reg(base, HDMI_CORE_VIDEO_CFG));
    printf("  AUDIO_CFG:   0x%08X\n", hdmi_read_reg(base, HDMI_CORE_AUDIO_CFG));
    printf("  H_ACTIVE:    0x%08X\n", hdmi_read_reg(base, HDMI_VIDEO_HACTIVE));
    printf("  V_ACTIVE:    0x%08X\n", hdmi_read_reg(base, HDMI_VIDEO_VACTIVE));
}

void hdmi_print_config(const hdmi_device_t *hdmi)
{
    if (!hdmi) {
        printf("HDMI: Invalid device\n");
        return;
    }

    printf("HDMI Configuration:\n");
    printf("  Initialized: %s\n", hdmi->initialized ? "Yes" : "No");
    printf("  Connected:   %s\n", hdmi->connected ? "Yes" : "No");
    printf("  Stable:      %s\n", hdmi->stable ? "Yes" : "No");
    printf("  Resolution:  %s\n", hdmi_resolution_to_string(hdmi->config.resolution));
    printf("  Color Depth: %d-bit\n", hdmi->config.color_depth);
    printf("  Audio:       %s\n", hdmi->config.audio_enabled ? "Enabled" : "Disabled");
    printf("  HDCP:        %s\n", hdmi->config.hdcp_enabled ? "Enabled" : "Disabled");
    
    const hdmi_video_timing_t *t = &hdmi->config.video_timing;
    printf("  Video Timing:\n");
    printf("    Active:      %dx%d\n", t->h_active, t->v_active);
    printf("    Pixel Clock: %u Hz\n", t->pixel_clock);
    printf("    H Blanking:  %d\n", t->h_blanking);
    printf("    V Blanking:  %d\n", t->v_blanking);
    printf("    H Sync:      %d+%d\n", t->h_sync_offset, t->h_sync_width);
    printf("    V Sync:      %d+%d\n", t->v_sync_offset, t->v_sync_width);
    printf("    Interlaced:  %s\n", t->interlaced ? "Yes" : "No");
    
    if (hdmi->config.audio_enabled) {
        const hdmi_audio_config_t *a = &hdmi->config.audio_config;
        printf("  Audio Config:\n");
        printf("    Sample Rate: %u Hz\n", a->sample_rate);
        printf("    Channels:    %d\n", a->channels);
        printf("    Bit Depth:   %d\n", a->bit_depth);
        printf("    Compressed:  %s\n", a->compressed ? "Yes" : "No");
    }
}

