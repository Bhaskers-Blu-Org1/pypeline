# #############################################################################
# lofar_bootes_ps.py
# ==================
# Author : Sepand KASHANI [sep@zurich.ibm.com]
# #############################################################################

"""
Simulated LOFAR imaging with Bluebild (PeriodicSynthesis).
"""

import astropy.coordinates as coord
import astropy.time as atime
import astropy.units as u
import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm as ProgressBar
from scipy.constants import speed_of_light

import pypeline.phased_array.beamforming as beamforming
import pypeline.phased_array.bluebild.data_processor as data_proc
import pypeline.phased_array.bluebild.imager.fourier_domain as bb_fd
import pypeline.phased_array.bluebild.parameter_estimator as param_est
import pypeline.phased_array.instrument as instrument
import pypeline.phased_array.util.data_gen.visibility as visibility
import pypeline.phased_array.util.data_gen.sky as sky
import pypeline.phased_array.util.gram as gr
import pypeline.phased_array.util.grid as grid
import pypeline.phased_array.util.io.image as image

# Observation
obs_start = atime.Time(56879.54171302732, scale='utc', format='mjd')
field_center = coord.SkyCoord(218 * u.deg, 34.5 * u.deg)
field_of_view = np.radians(5)
frequency = 145e6
wl = speed_of_light / frequency

# Instrument
N_station = 24
dev = instrument.LofarBlock(N_station)
mb_cfg = [(_, _, field_center) for _ in range(N_station)]
mb = beamforming.MatchedBeamformerBlock(mb_cfg)
gram = gr.GramBlock()

# Data generation
T_integration = 8
sky_model = sky.from_tgss_catalog(field_center, field_of_view, N_src=20)
vis = visibility.VisibilityGeneratorBlock(sky_model, T_integration, fs=196e3, SNR=np.inf)
time = obs_start + (T_integration * u.s) * np.arange(3595)
obs_end = time[-1]

# Imaging
N_level = 4
N_bits = 32
R = dev.icrs2bfsf_rot(obs_start, obs_end)
pix_q, pix_l, pix_colat, pix_lon = grid.ea_harmonic_grid(
    direction=R @ field_center.cartesian.xyz.value,  # BFSF-equivalent f_dir.
    FoV=field_of_view,
    N=dev.nyquist_rate(wl))
N_FS = dev.bfsf_kernel_bandwidth(wl, obs_start, obs_end)
T_kernel = np.radians(10)

### Intensity Field ===========================================================
# Parameter Estimation
I_est = param_est.IntensityFieldParameterEstimator(N_level, sigma=0.95)
for t in ProgressBar(time[::200]):
    XYZ = dev(t)
    W = mb(XYZ, wl)
    S = vis(XYZ, W, wl)
    G = gram(XYZ, W, wl)

    I_est.collect(S, G)
N_eig, c_centroid = I_est.infer_parameters()

# Imaging
I_dp = data_proc.IntensityFieldDataProcessorBlock(N_eig, c_centroid)
I_mfs = bb_fd.Fourier_IMFS_Block(wl, pix_colat, pix_lon,
                                 N_FS, T_kernel, R, N_level, N_bits)
for t in ProgressBar(time[::1]):
    XYZ = dev(t)
    W = mb(XYZ, wl)
    S = vis(XYZ, W, wl)
    G = gram(XYZ, W, wl)

    D, V, c_idx = I_dp(S, G)
    _ = I_mfs(D, V, XYZ.data, W.data, c_idx)
I_std, I_lsq = I_mfs.as_image()

### Sensitivity Field =========================================================
# Parameter Estimation
S_est = param_est.SensitivityFieldParameterEstimator(sigma=0.95)
for t in ProgressBar(time[::200]):
    XYZ = dev(t)
    W = mb(XYZ, wl)
    G = gram(XYZ, W, wl)

    S_est.collect(G)
N_eig = S_est.infer_parameters()

# Imaging
S_dp = data_proc.SensitivityFieldDataProcessorBlock(N_eig)
S_mfs = bb_fd.Fourier_IMFS_Block(wl, pix_colat, pix_lon,
                                 N_FS, T_kernel, R, 1, N_bits)
for t in ProgressBar(time[::50]):
    XYZ = dev(t)
    W = mb(XYZ, wl)
    G = gram(XYZ, W, wl)

    D, V = S_dp(G)
    _ = S_mfs(D, V, XYZ.data, W.data, cluster_idx=np.zeros(N_eig, dtype=int))
_, S = S_mfs.as_image()

# Plot Results ================================================================
fig, ax = plt.subplots(ncols=2)
I_std_eq = image.SphericalImage(I_std.data / S.data, I_std.grid)
I_std_eq.draw(catalog=sky_model, ax=ax[0])
ax[0].set_title('Bluebild Standardized Image')

I_lsq_eq = image.SphericalImage(I_lsq.data / S.data, I_lsq.grid)
I_lsq_eq.draw(catalog=sky_model, ax=ax[1])
ax[1].set_title('Bluebild Least-Squares Image')
