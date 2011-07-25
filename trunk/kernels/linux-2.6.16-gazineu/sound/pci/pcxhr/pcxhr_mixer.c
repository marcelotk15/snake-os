#define __NO_VERSION__
/*
 * Driver for Digigram pcxhr compatible soundcards
 *
 * mixer callbacks
 *
 * Copyright (c) 2004 by Digigram <alsa@digigram.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include <sound/driver.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <sound/core.h>
#include "pcxhr.h"
#include "pcxhr_hwdep.h"
#include "pcxhr_core.h"
#include <sound/control.h>
#include <sound/asoundef.h>
#include "pcxhr_mixer.h"


#define PCXHR_ANALOG_CAPTURE_LEVEL_MIN   0	/* -96.0 dB */
#define PCXHR_ANALOG_CAPTURE_LEVEL_MAX   255	/* +31.5 dB */
#define PCXHR_ANALOG_CAPTURE_ZERO_LEVEL  224	/* +16.0 dB ( +31.5 dB - fix level +15.5 dB ) */

#define PCXHR_ANALOG_PLAYBACK_LEVEL_MIN  0	/* -128.0 dB */
#define PCXHR_ANALOG_PLAYBACK_LEVEL_MAX  128	/*    0.0 dB */
#define PCXHR_ANALOG_PLAYBACK_ZERO_LEVEL 104	/*  -24.0 dB ( 0.0 dB - fix level +24.0 dB ) */

static int pcxhr_update_analog_audio_level(struct snd_pcxhr *chip, int is_capture, int channel)
{
	int err, vol;
	struct pcxhr_rmh rmh;

	pcxhr_init_rmh(&rmh, CMD_ACCESS_IO_WRITE);
	if (is_capture) {
		rmh.cmd[0] |= IO_NUM_REG_IN_ANA_LEVEL;
		rmh.cmd[2] = chip->analog_capture_volume[channel];
	} else {
		rmh.cmd[0] |= IO_NUM_REG_OUT_ANA_LEVEL;
		if (chip->analog_playback_active[channel])
			vol = chip->analog_playback_volume[channel];
		else
			vol = PCXHR_ANALOG_PLAYBACK_LEVEL_MIN;
		rmh.cmd[2] = PCXHR_ANALOG_PLAYBACK_LEVEL_MAX - vol;	/* playback analog levels are inversed */
	}
	rmh.cmd[1]  = 1 << ((2 * chip->chip_idx) + channel);	/* audio mask */
	rmh.cmd_len = 3;
	err = pcxhr_send_msg(chip->mgr, &rmh);
	if (err < 0) {
		snd_printk(KERN_DEBUG "error update_analog_audio_level card(%d) "
			   "is_capture(%d) err(%x)\n", chip->chip_idx, is_capture, err);
		return -EINVAL;
	}
	return 0;
}

/*
 * analog level control
 */
static int pcxhr_analog_vol_info(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	if (kcontrol->private_value == 0) {	/* playback */
		uinfo->value.integer.min = PCXHR_ANALOG_PLAYBACK_LEVEL_MIN;	/* -128 dB */
		uinfo->value.integer.max = PCXHR_ANALOG_PLAYBACK_LEVEL_MAX;	/* 0 dB */
	} else {				/* capture */
		uinfo->value.integer.min = PCXHR_ANALOG_CAPTURE_LEVEL_MIN;	/* -96 dB */
		uinfo->value.integer.max = PCXHR_ANALOG_CAPTURE_LEVEL_MAX;	/* 31.5 dB */
	}
	return 0;
}

static int pcxhr_analog_vol_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	down(&chip->mgr->mixer_mutex);
	if (kcontrol->private_value == 0) {	/* playback */
		ucontrol->value.integer.value[0] = chip->analog_playback_volume[0];
		ucontrol->value.integer.value[1] = chip->analog_playback_volume[1];
	} else {				/* capture */
		ucontrol->value.integer.value[0] = chip->analog_capture_volume[0];
		ucontrol->value.integer.value[1] = chip->analog_capture_volume[1];
	}
	up(&chip->mgr->mixer_mutex);
	return 0;
}

static int pcxhr_analog_vol_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int changed = 0;
	int is_capture, i;

	down(&chip->mgr->mixer_mutex);
	is_capture = (kcontrol->private_value != 0);
	for (i = 0; i < 2; i++) {
		int  new_volume = ucontrol->value.integer.value[i];
		int* stored_volume = is_capture ? &chip->analog_capture_volume[i] :
			&chip->analog_playback_volume[i];
		if (*stored_volume != new_volume) {
			*stored_volume = new_volume;
			changed = 1;
			pcxhr_update_analog_audio_level(chip, is_capture, i);
		}
	}
	up(&chip->mgr->mixer_mutex);
	return changed;
}

