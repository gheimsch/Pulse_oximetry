function runSerialPulseOxy
%RUNPULSEOXY runs pluse and oxymetrie measurement.
%
% 2019-01-04 krista.kappeler@gmail.com

clear all; close all; clc; 

addpath ..\..\Matlab
addpath fct

%% -- UI --------------------------------------------------------------- %%
nbOfSamplesInWin = 200;
updatePlotNbSample = 20;

showPulseDetectionPlot = false;
showOxyDetectionPlot = false;

%% -- CONSTANTS -------------------------------------------------------- %%
% figure constants
screensize = get(0,'Screensize');
screensize = floor(screensize);

% convertions
stepSizeADC = 125e-6; % [V]

% freqeuncy
freq_Hz = 2*8.6; % [Hz]

% oxyCalculation look up table
% lookUpTableOxy = [100, 100, 100, 100, 100, 100, 99, 97, 90, 86, 82, 74, 66, 58, 50, 41, 33, 25, 17, 9, 0];


%% -- SETUP SERIAL ----------------------------------------------------- %%
% find existing objects
newobjs = instrfind;
% close all objects
if ~isempty(newobjs)
    fclose(newobjs);
end
% open serial port
s = serial('COM8');
set(s,'BaudRate',115200);
fopen(s);

%% -- prepare plot ----------------------------------------------------- %%
% initialize values
DCInfrared = zeros(1,nbOfSamplesInWin);
ACInfrared = zeros(1,nbOfSamplesInWin);
DCRed = zeros(1,nbOfSamplesInWin);
ACRed = zeros(1,nbOfSamplesInWin);
time = [0:nbOfSamplesInWin-1].*1/freq_Hz;
topTrackACI = zeros(1,nbOfSamplesInWin);
bottomTrackACI = zeros(1,nbOfSamplesInWin);
pulseSignal = zeros(1,nbOfSamplesInWin);
ttOxyIRed = zeros(1,nbOfSamplesInWin);
btOxyIRed = zeros(1,nbOfSamplesInWin);
ttOxyRed = zeros(1,nbOfSamplesInWin);               
btOxyRed = zeros(1,nbOfSamplesInWin);


% prepare plot
fig = figure;
set(fig,'Position',screensize);

if showPulseDetectionPlot || showOxyDetectionPlot
    subplot(211)
end
hlDCInfrared = line(time, DCInfrared);
hlACInfrared = line(time, ACInfrared);
hlDCRed = line(time, DCRed);
hlACRed = line(time, ACRed);

hlDCInfrared.Color = 'b';
hlACInfrared.Color = 'c';
hlDCRed.Color = 'r';
hlACRed.Color = 'm';

legend({'DC Infrared', 'AC Infrared', 'DC Red', 'AC Red'})
grid on;
% xlabel('nb of samples');
xlabel('time [s]');
ylabel('measured voltage [V]')

% tracker plot
if showPulseDetectionPlot
    % prepare plot
    subplot(212)
    hold on;
    hlACInfrared = line(time, ACInfrared);
    hlTopTrack = line(time, topTrackACI);
    hlBottomTrack = line(time, bottomTrackACI);
    hlPulseSignal = line(time,pulseSignal);
    hold off,
    
    hlTopTrack.Color = 'k';
    hlBottomTrack.Color = 'k';
    hlPulseSignal.Color = 'r';
    legend({'DC Infrared', 'top', 'bottom', 'pulse signal'})
    grid on;
%     xlabel('nb of samples');
    xlabel('time [s]');
    ylabel('measured voltage [V]')
    
elseif showOxyDetectionPlot
    
    % prepare plot
    subplot(212)
    hold on;
    hlACInfrared = line(time, ACInfrared);
    hlACRed = line(time, ACRed);
    
    hlOxyTopTrackIRed = line(time, ttOxyIRed);
    hlOxyBottomTrackIRed = line(time, btOxyIRed);
    hlOxyTopTrackRed = line(time, ttOxyRed);
    hlOxyBottomTrackRed = line(time,btOxyRed);
    hold off,    
    
    hlACInfrared.Color = 'c';
    hlACRed.Color = 'm';
        
    hlOxyTopTrackIRed.Color = 'k';
    hlOxyBottomTrackIRed.Color = 'k';
    hlOxyTopTrackRed.Color = 'k';
    hlOxyBottomTrackRed.Color = 'k';
    legend({'AC Infrared', 'AC Red', 'top IR', 'bottom IR', 'top R', 'bottom IR'})
    grid on;
