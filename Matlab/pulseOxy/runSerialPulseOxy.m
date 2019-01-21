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

showPulseDetectionPlot = true;
showOxyDetectionPlot = false;

%% -- CONSTANTS -------------------------------------------------------- %%
% figure constants
screensize = get(0,'Screensize');
screensize = floor(screensize./2);

% convertions
stepSizeADC = 805e-6; % [V]

% freqeuncy
freq_Hz = 2*10; % [Hz]
timWinPlot_s = nbOfSamplesInWin./freq_Hz;

%% -- prepare plot ----------------------------------------------------- %%
% prepare plot
fig = figure;
set(fig,'Position',screensize);

if showPulseDetectionPlot || showOxyDetectionPlot
    subplot(211)
end
hlDCInfrared = animatedline;
hlACInfrared = animatedline;
hlDCRed = animatedline;
hlACRed = animatedline;

hlDCInfrared.Color = 'b';
hlACInfrared.Color = 'c';
hlDCRed.Color = 'r';
hlACRed.Color = 'm';

legend({'DC Infrared', 'AC Infrared', 'DC Red', 'AC Red'})
grid on;
xlabel('time [s]');
ylabel('measured voltage [V]')

% tracker plot
if showPulseDetectionPlot
    % prepare plot
    subplot(212)
    hold on;
    hlACInfrared2 = animatedline;
    hlTopTrack = animatedline;
    hlBottomTrack = animatedline;
    hlPulseSignal = animatedline;
    hold off;
    
    hlACInfrared2.Color = 'c';    
    hlTopTrack.Color = 'k';
    hlBottomTrack.Color = 'k';
    hlPulseSignal.Color = 'r';
    
    legend({'AC Infrared', 'top', 'bottom', 'pulse signal'})
    grid on;
    xlabel('time [s]');
    ylabel('measured voltage [V]')
    
elseif showOxyDetectionPlot
    
    % prepare plot
    subplot(212)
    hold on;   
    hlACInfrared2 = animatedline;
    hlACRed2 = animatedline;
    hlOxyTopTrackIRed = animatedline;
    hlOxyBottomTrackIRed = animatedline;
    hlOxyTopTrackRed = animatedline;
    hlOxyBottomTrackRed = animatedline;        
    hold off;    
            
    hlACInfrared2.Color = 'c';
    hlACRed2.Color = 'm';
    
    hlOxyTopTrackIRed.Color = 'k';
    hlOxyBottomTrackIRed.Color = 'b';
    hlOxyTopTrackRed.Color = 'k';
    hlOxyBottomTrackRed.Color = 'b';
    
    legend({'AC Infrared', 'AC Red', 'top IR', 'bottom IR', 'top R', 'bottom IR'})
    grid on;
    xlabel('time [s]');
    ylabel('measured voltage [V]')  
    
end

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
     
tic;

%% -- calculate -------------------------------------------------------- %%
n = 0; % initialize