static struct snd_kcontrol_new pcxhr_control_analog_level = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	/* name will be filled later */
	.info =		pcxhr_analog_vol_info,
	.get =		pcxhr_analog_vol_get,
	.put =		pcxhr_analog_vol_put,
};

/* shared */
static int pcxhr_sw_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int pcxhr_audio_sw_get(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);

	down(&chip->mgr->mixer_mutex);
	ucontrol->value.integer.value[0] = chip->analog_playback_active[0];
	ucontrol->value.integer.value[1] = chip->analog_playback_active[1];
	up(&chip->mgr->mixer_mutex);
	return 0;
}

static int pcxhr_audio_sw_put(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int i, changed = 0;
	down(&chip->mgr->mixer_mutex);
	for(i = 0; i < 2; i++) {
		if (chip->analog_playback_active[i] != ucontrol->value.integer.value[i]) {
			chip->analog_playback_active[i] = ucontrol->value.integer.value[i];
			changed = 1;
			pcxhr_update_analog_audio_level(chip, 0, i);	/* update playback levels */
		}
	}
	up(&chip->mgr->mixer_mutex);
	return changed;
}

static struct snd_kcontrol_new pcxhr_control_output_switch = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.name =		"Master Playback Switch",
	.info =		pcxhr_sw_info,		/* shared */
	.get =		pcxhr_audio_sw_get,
	.put =		pcxhr_audio_sw_put
};


#define PCXHR_DIGITAL_LEVEL_MIN		0x000	/* -110 dB */
#define PCXHR_DIGITAL_LEVEL_MAX		0x1ff	/* +18 dB */
#define PCXHR_DIGITAL_ZERO_LEVEL	0x1b7	/*  0 dB */


#define MORE_THAN_ONE_STREAM_LEVEL	0x000001
#define VALID_STREAM_PAN_LEVEL_MASK	0x800000
#define VALID_STREAM_LEVEL_MASK		0x400000
#define VALID_STREAM_LEVEL_1_MASK	0x200000
#define VALID_STREAM_LEVEL_2_MASK	0x100000

static int pcxhr_update_playback_stream_level(struct snd_pcxhr* chip, int idx)
{
	int err;
	struct pcxhr_rmh rmh;
	struct pcxhr_pipe *pipe = &chip->playback_pipe;
	int left, right;

	if (chip->digital_playback_active[idx][0])
		left = chip->digital_playback_volume[idx][0];
	else
		left = PCXHR_DIGITAL_LEVEL_MIN;
	if (chip->digital_playback_active[idx][1])
		right = chip->digital_playback_volume[idx][1];
	else
		right = PCXHR_DIGITAL_LEVEL_MIN;

	pcxhr_init_rmh(&rmh, CMD_STREAM_OUT_LEVEL_ADJUST);
	/* add pipe and stream mask */
	pcxhr_set_pipe_cmd_params(&rmh, 0, pipe->first_audio, 0, 1<<idx);
	/* volume left->left / right->right panoramic level */
	rmh.cmd[0] |= MORE_THAN_ONE_STREAM_LEVEL;
	rmh.cmd[2]  = VALID_STREAM_PAN_LEVEL_MASK | VALID_STREAM_LEVEL_1_MASK;
	rmh.cmd[2] |= (left << 10);
	rmh.cmd[3]  = VALID_STREAM_PAN_LEVEL_MASK | VALID_STREAM_LEVEL_2_MASK;
	rmh.cmd[3] |= right;
	rmh.cmd_len = 4;

	err = pcxhr_send_msg(chip->mgr, &rmh);
	if (err < 0) {
		snd_printk(KERN_DEBUG "error update_playback_stream_level "
			   "card(%d) err(%x)\n", chip->chip_idx, err);
		return -EINVAL;
	}
	return 0;
}

#define AUDIO_IO_HAS_MUTE_LEVEL		0x400000
#define AUDIO_IO_HAS_MUTE_MONITOR_1	0x200000
#define VALID_AUDIO_IO_DIGITAL_LEVEL	0x000001
#define VALID_AUDIO_IO_MONITOR_LEVEL	0x000002
#define VALID_AUDIO_IO_MUTE_LEVEL	0x000004
#define VALID_AUDIO_IO_MUTE_MONITOR_1	0x000008

