import numpy as np


def apply_filter(data_buffer, coefficients):
    # Ensure the coefficients are in a numpy array for convenience

    coefficients = np.array(coefficients)

    # Create an array to hold the filtered data
    filtered_data = np.zeros_like(data_buffer)

    # Apply the filter
    for i in range(len(coefficients), len(data_buffer)):
        filtered_data[i] = np.dot(data_buffer[i - len(coefficients):i], coefficients[::-1])
    return filtered_data


def max_difference(data, n):
    results = np.zeros_like(data)

    # Compute the maximum difference
    for i in range(n, len(data) - n):
        left_max_diff = np.max(np.abs(data[i] - data[i - n:i]))
        right_max_diff = np.max(np.abs(data[i] - data[i + 1:i + n + 1]))
        results[i] = (left_max_diff + right_max_diff) / 2

    return results


def detect_outliers(data, c):
    outliers = np.zeros_like(data)

    # Compute the running mean and standard deviation
    mean = np.cumsum(data) / (np.arange(len(data)) + 1)
    std_dev = np.sqrt(np.cumsum((data - mean) ** 2) / (np.arange(len(data)) + 1))

    # Identify outliers
    for i in range(len(data)):
        if np.abs(data[i] - mean[i]) > c * std_dev[i]:
            outliers[i] = 1

    return outliers


def post_processing(signal, window_length_ms, sample_rate):
    # Convert window length from ms to samples
    window_length_samples = int(window_length_ms * sample_rate / 1000)

    # Initialize output
    output = np.zeros_like(signal)

    # Slide window across signal
    for i in range(0, len(signal), window_length_samples):
        # Select window
        window = signal[i:i + window_length_samples]

        # Find local maximum
        local_max_index = np.argmax(window)

        # Only keep the maximum sample within the window
        output[i + local_max_index] = window[local_max_index]

    return output
