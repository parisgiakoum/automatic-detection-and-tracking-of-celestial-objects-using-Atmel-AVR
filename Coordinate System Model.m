%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                               
%   University: 			Technical University of Crete
%   School:					School of Electrical & Computer Engineering
%   Course:					Embedded Systems - HRY 411
%   Professor:				A. Dollas
%   Authors:     			P. Giakoumakis, N. Ghionis, P.Portokalakis 
%   Create Date: 			29-11-2017
%   Project Name: 			Automatic Detection of Celestial Bodies with Telescope
%   Target Device:			ATMEL AVR ATmega16L
%   Gimbal:					AS20-RS485
%   Development Platform:	ATMEL STUDIO 7
%   Version:				0.1
%
%   Additional Comments:	This MATLAB code is the accompanying file of the main project for better
%                           understanding of the coordinate system used to
%                           find and/or track a celestial body.
%  For more details, see the reports
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%   First Part
%   Source: https://www.mathworks.com/help/map/examples/plotting-a-3-d-dome-as-a-mesh-over-a-globe.html


grs80 = referenceEllipsoid('grs80','km');
domeRadius =  10000;  % km
domeLat =  35.52;       % degrees
domeLon = 24.06;       % degrees
domeAlt = 0;         % km
 
 [x,y,z] = sphere(30);
 xEast  = domeRadius * x;
 yNorth = domeRadius * y;
 zUp    = domeRadius * z;
 zUp(zUp < 0) = 0;
 figure('Renderer','opengl')
 surf(xEast, yNorth, zUp,'FaceColor','yellow','FaceAlpha',0.5)
 axis equal
 
 [xECEF, yECEF, zECEF] ...
     = enu2ecef(xEast, yNorth, zUp, domeLat, domeLon, domeAlt, grs80);
 surf(xECEF, yECEF, zECEF,'FaceColor','yellow','FaceAlpha',0.5)
 axis equal
figure('Renderer','opengl')
ax = axesm('globe','Geoid',grs80,'Grid','on', ...
    'GLineWidth',1,'GLineStyle','-',...
    'Gcolor',[0.9 0.9 0.1],'Galtitude',100);
ax.Position = [0 0 1 1];
axis equal off
view(3)

load topo
geoshow(topo,topolegend,'DisplayType','texturemap')
demcmap(topo)
land = shaperead('landareas','UseGeoCoords',true);
plotm([land.Lat],[land.Lon],'Color','black')
rivers = shaperead('worldrivers','UseGeoCoords',true);
plotm([rivers.Lat],[rivers.Lon],'Color','blue')

surf(xECEF, yECEF, zECEF,'FaceColor','yellow','FaceAlpha',0.5)





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Second Part: Visualization of the data to be used
%                from the avr to track/find a celestial body
%
%   Data taken from the JPL HORIZONS on-line solar system data
%   and ephimeris computation service.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%Import data from a .txt file
data = readtable('data2.txt','Delimiter',' ');

%Organize data into corresponding arrays
azimuth = table2array(data(1:end,4));
elevation = table2array(data(1:end,5));
times = table2array(data(1:end,2));

%Select key timestamps to draw to the plot
middle = times(length(times)/2);
middle2 = middle{1};

azimuth_1st = azimuth(1);
elevation_1st = elevation(1);

azimuth_between_1_and_middle = azimuth(floor((1 + length(azimuth)/2)/2));
elevation_between_1_and_middle = elevation(floor((1 + length(azimuth)/2)/2));
time_between_1_and_middle = times(floor((1 + length(azimuth)/2)/2));

azimuth_middle = azimuth(length(azimuth)/2);
elevation_middle = elevation(length(elevation)/2);

azimuth_between_middle_and_end = azimuth(floor(length(azimuth)/2 + length(azimuth)/2/2));
elevation_between_middle_and_end = elevation(floor(length(azimuth)/2 + length(azimuth)/2/2));
time_between_middle_and_end = times(floor(length(azimuth)/2 + length(azimuth)/2/2));

azimuth_last = azimuth(end)

%Constructing the plot
figure;
stem(azimuth,elevation,':');
text(azimuth(length(azimuth)/2),elevation(length(elevation)/2)+20,times(length(times)/2))
text(azimuth_1st,elevation_1st+20,times(1))
text(azimuth_between_1_and_middle,elevation_between_1_and_middle,time_between_1_and_middle);
text(azimuth_between_middle_and_end,elevation_between_middle_and_end,time_between_middle_and_end);

azim_elev_text = sprintf('Horizontal Coordinate System \n Observation Point - Chania');
title(azim_elev_text);
xlabel('Azimuth (\circ)')
ylabel('Elevation (\circ)')
grid on;
axis([0 370 -100 100])

hold on;


%Plot vertical lines to clearly see selected hours of the day
vline(azimuth_1st)
vline(azimuth_middle)
vline(azimuth_last)
vline(azimuth_between_1_and_middle)
vline(azimuth_between_middle_and_end)
%figure;
%polar(azimuth,elevation);