static int pcxhr_update_audio_pipe_level(struct snd_pcxhr* chip, int capture, int channel)
{
	int err;
	struct pcxhr_rmh rmh;
	struct pcxhr_pipe *pipe;

	if (capture)
		pipe = &chip->capture_pipe[0];
	else
		pipe = &chip->playback_pipe;

	pcxhr_init_rmh(&rmh, CMD_AUDIO_LEVEL_ADJUST);
	/* add channel mask */
	pcxhr_set_pipe_cmd_params(&rmh, capture, 0, 0, 1 << (channel + pipe->first_audio));
	/* TODO : if mask (3 << pipe->first_audio) is used, left and right channel
	 * will be programmed to the same params
	 */
	if (capture) {
		rmh.cmd[0] |= VALID_AUDIO_IO_DIGITAL_LEVEL;
		/* VALID_AUDIO_IO_MUTE_LEVEL not yet handled (capture pipe level) */
		rmh.cmd[2] = chip->digital_capture_volume[channel];
	} else {
		rmh.cmd[0] |= VALID_AUDIO_IO_MONITOR_LEVEL | VALID_AUDIO_IO_MUTE_MONITOR_1;
		/* VALID_AUDIO_IO_DIGITAL_LEVEL and VALID_AUDIO_IO_MUTE_LEVEL not yet
		 * handled (playback pipe level)
		 */
		rmh.cmd[2] = chip->monitoring_volume[channel] << 10;
		if (chip->monitoring_active[channel] == 0)
			rmh.cmd[2] |= AUDIO_IO_HAS_MUTE_MONITOR_1;
	}
	rmh.cmd_len = 3;

	err = pcxhr_send_msg(chip->mgr, &rmh);
	if(err<0) {
		snd_printk(KERN_DEBUG "error update_audio_level card(%d) err(%x)\n",
			   chip->chip_idx, err);
		return -EINVAL;
	}
	return 0;
}


/* shared */
static int pcxhr_digital_vol_info(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = PCXHR_DIGITAL_LEVEL_MIN;   /* -109.5 dB */
	uinfo->value.integer.max = PCXHR_DIGITAL_LEVEL_MAX;   /*   18.0 dB */
	return 0;
}


static int pcxhr_pcm_vol_get(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int idx = snd_ctl_get_ioffidx(kcontrol, &ucontrol->id);		/* index */
	int *stored_volume;
	int is_capture = kcontrol->private_value;

	down(&chip->mgr->mixer_mutex);
	if (is_capture)
		stored_volume = chip->digital_capture_volume;		/* digital capture */
	else
		stored_volume = chip->digital_playback_volume[idx];	/* digital playback */
	ucontrol->value.integer.value[0] = stored_volume[0];
	ucontrol->value.integer.value[1] = stored_volume[1];
	up(&chip->mgr->mixer_mutex);
	return 0;
}

static int pcxhr_pcm_vol_put(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int idx = snd_ctl_get_ioffidx(kcontrol, &ucontrol->id);		/* index */
	int changed = 0;
	int is_capture = kcontrol->private_value;
	int *stored_volume;
	int i;

	down(&chip->mgr->mixer_mutex);
	if (is_capture)
		stored_volume = chip->digital_capture_volume;		/* digital capture */
	else
		stored_volume = chip->digital_playback_volume[idx];	/* digital playback */
	for (i = 0; i < 2; i++) {
		if (stored_volume[i] != ucontrol->value.integer.value[i]) {
			stored_volume[i] = ucontrol->value.integer.value[i];
			changed = 1;
			if (is_capture)	/* update capture volume */
				pcxhr_update_audio_pipe_level(chip, 1, i);
		}
	}
	if (! is_capture && changed)
		pcxhr_update_playback_stream_level(chip, idx);	/* update playback volume */
	up(&chip->mgr->mixer_mutex);
	return changed;
}

static struct snd_kcontrol_new snd_pcxhr_pcm_vol =
{
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	/* name will be filled later */
	/* count will be filled later */
	.info =		pcxhr_digital_vol_info,		/* shared */
	.get =		pcxhr_pcm_vol_get,
	.put =		pcxhr_pcm_vol_put,
};


static int pcxhr_pcm_sw_get(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int idx = snd_ctl_get_ioffidx(kcontrol, &ucontrol->id); /* index */

	down(&chip->mgr->mixer_mutex);
	ucontrol->value.integer.value[0] = chip->digital_playback_active[idx][0];
	ucontrol->value.integer.value[1] = chip->digital_playback_active[idx][1];
	up(&chip->mgr->mixer_mutex);
	return 0;
}

