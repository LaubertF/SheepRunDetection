import av


def with_pyav(video: str, index: int = 0):
    """
    Link: https://pypi.org/project/av/
    My comments:
        Works really well, but it is slower than ffprobe.
        The big advantage is that ffmpeg does not have to be installed on the computer, because pyav installs it automatically

    Parameters:
        video (str): Video path
        index (int): Stream index of the video.
    Returns:
        List of timestamps in ms
    """
    container = av.open(video)
    video = container.streams.get(index)[0]

    if video.type != "video":
        raise ValueError(
            f'The index {index} is not a video stream. It is an {video.type} stream.'
        )

    av_timestamps = [
        int(packet.pts * video.time_base * 1000) for packet in container.demux(video) if packet.pts is not None
    ]
    container.close()
    av_timestamps.sort()
    #get differences between timestamps
    return av_timestamps


alist = with_pyav("Resources/vid.mp4")
print(alist)
# %%
