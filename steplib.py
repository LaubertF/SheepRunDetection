import numpy as np


def apply_filter(data_buffer, coefficients):
    # Ensure the coefficients are in a numpy array for convenience

    coefficients = np.array(coefficients)

    # Create an array to hold the filtered data
    filtered_data = np.zeros_like(data_buffer)


    # Apply the filter
    for i in range(len(coefficients), len(data_buffer)):
        filtered_data[i] = np.dot(data_buffer[i - len(coefficients):i], coefficients[::-1])
    filtered_data[0] = 1
    return filtered_data


def mean_difference(arr, N):
    mean_diffs = [0] * N
    for i in range(N, len(arr) - N):
        left_mean_diff = np.mean([arr[i] - arr[i - k] for k in range(1, N + 1)])
        right_mean_diff = np.mean([arr[i] - arr[i + k] for k in range(1, N + 1)])
        mean_diffs.append((left_mean_diff + right_mean_diff) / 2)
    mean_diffs.extend([0] * N)
    return mean_diffs


def maximum_difference(arr, N):
    max_diffs = [0] * N
    for i in range(N, len(arr) - N):
        left_max_diff = np.max([arr[i] - arr[i - k] for k in range(1, N + 1)])
        right_max_diff = np.max([arr[i] - arr[i + k] for k in range(1, N + 1)])
        max_diffs.append((left_max_diff + right_max_diff) / 2)
    max_diffs.extend([0] * N)
    return max_diffs


def modified_pan_tompkins_scoring(arr, N):
    # Zero-mean the entire data
    N *= 2
    zero_meaned_data = np.zeros_like(arr)
    for i in range(N, len(arr)):
        window = arr[i - N:i]
        diff = arr[i] - np.mean(window)
        # Set negative data points to zero
        if diff > 0:
            zero_meaned_data[i] = diff * diff
    return zero_meaned_data


def detect_outliers(data, c):
    outliers = np.zeros_like(data)

    # Compute the running mean and standard deviation
    mean = np.cumsum(data) / (np.arange(len(data)) + 1)
    std_dev = np.sqrt(np.cumsum((data - mean) ** 2) / (np.arange(len(data)) + 1))

    # Identify outliers
    for i in range(len(data)):
        if data[i] - mean[i] > c * std_dev[i]:
            outliers[i] = 1

    return outliers


def post_processing(signal, window_length_ms, sample_rate):
    # Convert window length from ms to samples
    window_length_samples = int(window_length_ms * sample_rate / 1000)

    # Initialize output
    output = np.zeros_like(signal)

    # Slide window across signal
    i = 0
    while i < len(signal):

        if signal[i] == 1:
            output[i] = 1
            i += window_length_samples
        else:
            output[i] = 0
            i += 1
    return output

# %%