static int pcxhr_pcm_sw_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int changed = 0;
	int idx = snd_ctl_get_ioffidx(kcontrol, &ucontrol->id); /* index */
	int i, j;

	down(&chip->mgr->mixer_mutex);
	j = idx;
	for (i = 0; i < 2; i++) {
		if (chip->digital_playback_active[j][i] != ucontrol->value.integer.value[i]) {
			chip->digital_playback_active[j][i] = ucontrol->value.integer.value[i];
			changed = 1;
		}
	}
	if (changed)
		pcxhr_update_playback_stream_level(chip, idx);
	up(&chip->mgr->mixer_mutex);
	return changed;
}

static struct snd_kcontrol_new pcxhr_control_pcm_switch = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.name =		"PCM Playback Switch",
	.count =	PCXHR_PLAYBACK_STREAMS,
	.info =		pcxhr_sw_info,		/* shared */
	.get =		pcxhr_pcm_sw_get,
	.put =		pcxhr_pcm_sw_put
};


/*
 * monitoring level control
 */

static int pcxhr_monitor_vol_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	down(&chip->mgr->mixer_mutex);
	ucontrol->value.integer.value[0] = chip->monitoring_volume[0];
	ucontrol->value.integer.value[1] = chip->monitoring_volume[1];
	up(&chip->mgr->mixer_mutex);
	return 0;
}

static int pcxhr_monitor_vol_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int changed = 0;
	int i;

	down(&chip->mgr->mixer_mutex);
	for (i = 0; i < 2; i++) {
		if (chip->monitoring_volume[i] != ucontrol->value.integer.value[i]) {
			chip->monitoring_volume[i] = ucontrol->value.integer.value[i];
			if(chip->monitoring_active[i])	/* do only when monitoring is unmuted */
				/* update monitoring volume and mute */
				pcxhr_update_audio_pipe_level(chip, 0, i);
			changed = 1;
		}
	}
	up(&chip->mgr->mixer_mutex);
	return changed;
}

static struct snd_kcontrol_new pcxhr_control_monitor_vol = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.name =         "Monitoring Volume",
	.info =		pcxhr_digital_vol_info,		/* shared */
	.get =		pcxhr_monitor_vol_get,
	.put =		pcxhr_monitor_vol_put,
};

/*
 * monitoring switch control
 */

static int pcxhr_monitor_sw_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	down(&chip->mgr->mixer_mutex);
	ucontrol->value.integer.value[0] = chip->monitoring_active[0];
	ucontrol->value.integer.value[1] = chip->monitoring_active[1];
	up(&chip->mgr->mixer_mutex);
	return 0;
}

static int pcxhr_monitor_sw_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int changed = 0;
	int i;

	down(&chip->mgr->mixer_mutex);
	for (i = 0; i < 2; i++) {
		if (chip->monitoring_active[i] != ucontrol->value.integer.value[i]) {
			chip->monitoring_active[i] = ucontrol->value.integer.value[i];
			changed |= (1<<i); /* mask 0x01 and 0x02 */
		}
	}
	if(changed & 0x01)
		/* update left monitoring volume and mute */
		pcxhr_update_audio_pipe_level(chip, 0, 0);
	if(changed & 0x02)
		/* update right monitoring volume and mute */
		pcxhr_update_audio_pipe_level(chip, 0, 1);

	up(&chip->mgr->mixer_mutex);
	return (changed != 0);
}

static struct snd_kcontrol_new pcxhr_control_monitor_sw = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.name =         "Monitoring Switch",
	.info =         pcxhr_sw_info,		/* shared */
	.get =          pcxhr_monitor_sw_get,
	.put =          pcxhr_monitor_sw_put
};



/*
 * audio source select
 */
#define PCXHR_SOURCE_AUDIO01_UER	0x000100
#define PCXHR_SOURCE_AUDIO01_SYNC	0x000200
#define PCXHR_SOURCE_AUDIO23_UER	0x000400
#define PCXHR_SOURCE_AUDIO45_UER	0x001000
#define PCXHR_SOURCE_AUDIO67_UER	0x040000