%     xlabel('nb of samples');
    xlabel('time [s]');
    ylabel('measured voltage [V]')   
    
end
     
    

%% -- calculate -------------------------------------------------------- %%
n = 0; % initialize

while 1
    
    %% prepare
    n = n+1;
    
    % raw data
    DCInfrared(1) = [];
    ACInfrared(1) = [];
    DCRed(1) = [];
    ACRed(1) = [];

    %%  get raw data
    fprintf(s,'*IDN?');
    out = fscanf(s);
    val = textscan(out, '%d,%d,%d,%d');

    %% update values
    % raw data
    DCInfrared(end+1) = double(val{1}).*stepSizeADC;
    ACInfrared(end+1) = double(val{2}).*stepSizeADC;
    DCRed(end+1) = double(val{3}).*stepSizeADC;
    ACRed(end+1) = double(val{4}).*stepSizeADC;   

    
    %% calculate oxygen saturation
%     oxySat = calcOxySat(DCInfrared(end),ACInfrared(end),DCRed(end),ACRed(end));
    [oxySat, btSigIRed, ttSigIRed, btSigRed, ttSigRed ] = calcOxySat(DCInfrared(end),ACInfrared(end),DCRed(end),ACRed(end));
%     disp(['oxygen saturation: ' num2str(oxySat)]);
    
    % calc to percent
    oxySat = round(100*oxySat);
%     disp(['Oxy Sat [%]: ' num2str(oxySat)]);
    if isnan(oxySat)
        oxySat = 0;
    else
        oxySat = min(oxySat,100);
    end

    % update tracker values
    ttOxyIRed(1) = [];
    btOxyIRed(1) = [];
    ttOxyRed(1) = [];
    btOxyRed(1) = [];    
    ttOxyIRed(end+1) = ttSigIRed;
    btOxyIRed(end+1) = btSigIRed; 
    ttOxyRed(end+1) = ttSigRed;
    btOxyRed(end+1) = btSigRed;        
    
    %% calculate pulse
    timeStamp_s = n./freq_Hz;
    [estimatedPulse, pulseSig, bottomTrackSig, topTrackSig]  = calcPulse(ACInfrared(end),timeStamp_s);
    
    % update tracker values
    topTrackACI(1) = [];
    bottomTrackACI(1) = [];
    pulseSignal(1) = [];
    topTrackACI(end+1) = topTrackSig;
    bottomTrackACI(end+1) = bottomTrackSig; 
    pulseSignal(end+1) = pulseSig;
    
    
    %% update graphes
    if mod(n,updatePlotNbSample) == 0
        shg;
        set(hlDCInfrared,'ydata',DCInfrared);
        set(hlACInfrared,'ydata',ACInfrared);
        set(hlDCRed,'ydata',DCRed);
        set(hlACRed,'ydata',ACRed);
                
        if showPulseDetectionPlot
            % set tracker
            set(hlTopTrack,'ydata',topTrackACI);
            set(hlBottomTrack,'ydata',bottomTrackACI);   
            set(hlPulseSignal, 'ydata', pulseSignal);
        elseif showOxyDetectionPlot
            set(hlOxyTopTrackIRed,'ydata',ttOxyIRed);
            set(hlOxyBottomTrackIRed,'ydata',btOxyIRed);               
            set(hlOxyTopTrackRed,'ydata',ttOxyRed);
            set(hlOxyBottomTrackRed,'ydata',btOxyRed); 
        end
        % update title
        if showPulseDetectionPlot
            subplot(211);
        end
        title(['Pulse: ' num2str(estimatedPulse) ' - OxySat: ' num2str(oxySat)]);
    end
    
    %% display raw data
%     disp(['IRED: ' num2str(DCInfrared(end)) ' - ' num2str(ACInfrared(end)) ' RED: ' num2str(DCRed(end)) ' - ' num2str(ACRed(end))]);    
     

end

fclose(s);
delete(s);


end


