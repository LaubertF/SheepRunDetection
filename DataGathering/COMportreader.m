% COM port reader pre Xbee v2.0
clear all
close all

device = serialport("COM4",57600);
%% odoslanie presneho casu na COM port
sendTimestamp(device)
%% citanie dat z COM portu
[getter, S] = timestamps();
getter(true);
drawnow
data = gatherData(device);
device = [];
abs_time_of_clicks = getter();
dt = datetime(abs_time_of_clicks, 'ConvertFrom', 'datenum');
steps = (datestr(dt, 'yyyy-mm-dd HH:MM:SS.FFF'));
headerRow = {'datetime'};
%steps = [headerRow; steps2];
combinedData = parse(data,false);

% Define the file name
filename = 'output.csv';
stepsfile = 'steps.csv';

% Write data to CSV file
writecell(combinedData, filename);
writematrix(steps, stepsfile);