static int pcxhr_set_audio_source(struct snd_pcxhr* chip)
{
	struct pcxhr_rmh rmh;
	unsigned int mask, reg;
	unsigned int codec;
	int err, use_src, changed;

	switch (chip->chip_idx) {
	case 0 : mask = PCXHR_SOURCE_AUDIO01_UER; codec = CS8420_01_CS; break;
	case 1 : mask = PCXHR_SOURCE_AUDIO23_UER; codec = CS8420_23_CS; break;
	case 2 : mask = PCXHR_SOURCE_AUDIO45_UER; codec = CS8420_45_CS; break;
	case 3 : mask = PCXHR_SOURCE_AUDIO67_UER; codec = CS8420_67_CS; break;
	default: return -EINVAL;
	}
	reg = 0;	/* audio source from analog plug */
	use_src = 0;	/* do not activate codec SRC */

	if (chip->audio_capture_source != 0) {
		reg = mask;	/* audio source from digital plug */
		if (chip->audio_capture_source == 2)
			use_src = 1;
	}
	/* set the input source */
	pcxhr_write_io_num_reg_cont(chip->mgr, mask, reg, &changed);
	/* resync them (otherwise channel inversion possible) */
	if (changed) {
		pcxhr_init_rmh(&rmh, CMD_RESYNC_AUDIO_INPUTS);
		rmh.cmd[0] |= (1 << chip->chip_idx);
		err = pcxhr_send_msg(chip->mgr, &rmh);
		if (err)
			return err;
	}
	pcxhr_init_rmh(&rmh, CMD_ACCESS_IO_WRITE);	/* set codec SRC on off */
	rmh.cmd_len = 3;
	rmh.cmd[0] |= IO_NUM_UER_CHIP_REG;
	rmh.cmd[1] = codec;
	rmh.cmd[2] = (CS8420_DATA_FLOW_CTL & CHIP_SIG_AND_MAP_SPI) | (use_src ? 0x41 : 0x54);
	err = pcxhr_send_msg(chip->mgr, &rmh);
	if(err)
		return err;
	rmh.cmd[2] = (CS8420_CLOCK_SRC_CTL & CHIP_SIG_AND_MAP_SPI) | (use_src ? 0x41 : 0x49);
	err = pcxhr_send_msg(chip->mgr, &rmh);
	return err;
}

static int pcxhr_audio_src_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo)
{
	static char *texts[3] = {"Analog", "Digital", "Digi+SRC"};

	uinfo->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
	uinfo->count = 1;
	uinfo->value.enumerated.items = 3;
	if (uinfo->value.enumerated.item > 2)
		uinfo->value.enumerated.item = 2;
	strcpy(uinfo->value.enumerated.name,
		texts[uinfo->value.enumerated.item]);
	return 0;
}

static int pcxhr_audio_src_get(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	ucontrol->value.enumerated.item[0] = chip->audio_capture_source;
	return 0;
}

static int pcxhr_audio_src_put(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int ret = 0;

	down(&chip->mgr->mixer_mutex);
	if (chip->audio_capture_source != ucontrol->value.enumerated.item[0]) {
		chip->audio_capture_source = ucontrol->value.enumerated.item[0];
		pcxhr_set_audio_source(chip);
		ret = 1;
	}
	up(&chip->mgr->mixer_mutex);
	return ret;
}

static struct snd_kcontrol_new pcxhr_control_audio_src = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.name =		"Capture Source",
	.info =		pcxhr_audio_src_info,
	.get =		pcxhr_audio_src_get,
	.put =		pcxhr_audio_src_put,
};


/*
 * clock type selection
 * enum pcxhr_clock_type {
 *		PCXHR_CLOCK_TYPE_INTERNAL = 0,
 *		PCXHR_CLOCK_TYPE_WORD_CLOCK,
 *		PCXHR_CLOCK_TYPE_AES_SYNC,
 *		PCXHR_CLOCK_TYPE_AES_1,
 *		PCXHR_CLOCK_TYPE_AES_2,
 *		PCXHR_CLOCK_TYPE_AES_3,
 *		PCXHR_CLOCK_TYPE_AES_4,
 *	};
 */

static int pcxhr_clock_type_info(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_info *uinfo)
{
	static char *texts[7] = {
		"Internal", "WordClock", "AES Sync", "AES 1", "AES 2", "AES 3", "AES 4"
	};
	struct pcxhr_mgr *mgr = snd_kcontrol_chip(kcontrol);
	int clock_items = 3 + mgr->capture_chips;

	uinfo->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
	uinfo->count = 1;
	uinfo->value.enumerated.items = clock_items;
	if (uinfo->value.enumerated.item >= clock_items)
		uinfo->value.enumerated.item = clock_items-1;
	strcpy(uinfo->value.enumerated.name,
		texts[uinfo->value.enumerated.item]);
	return 0;
}

static int pcxhr_clock_type_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct pcxhr_mgr *mgr = snd_kcontrol_chip(kcontrol);
	ucontrol->value.enumerated.item[0] = mgr->use_clock_type;
	return 0;
}

