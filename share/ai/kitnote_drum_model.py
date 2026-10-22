# SPDX-License-Identifier: GPL-3.0-only
# KitNote: KitNote's own drum model - which drums are struck when, in a recording of drums (the drum stem the
# separation gives). A small network: a few convolutions over a log-frequency spectrogram, then a recurrent
# layer over time; one output a frame (100 a second) for each of bass drum, snare, toms, hi-hat and cymbals.
#
# The weights (kitnote_drums.pt) are made by tools/kitnote/train_drum_model.py from openly licensed data:
# the Groove MIDI Dataset (Gillick et al., Google Magenta, CC BY 4.0). Nothing else is needed to use it than
# PyTorch and NumPy, which KitNote's audio engine has.

import numpy as np
import torch
import torch.nn as nn

RATE = 44100                   # samples a second the model hears
FPS = 100                      # frames a second it answers
N_FFT = 2048
HOP = RATE // FPS              # 441
CLASSES = ("BD", "SD", "TT", "HH", "CY")


def filterbank(bands_per_octave=12, fmin=30.0, fmax=17000.0):
    """Triangular filters, so many to the octave, from fmin to fmax: [bands, N_FFT // 2 + 1]. Where filters
    would be narrower than a bin of the FFT (the low end), neighbours share it and are made one."""
    freqs = np.fft.rfftfreq(N_FFT, 1.0 / RATE)
    count = int(np.floor(np.log2(fmax / fmin) * bands_per_octave)) + 1
    centres = fmin * 2.0 ** (np.arange(count) / bands_per_octave)
    bins = np.unique(np.round(centres / (RATE / N_FFT)).astype(int))
    bins = bins[(bins > 0) & (bins < len(freqs) - 1)]
    bank = np.zeros((len(bins) - 2, len(freqs)), np.float32)
    for i in range(1, len(bins) - 1):
        left, centre, right = bins[i - 1], bins[i], bins[i + 1]
        bank[i - 1, left:centre + 1] = np.linspace(0, 1, centre - left + 1)
        bank[i - 1, centre:right + 1] = np.linspace(1, 0, right - centre + 1)
        bank[i - 1] /= bank[i - 1].sum()
    return bank


BANK = filterbank()
N_BANDS = BANK.shape[0]


def spectrogram(signal):
    """The log-frequency spectrogram of a mono signal (float32 at RATE): [frames, N_BANDS], log(1 + x). (A few
    thousand frames at a time: the frames of an hour's recording at once would take gigabytes.)"""
    padded = np.pad(signal.astype(np.float32), N_FFT // 2)
    frames = np.lib.stride_tricks.sliding_window_view(padded, N_FFT)[::HOP]
    window = np.hanning(N_FFT).astype(np.float32)
    out = np.empty((len(frames), N_BANDS), np.float32)
    for start in range(0, len(frames), 4096):
        magnitude = np.abs(np.fft.rfft(frames[start:start + 4096] * window, axis=1))
        out[start:start + len(magnitude)] = np.log1p(magnitude @ BANK.T)
    return out


def features(audio):
    """[channels, samples] at RATE -> the model's input [frames, N_BANDS]."""
    return spectrogram(np.asarray(audio, np.float32).mean(axis=0))


class DrumNet(nn.Module):
    """Convolutions over (time, frequency), then a bidirectional GRU over time; logits [batch, frames, classes]."""

    def __init__(self, bands=N_BANDS, classes=len(CLASSES), width=32, hidden=96):
        super().__init__()

        def block(cin, cout):
            return nn.Sequential(
                nn.Conv2d(cin, cout, 3, padding=1), nn.BatchNorm2d(cout), nn.ReLU(),
                nn.Conv2d(cout, cout, 3, padding=1), nn.BatchNorm2d(cout), nn.ReLU(),
                nn.MaxPool2d((1, 3)), nn.Dropout(0.1))

        self.convs = nn.Sequential(block(1, width), block(width, width * 2))
        reduced = bands // 3 // 3
        self.gru = nn.GRU(width * 2 * reduced, hidden, num_layers=2, batch_first=True, bidirectional=True, dropout=0.2)
        self.out = nn.Linear(hidden * 2, classes)

    def forward(self, x):                       # x: [batch, frames, bands]
        h = self.convs(x.unsqueeze(1))          # [batch, channels, frames, bands']
        b, c, t, f = h.shape
        h = h.permute(0, 2, 1, 3).reshape(b, t, c * f)
        h, _ = self.gru(h)
        return self.out(h)


CHUNK = 360 * FPS               # frames the model hears at once (six minutes: a song is heard whole) ...
CONTEXT = 30 * FPS              # ... and, on each side of them, frames it hears only for what comes before and after


def activations(model, feats):
    """The model's activations [frames, classes] (0 to 1) of features [frames, N_BANDS]. A recording longer than
    CHUNK is heard in pieces of CHUNK frames, each with CONTEXT frames around it, and only the middle of each
    piece is kept: the whole of an hour's recording at once would take gigabytes. A recording up to CHUNK
    frames long is heard whole, as before."""
    frames = len(feats)
    out = np.empty((frames, len(CLASSES)), np.float32)
    with torch.no_grad():
        for start in range(0, frames, CHUNK):
            end = min(frames, start + CHUNK)
            lo, hi = max(0, start - CONTEXT), min(frames, end + CONTEXT)
            piece = torch.from_numpy(np.ascontiguousarray(feats[lo:hi])).unsqueeze(0)
            out[start:end] = model(piece).sigmoid().numpy()[0][start - lo:end - lo]
    return out


def load(path, device="cpu"):
    """(model in eval mode, features) from a weights file of train_drum_model.py."""
    saved = torch.load(path, map_location=device, weights_only=True)
    model = DrumNet(**saved.get("config", {}))
    model.load_state_dict(saved["state"])
    model.eval()
    return model, features
