%%EVALTiming
%
% 2019-01-18

clear all; close all; clc; 

%% load data
load('timeStamp_s.mat');
load('timeToc.mat');

stamp_s = timeStamp_s;
toc_s = timeToc(1:length(stamp_s));

%% plot timing
figure; plot(stamp_s-toc_s)


figure;
hold on;
plot(stamp_s,toc_s)
plot([stamp_s(1) stamp_s(end)],[stamp_s(1) stamp_s(end)]);
hold off;
grid on;
xlabel('time stamp [s]');
ylabel('toc [s]');
