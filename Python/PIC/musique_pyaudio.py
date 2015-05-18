# Will Yager
# This Python script sends color/brightness data based on
# ambient sound frequencies to the LEDs.

import pyaudio as pa
import numpy as np
import sys
import serial
# Output values max at 1.0
import notes_scaled_nosaturation
from colorsys import hls_to_rgb

audio_stream = pa.PyAudio().open(format=pa.paInt16, \
								channels=2, \
								rate=44100, \
								input=True, \
								frames_per_buffer=1024)

# Convert the audio data to numbers, num_samples at a time.
def read_audio(audio_stream, num_samples):
	while True:
		# Read all the input data. 
		samples = audio_stream.read(num_samples) 
		# Convert input data to numbers
		samples = np.fromstring(samples, dtype=np.int16).astype(np.float)
		samples_l = samples[::2]  
		samples_r = samples[1::2]
		yield (samples_l, samples_r)


def lum_hue(leds, num_leds):
	for samples in leds:
		sum_for_hue = 0
		for i, v in enumerate(samples):
			sum_for_hue += i * v

		lum = sum(samples) / float(len(samples))
		hue = sum_for_hue / float(sum(samples) or 1) / float(len(samples))

		yield (hue, lum)
		
def rolling_scale(vals, falloff):
	v_min = None
	v_max = None
	for val in vals:
		if v_min is None:
			v_min = val
			v_max = val
		else:
			v_min = tuple(map(min, zip(v_min, val)))
			v_max = tuple(map(max, zip(v_max, val)))
		v_min = tuple(i * falloff + j * (1 - falloff) for i, j in zip(v_min, val))
		v_max = tuple(i * falloff + j * (1 - falloff) for i, j in zip(v_max, val))
		if v_max != v_min:
			yield tuple((vc - vmin) / float(vmax - vmin) for vc, vmin, vmax in zip(val, v_min, v_max))
		else:
			yield val

def colorize(hlgen):
	hlgen = rolling_scale(hlgen, 1)

	for hue, lum in hlgen:
		yield hls_to_rgb(hue, lum, 0.5)

ftdi_file = "/dev/ttyUSB0"
ftdi = serial.Serial(ftdi_file, 38400)


if __name__ == '__main__':

	audio = read_audio(audio_stream, num_samples=512)
	leds = notes_scaled_nosaturation.process(audio, num_leds=32, num_samples=512, sample_rate=44100)
	hl = lum_hue(leds, num_leds=32)
	colors = colorize(hl)

	for c in colors:
		c = list(map(lambda x: int(x * 1023), c))

		string = "R{0}G{1}B{2}".format(c[0],c[1],c[2])

		ftdi.write(string.encode())
