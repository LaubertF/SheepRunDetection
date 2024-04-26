import cv2
import datetime
import frameTimestamp
import csv


def string_to_milliseconds(string):
    date_time_obj = datetime.datetime.strptime(string, '%d-%m-%Y %H:%M:%S.%f')
    return date_time_obj.timestamp() * 1000


def getFrame(frame_nr):
    global video
    video.set(cv2.CAP_PROP_POS_FRAMES, frame_nr)


walking = []
running = []
videoName = "Resources/vid.mp4"

video = cv2.VideoCapture(videoName)
if video.isOpened():
    print('Video Successfully opened')
else:
    print('Something went wrong check if the video name and path is correct')
nr_of_frames = int(video.get(cv2.CAP_PROP_FRAME_COUNT))
# define a scale lvl for visualization
scaleLevel = 2  # it means reduce the size to 2**(scaleLevel-1)

windowName = 'Video'
cv2.namedWindow(windowName)
# let's reproduce the video
cv2.createTrackbar("Frame", "Video", 0, nr_of_frames, getFrame)
paused = False
timestamps = frameTimestamp.with_pyav(videoName)
start = 0
playing = True
while playing:
    ret, frame = video.read()  # read a single frame
    if not ret:  # this mean it could not read the frame
        print("Could not read the frame")
        cv2.destroyWindow(windowName)
        break
    cv2.setTrackbarPos("Frame", "Video", int(video.get(cv2.CAP_PROP_POS_FRAMES)))
    rescaled_frame = frame
    for i in range(scaleLevel - 1):
        rescaled_frame = cv2.pyrDown(rescaled_frame)

    cv2.imshow(windowName, rescaled_frame)

    waitKey = (cv2.waitKey(1) & 0xFF)
    if waitKey == ord('q'):  # if Q pressed you could do something else with other keypress
        print("closing video and exiting")
        cv2.destroyWindow(windowName)
        video.release()
        break
    if waitKey == ord('p'):
        print("pausing video")
        paused = True
    while True and paused:
        waitKey = cv2.waitKey(0) & 0xFF
        if waitKey == ord('d'):
            break
        if waitKey == ord('a'):
            video.set(cv2.CAP_PROP_POS_FRAMES, video.get(cv2.CAP_PROP_POS_FRAMES) - 2)
            break
        if waitKey == ord('p'):
            paused = False
            continue
        if waitKey == ord('w'):
            walking.append(int(video.get(cv2.CAP_PROP_POS_FRAMES)))
            if len(walking) % 2 == 1:
                print("Mark end of walking")
            else:
                print("Walking phase marked")
        if waitKey == ord('r'):
            running.append(int(video.get(cv2.CAP_PROP_POS_FRAMES)))
            if len(running) % 2 == 1:
                print("Mark end of running")
            else:
                print("Running phase marked")
        if waitKey == ord('q'):
            playing = False
            break
        if waitKey == ord('m'):
            print("Enter the mark (01-01-2023 00:00:00.000):")
            inp = input()
            try:
                mark = string_to_milliseconds(inp)
                inp_milliseconds = timestamps[int(video.get(cv2.CAP_PROP_POS_FRAMES))]
                start = mark - inp_milliseconds
                print("Mark selected")
            except:
                print("Invalid input")
            break

walk_datetime = []
run_datetime = []
for walk in walking:
    ms = timestamps[walk] + start
    walk_datetime.append(datetime.datetime.fromtimestamp(ms / 1000.0).strftime("%H:%M:%S.%f")[:-3])
for run in running:
    ms = timestamps[run] + start
    run_datetime.append(datetime.datetime.fromtimestamp(ms / 1000.0).strftime("%H:%M:%S.%f")[:-3])

print(walk_datetime)
print(run_datetime)


def split_data(data, state):
    csv_data = []
    for j in range(0, len(data), 2):
        odd_value = data[j]
        even_value = data[j + 1] if j + 1 < len(data) else ''  # Handle the case where there's no even value
        csv_data.append([odd_value, even_value, state])
    return csv_data


csv_filename = 'output.csv'

with open(csv_filename, 'w', newline='') as csvfile:
    csv_writer = csv.writer(csvfile)
    csv_writer.writerows(split_data(walk_datetime, 'walking'))
    csv_writer.writerows(split_data(run_datetime, 'running'))

# %%
