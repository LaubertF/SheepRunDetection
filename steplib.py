import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
# enum


def apply_filter(data_buffer, coefficients):
    return np.convolve(data_buffer, coefficients, mode='same')


def mean_difference(arr, N):
    rolling = pd.Series(arr).rolling(window=N).mean()
    return arr - rolling


def maximum_difference(arr, N):
    rolling = pd.Series(arr).rolling(window=N).min()
    return arr - rolling


def modified_pan_tompkins_scoring(arr, N):
    N *= 2
    zero_meaned_data = np.zeros_like(arr)
    for i in range(N, len(arr)):
        window = arr[i - N:i]
        diff = arr[i] - np.mean(window)
        # Set negative data points to zero
        if diff > 0:
            zero_meaned_data[i] = (diff * diff) + 1
    return zero_meaned_data


def detect_outliers(data, c):
    outliers = np.zeros_like(data)

    # Compute the running mean and standard deviation
    mean = np.cumsum(data) / (np.arange(len(data)) + 1)
    std_dev = np.sqrt(np.cumsum((data - mean) ** 2) / (np.arange(len(data)) + 1))
    outliers[(data - mean) > c * std_dev] = 1
    return outliers


def post_processing(signal, window_length_ms, sample_rate):
    # Convert window length from ms to samples
    window_length_samples = int(window_length_ms * sample_rate / 1000)

    output = np.zeros_like(signal)

    # No detections after the last detection
    i = 0
    while i < len(signal):

        if signal[i] == 1:
            output[i] = 1
            i += window_length_samples
        else:
            output[i] = 0
            i += 1
    return output


def getSteps(file, window_size, threshold, data):
    file_data = pd.read_csv(file)
    expected = np.where(file_data["annotation"] == 1)[0]
    data1 = file_data["x"] ** 2 + file_data["y"] ** 2 + file_data["z"] ** 2
    accelerometer = data1.apply(lambda x: x ** 0.5)
    data1 = apply_filter(accelerometer, data.fir)
    data1 = maximum_difference(data1, window_size)
    data1 = detect_outliers(data1, threshold)
    data1 = post_processing(data1, data.post_window, data.sample_rate)
    calculated = np.where(data1 == 1)[0]
    return calculated, expected , accelerometer

def chart(data_in):
    (calculated, expected, accelerometer) = data_in
    x=range(len(accelerometer))
    y=accelerometer
    plt.figure(figsize=(10,5))
    plt.plot(x,y, color='red', marker='o')
    plt.xlabel('index')
    plt.ylabel('magnitude')
    #show x label every 800
    #add rectangles
    for e in expected:
        plt.axvline(x =e, color ='g', label ='axvline - full height', ymin=0.5, alpha=0.8)
    for c in calculated:
        plt.axvline(x =c, color ='b', label ='axvline - full height', ymax=0.5, alpha=0.8)


    plt.show()
# %%