static int pcxhr_clock_type_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct pcxhr_mgr *mgr = snd_kcontrol_chip(kcontrol);
	int rate, ret = 0;

	down(&mgr->mixer_mutex);
	if (mgr->use_clock_type != ucontrol->value.enumerated.item[0]) {
		down(&mgr->setup_mutex);
		mgr->use_clock_type = ucontrol->value.enumerated.item[0];
		if (mgr->use_clock_type)
			pcxhr_get_external_clock(mgr, mgr->use_clock_type, &rate);
		else
			rate = mgr->sample_rate;
		if (rate) {
			pcxhr_set_clock(mgr, rate);
			if (mgr->sample_rate)
				mgr->sample_rate = rate;
		}
		up(&mgr->setup_mutex);
		ret = 1;	/* return 1 even if the set was not done. ok ? */
	}
	up(&mgr->mixer_mutex);
	return ret;
}

static struct snd_kcontrol_new pcxhr_control_clock_type = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.name =		"Clock Mode",
	.info =		pcxhr_clock_type_info,
	.get =		pcxhr_clock_type_get,
	.put =		pcxhr_clock_type_put,
};

/*
 * clock rate control
 * specific control that scans the sample rates on the external plugs
 */
static int pcxhr_clock_rate_info(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_info *uinfo)
{
	struct pcxhr_mgr *mgr = snd_kcontrol_chip(kcontrol);
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 3 + mgr->capture_chips;
	uinfo->value.integer.min = 0;		/* clock not present */
	uinfo->value.integer.max = 192000;	/* max sample rate 192 kHz */
	return 0;
}

static int pcxhr_clock_rate_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct pcxhr_mgr *mgr = snd_kcontrol_chip(kcontrol);
	int i, err, rate;

	down(&mgr->mixer_mutex);
	for(i = 0; i < 3 + mgr->capture_chips; i++) {
		if (i == PCXHR_CLOCK_TYPE_INTERNAL)
			rate = mgr->sample_rate_real;
		else {
			err = pcxhr_get_external_clock(mgr, i, &rate);
			if (err)
				break;
		}
		ucontrol->value.integer.value[i] = rate;
	}
	up(&mgr->mixer_mutex);
	return 0;
}

static struct snd_kcontrol_new pcxhr_control_clock_rate = {
	.access =	SNDRV_CTL_ELEM_ACCESS_READ,
	.iface =	SNDRV_CTL_ELEM_IFACE_CARD,
	.name =		"Clock Rates",
	.info =		pcxhr_clock_rate_info,
	.get =		pcxhr_clock_rate_get,
};

/*
 * IEC958 status bits
 */
static int pcxhr_iec958_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_IEC958;
	uinfo->count = 1;
	return 0;
}

static int pcxhr_iec958_capture_byte(struct snd_pcxhr *chip, int aes_idx, unsigned char* aes_bits)
{
	int i, err;
	unsigned char temp;
	struct pcxhr_rmh rmh;

	pcxhr_init_rmh(&rmh, CMD_ACCESS_IO_READ);
	rmh.cmd[0] |= IO_NUM_UER_CHIP_REG;
	switch (chip->chip_idx) {
	case 0:	rmh.cmd[1] = CS8420_01_CS; break;	/* use CS8416_01_CS for AES SYNC plug */
	case 1:	rmh.cmd[1] = CS8420_23_CS; break;
	case 2:	rmh.cmd[1] = CS8420_45_CS; break;
	case 3:	rmh.cmd[1] = CS8420_67_CS; break;
	default: return -EINVAL;
	}
	switch (aes_idx) {
	case 0:	rmh.cmd[2] = CS8420_CSB0; break;	/* use CS8416_CSBx for AES SYNC plug */
	case 1:	rmh.cmd[2] = CS8420_CSB1; break;
	case 2:	rmh.cmd[2] = CS8420_CSB2; break;
	case 3:	rmh.cmd[2] = CS8420_CSB3; break;
	case 4:	rmh.cmd[2] = CS8420_CSB4; break;
	default: return -EINVAL;
	}
	rmh.cmd[1] &= 0x0fffff;			/* size and code the chip id for the fpga */
	rmh.cmd[2] &= CHIP_SIG_AND_MAP_SPI;	/* chip signature + map for spi read */
	rmh.cmd_len = 3;
	err = pcxhr_send_msg(chip->mgr, &rmh);
	if (err)
		return err;
	temp = 0;
	for (i = 0; i < 8; i++) {
		/* attention : reversed bit order (not with CS8416_01_CS) */
		temp <<= 1;
		if (rmh.stat[1] & (1 << i))
			temp |= 1;
	}
	snd_printdd("read iec958 AES %d byte %d = 0x%x\n", chip->chip_idx, aes_idx, temp);
	*aes_bits = temp;
	return 0;
}

