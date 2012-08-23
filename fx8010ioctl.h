#ifndef _FX8010_IOCTL_H
#define _FX8010_IOCTL_H

#include <linux/types.h>
#include <alsa/hwdep.h>
enum sndrv_hwdep_iface {
	SNDRV_HWDEP_IFACE_OPL2 = 0,
	SNDRV_HWDEP_IFACE_OPL3,
	SNDRV_HWDEP_IFACE_OPL4,
	SNDRV_HWDEP_IFACE_SB16CSP,	/* Creative Signal Processor */
	SNDRV_HWDEP_IFACE_EMU10K1,	/* FX8010 processor in EMU10K1 chip */
	SNDRV_HWDEP_IFACE_YSS225,	/* Yamaha FX processor */
	SNDRV_HWDEP_IFACE_ICS2115,	/* Wavetable synth */
	SNDRV_HWDEP_IFACE_SSCAPE,	/* Ensoniq SoundScape ISA card
					 * (MC68EC000) */

	/*
	 * Don't forget to change the following: 
	 */
	SNDRV_HWDEP_IFACE_LAST = SNDRV_HWDEP_IFACE_SSCAPE,
};

enum sndrv_ctl_elem_iface {
	SNDRV_CTL_ELEM_IFACE_CARD = 0,	/* global control */
	SNDRV_CTL_ELEM_IFACE_HWDEP,	/* hardware dependent device */
	SNDRV_CTL_ELEM_IFACE_MIXER,	/* virtual mixer device */
	SNDRV_CTL_ELEM_IFACE_PCM,	/* PCM device */
	SNDRV_CTL_ELEM_IFACE_RAWMIDI,	/* RawMidi device */
	SNDRV_CTL_ELEM_IFACE_TIMER,	/* timer device */
	SNDRV_CTL_ELEM_IFACE_SEQUENCER,	/* sequencer client */
	SNDRV_CTL_ELEM_IFACE_LAST = SNDRV_CTL_ELEM_IFACE_SEQUENCER,
};

struct sndrv_ctl_elem_id {
	unsigned int numid;	/* numeric identifier, zero = invalid */
	enum sndrv_ctl_elem_iface iface;	/* interface identifier */
	unsigned int device;	/* device/client number */
	unsigned int subdevice;	/* subdevice (substream) number */
	unsigned char name[44];	/* ASCII name of item */
	unsigned int index;	/* index of item */
};

typedef struct sndrv_ctl_elem_id snd_ctl_elem_id_t;

#include <sound/emu10k1.h>

#endif
