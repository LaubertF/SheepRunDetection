%clear all
%load('data_test2.mat');
n=8;
fs=15; % 76Hz
f=fs/2;
wn=3/f;
b=fir1(n,wn,'low');
freqz(b,1,512,fs);
y=filter(b,1,accx);
figure(2)
ax1=subplot(2,1,1);
plot(dt,accx)
ax2=subplot(2,1,2);
plot(dt,y);
linkaxes([ax1,ax2],'x');   %pri zoomovani su grafy previazane v x-osy