static int pcxhr_iec958_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	unsigned char aes_bits;
	int i, err;

	down(&chip->mgr->mixer_mutex);
	for(i = 0; i < 5; i++) {
		if (kcontrol->private_value == 0)	/* playback */
			aes_bits = chip->aes_bits[i];
		else {				/* capture */
			err = pcxhr_iec958_capture_byte(chip, i, &aes_bits);
			if (err)
				break;
		}
		ucontrol->value.iec958.status[i] = aes_bits;
	}
	up(&chip->mgr->mixer_mutex);
        return 0;
}

static int pcxhr_iec958_mask_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	int i;
	for (i = 0; i < 5; i++)
		ucontrol->value.iec958.status[i] = 0xff;
        return 0;
}

static int pcxhr_iec958_update_byte(struct snd_pcxhr *chip, int aes_idx, unsigned char aes_bits)
{
	int i, err, cmd;
	unsigned char new_bits = aes_bits;
	unsigned char old_bits = chip->aes_bits[aes_idx];
	struct pcxhr_rmh rmh;

	for (i = 0; i < 8; i++) {
		if ((old_bits & 0x01) != (new_bits & 0x01)) {
			cmd = chip->chip_idx & 0x03;		/* chip index 0..3 */
			if(chip->chip_idx > 3)
				/* new bit used if chip_idx>3 (PCX1222HR) */
				cmd |= 1 << 22;
			cmd |= ((aes_idx << 3) + i) << 2;	/* add bit offset */
			cmd |= (new_bits & 0x01) << 23;		/* add bit value */
			pcxhr_init_rmh(&rmh, CMD_ACCESS_IO_WRITE);
			rmh.cmd[0] |= IO_NUM_REG_CUER;
			rmh.cmd[1] = cmd;
			rmh.cmd_len = 2;
			snd_printdd("write iec958 AES %d byte %d bit %d (cmd %x)\n",
				    chip->chip_idx, aes_idx, i, cmd);
			err = pcxhr_send_msg(chip->mgr, &rmh);
			if (err)
				return err;
		}
		old_bits >>= 1;
		new_bits >>= 1;
	}
	chip->aes_bits[aes_idx] = aes_bits;
	return 0;
}

static int pcxhr_iec958_put(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_pcxhr *chip = snd_kcontrol_chip(kcontrol);
	int i, changed = 0;

	/* playback */
	down(&chip->mgr->mixer_mutex);
	for (i = 0; i < 5; i++) {
		if (ucontrol->value.iec958.status[i] != chip->aes_bits[i]) {
			pcxhr_iec958_update_byte(chip, i, ucontrol->value.iec958.status[i]);
			changed = 1;
		}
	}
	up(&chip->mgr->mixer_mutex);
	return changed;
}

static struct snd_kcontrol_new pcxhr_control_playback_iec958_mask = {
	.access =	SNDRV_CTL_ELEM_ACCESS_READ,
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		SNDRV_CTL_NAME_IEC958("",PLAYBACK,MASK),
	.info =		pcxhr_iec958_info,
	.get =		pcxhr_iec958_mask_get
};
static struct snd_kcontrol_new pcxhr_control_playback_iec958 = {
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =         SNDRV_CTL_NAME_IEC958("",PLAYBACK,DEFAULT),
	.info =         pcxhr_iec958_info,
	.get =          pcxhr_iec958_get,
	.put =          pcxhr_iec958_put,
	.private_value = 0 /* playback */
};

static struct snd_kcontrol_new pcxhr_control_capture_iec958_mask = {
	.access =	SNDRV_CTL_ELEM_ACCESS_READ,
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =		SNDRV_CTL_NAME_IEC958("",CAPTURE,MASK),
	.info =		pcxhr_iec958_info,
	.get =		pcxhr_iec958_mask_get
};
static struct snd_kcontrol_new pcxhr_control_capture_iec958 = {
	.access =	SNDRV_CTL_ELEM_ACCESS_READ,
	.iface =	SNDRV_CTL_ELEM_IFACE_PCM,
	.name =         SNDRV_CTL_NAME_IEC958("",CAPTURE,DEFAULT),
	.info =         pcxhr_iec958_info,
	.get =          pcxhr_iec958_get,
	.private_value = 1 /* capture */
};