while 1
        
    %% prepare
    n = n+1;

    %%  get raw data
    fprintf(s,'*IDN?');
    timeToc(n) = toc;
    out = fscanf(s);   
    val = textscan(out, '%d,%d,%d,%d');    
    
    if isempty(val{4}) 
        DCInfrared(n) = 0;
        ACInfrared(n) = 0;
        DCRed(n) = 0;
        ACRed(n) = 0;         
    else
        % raw data
        DCInfrared(n) = double(val{1}).*stepSizeADC;
        ACInfrared(n) = double(val{2}).*stepSizeADC;
        DCRed(n) = double(val{3}).*stepSizeADC;
        ACRed(n) = double(val{4}).*stepSizeADC;     
    end
    
    %% update timing
    timeStamp_s(n) = n./freq_Hz;
    timPlotStart_s = max(timeStamp_s(n)-timWinPlot_s,0);
    
    %% calculate oxygen saturation
	[oxySat(n), btOxyIRed(n), ttOxyIRed(n), btOxyRed(n), ttOxyRed(n) ] = calcOxySat(DCInfrared(n),ACInfrared(n),DCRed(n),ACRed(n));
        
    % calc to percent
    oxySat(n) = round(100*oxySat(n));

    %% plausibility oxy sat
    if isnan(oxySat(n))
        oxySat(n) = 0;
    else
        oxySat(n) = min(oxySat(n),100);
    end   
        
    %% calculate pulse
    [estimatedPulse(n), pulseSig, bottomTrackACI(n), topTrackACI(n)]  = calcPulse(ACInfrared(n),timeStamp_s(n));
    
    % convert to double
    pulseSignal(n) = double(pulseSig);
    
    %% plausibility pulse
    if estimatedPulse(n) < 30
        oxySat(n) = 0;
        estimatedPulse(n) = 0;      
    elseif estimatedPulse(n) > 200
        oxySat(n) = 0;
        estimatedPulse(n) = 0;
    end
        
    
    %% update graphes
    if mod(n,updatePlotNbSample) == 0
        
        % update title
        if showPulseDetectionPlot || showOxyDetectionPlot
            subplot(211);
        end
        
        title(['Pulse: ' num2str(estimatedPulse(n)) ' - OxySat: ' num2str(oxySat(n)) ' %']);
        
        addpoints(hlDCInfrared,timeStamp_s(n-updatePlotNbSample+1:n),DCInfrared(n-updatePlotNbSample+1:n));
        addpoints(hlACInfrared,timeStamp_s(n-updatePlotNbSample+1:n),ACInfrared(n-updatePlotNbSample+1:n));
        addpoints(hlDCRed,timeStamp_s(n-updatePlotNbSample+1:n),DCRed(n-updatePlotNbSample+1:n));
        addpoints(hlACRed,timeStamp_s(n-updatePlotNbSample+1:n),ACRed(n-updatePlotNbSample+1:n));
        
        xlim([timPlotStart_s timeStamp_s(n)]);

        if showPulseDetectionPlot
            subplot(2,1,2)           
            addpoints(hlACInfrared2,timeStamp_s(n-updatePlotNbSample+1:n),ACInfrared(n-updatePlotNbSample+1:n));          
            addpoints(hlTopTrack,timeStamp_s(n-updatePlotNbSample+1:n),topTrackACI(n-updatePlotNbSample+1:n));
            addpoints(hlBottomTrack,timeStamp_s(n-updatePlotNbSample+1:n),bottomTrackACI(n-updatePlotNbSample+1:n));
            addpoints(hlPulseSignal,timeStamp_s(n-updatePlotNbSample+1:n),pulseSignal(n-updatePlotNbSample+1:n)); 
            xlim([timPlotStart_s timeStamp_s(n)]);
            
        elseif showOxyDetectionPlot
            subplot(2,1,2)    
            addpoints(hlACInfrared2,timeStamp_s(n-updatePlotNbSample+1:n),ACInfrared(n-updatePlotNbSample+1:n));
            addpoints(hlACRed2,timeStamp_s(n-updatePlotNbSample+1:n),ACRed(n-updatePlotNbSample+1:n));                        
            addpoints(hlOxyTopTrackIRed,timeStamp_s(n-updatePlotNbSample+1:n),ttOxyIRed(n-updatePlotNbSample+1:n));
            addpoints(hlOxyBottomTrackIRed,timeStamp_s(n-updatePlotNbSample+1:n),btOxyIRed(n-updatePlotNbSample+1:n));
            addpoints(hlOxyTopTrackRed,timeStamp_s(n-updatePlotNbSample+1:n),ttOxyRed(n-updatePlotNbSample+1:n));
            addpoints(hlOxyBottomTrackRed,timeStamp_s(n-updatePlotNbSample+1:n),btOxyRed(n-updatePlotNbSample+1:n));
            xlim([timPlotStart_s timeStamp_s(n)]);
            
        end   
        
        drawnow;

    end
    
    %% display raw data
    % disp(['IRED: ' num2str(DCInfrared(end)) ' - ' num2str(ACInfrared(end)) ' RED: ' num2str(DCRed(end)) ' - ' num2str(ACRed(end))]);       

end

fclose(s);
delete(s);


end