static void pcxhr_init_audio_levels(struct snd_pcxhr *chip)
{
	int i;

	for (i = 0; i < 2; i++) {
		if (chip->nb_streams_play) {
			int j;
			/* at boot time the digital volumes are unmuted 0dB */
			for (j = 0; j < PCXHR_PLAYBACK_STREAMS; j++) {
				chip->digital_playback_active[j][i] = 1;
				chip->digital_playback_volume[j][i] = PCXHR_DIGITAL_ZERO_LEVEL;
			}
			/* after boot, only two bits are set on the uer interface */
			chip->aes_bits[0] = IEC958_AES0_PROFESSIONAL | IEC958_AES0_PRO_FS_48000;
/* only for test purpose, remove later */
#ifdef CONFIG_SND_DEBUG
			/* analog volumes for playback (is LEVEL_MIN after boot) */
			chip->analog_playback_active[i] = 1;
			chip->analog_playback_volume[i] = PCXHR_ANALOG_PLAYBACK_ZERO_LEVEL;
			pcxhr_update_analog_audio_level(chip, 0, i);
#endif
/* test end */
		}
		if (chip->nb_streams_capt) {
			/* at boot time the digital volumes are unmuted 0dB */
			chip->digital_capture_volume[i] = PCXHR_DIGITAL_ZERO_LEVEL;
/* only for test purpose, remove later */
#ifdef CONFIG_SND_DEBUG
			/* analog volumes for playback (is LEVEL_MIN after boot) */
			chip->analog_capture_volume[i]  = PCXHR_ANALOG_CAPTURE_ZERO_LEVEL;
			pcxhr_update_analog_audio_level(chip, 1, i);
#endif
/* test end */
		}
	}

	return;
}


int pcxhr_create_mixer(struct pcxhr_mgr *mgr)
{
	struct snd_pcxhr *chip;
	int err, i;

	init_MUTEX(&mgr->mixer_mutex); /* can be in another place */

	for (i = 0; i < mgr->num_cards; i++) {
		struct snd_kcontrol_new temp;
		chip = mgr->chip[i];

		if (chip->nb_streams_play) {
			/* analog output level control */
			temp = pcxhr_control_analog_level;
			temp.name = "Master Playback Volume";
			temp.private_value = 0; /* playback */
			if ((err = snd_ctl_add(chip->card, snd_ctl_new1(&temp, chip))) < 0)
				return err;
			/* output mute controls */
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_output_switch,
							    chip))) < 0)
				return err;
			
			temp = snd_pcxhr_pcm_vol;
			temp.name = "PCM Playback Volume";
			temp.count = PCXHR_PLAYBACK_STREAMS;
			temp.private_value = 0; /* playback */
			if ((err = snd_ctl_add(chip->card, snd_ctl_new1(&temp, chip))) < 0)
				return err;

			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_pcm_switch,
							    chip))) < 0)
				return err;

			/* IEC958 controls */
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_playback_iec958_mask,
							    chip))) < 0)
				return err;
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_playback_iec958,
							    chip))) < 0)
				return err;
		}
		if (chip->nb_streams_capt) {
			/* analog input level control only on first two chips !*/
			temp = pcxhr_control_analog_level;
			temp.name = "Master Capture Volume";
			temp.private_value = 1; /* capture */
			if ((err = snd_ctl_add(chip->card, snd_ctl_new1(&temp, chip))) < 0)
				return err;

			temp = snd_pcxhr_pcm_vol;
			temp.name = "PCM Capture Volume";
			temp.count = 1;
			temp.private_value = 1; /* capture */
			if ((err = snd_ctl_add(chip->card, snd_ctl_new1(&temp, chip))) < 0)
				return err;
			/* Audio source */
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_audio_src,
							    chip))) < 0)
				return err;
			/* IEC958 controls */
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_capture_iec958_mask,
							    chip))) < 0)
				return err;
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_capture_iec958,
							    chip))) < 0)
				return err;
		}
		/* monitoring only if playback and capture device available */
		if (chip->nb_streams_capt > 0 && chip->nb_streams_play > 0) {
			/* monitoring */
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_monitor_vol,
							    chip))) < 0)
				return err;
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_monitor_sw,
							    chip))) < 0)
				return err;
		}

		if (i == 0) {
			/* clock mode only one control per pcxhr */
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_clock_type,
							    mgr))) < 0)
				return err;
			/* non standard control used to scan the external clock presence/frequencies */
			if ((err = snd_ctl_add(chip->card,
					       snd_ctl_new1(&pcxhr_control_clock_rate,
							    mgr))) < 0)
				return err;
		}

		/* init values for the mixer data */
		pcxhr_init_audio_levels(chip);
	}

	return 0;
}